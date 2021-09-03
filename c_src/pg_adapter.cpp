#include "pg_adapter.h"
#include <casbin/util/util.h>
#include <casbin/exception/unsupported_operation_exception.h>
#include <pqxx/pqxx>
#include <strings.h>
#include <iostream>
#include <optional>

using namespace casbinex;

// LoadPolicyLine loads a text line as a policy rule to model.
void PgAdapter::LoadPolicyLine(std::vector<std::string> tokens, casbin::Model* model) {
    for (std::size_t i = 0; i < tokens.size(); i++)
        tokens[i] = casbin::Trim(tokens[i]);

    std::string key = tokens[0];
    std::string sec = key.substr(0,1);
    std::vector<std::string> new_tokens(tokens.begin()+1, tokens.end());

    if (model->m.find(sec) == model->m.end())
        model->m[sec] = casbin::AssertionMap();

    (model->m[sec].assertion_map[key]->policy).push_back(new_tokens);
}

// NewAdapter is the constructor for Adapter.
PgAdapter::PgAdapter(std::string connectionString):
connectionString(connectionString)
{
  this->file_path = connectionString;
  this->filtered = false;
  pqxx::connection c{connectionString};
  
  pqxx::work txn{c};
  txn.exec( R"(CREATE TABLE IF NOT EXISTS casbin_rule (
    id integer NOT NULL,
    ptype character varying,
    v0 character varying,
    v1 character varying,
    v2 character varying,
    v3 character varying,
    v4 character varying,
    v5 character varying,
    v6 character varying
    );)"
  );

  txn.commit();
}

// LoadPolicy loads all policy rules from the storage.
void PgAdapter::LoadPolicy(casbin::Model* model)
{

  pqxx::connection c{connectionString};
  pqxx::work txn{c};
  pqxx::result r{txn.exec("select ptype, v0, v1, v2, v3, v4, v5, v6 from casbin_rule")};

  model->ClearPolicy();

  for(auto row: r){
    std::vector<std::string> policy_line;

    for(int ii = 0; ii < row.size(); ++ii)
      if(!row[ii].is_null()) policy_line.push_back(row[ii].c_str());

    LoadPolicyLine(policy_line, model);
  }
}

// SavePolicy saves all policy rules to the storage.
void PgAdapter::SavePolicy(casbin::Model* model)
{
  if(!model) return;

  pqxx::connection c{connectionString};
  pqxx::work txn{c};

  txn.exec("truncate table casbin_rule;");
  pqxx::stream_to stream = pqxx::stream_to::table(
    txn,
    {"casbin_rule"},
    {"ptype", "v0", "v1", "v2", "v3", "v4", "v5", "v6"}
  );


  std::vector<std::vector<std::string>> policyRows;
  {
    casbin::AssertionMap astMap = model->m["p"];
    for(auto entry: astMap.assertion_map){
      std::string ptype = entry.first;
      for(auto rule: entry.second->policy){
        std::vector<std::optional<std::string>> row;
        row.push_back(ptype);

        for(std::size_t ii = 0; ii <=6; ++ii)
          (rule.size() > ii) 
            ?row.push_back(c.esc(rule[ii]))
            :row.push_back(std::nullopt);

        stream << row;
      }
    }
  }
  {
    casbin::AssertionMap astMap = model->m["g"];
    for(auto entry: astMap.assertion_map){
      std::string ptype = entry.first;
      for(auto rule: entry.second->policy){
        std::vector<std::optional<std::string>> row;
        row.push_back(ptype);

        for(std::size_t ii = 0; ii <= 6; ++ii) 
          (rule.size() > ii)
            ?row.push_back(c.esc(rule[ii]))
            :row.push_back(std::nullopt);

        stream << row;
      }
    }
  }

  stream.complete();
  txn.commit();
}

// AddPolicy adds a policy rule to the storage.
void PgAdapter::AddPolicy(std::string sec, std::string p_type, std::vector<std::string> rule)
{
  pqxx::connection c{connectionString};
  pqxx::work txn{c};
  pqxx::params values;

  c.prepare("insert_row", "INSERT INTO casbin_rule ( ptype, v0, v1, v2, v3, v4, v5, v6 ) values ( $1, $2, $3, $4, $5, $6, $7, $8);");
  
  values.append(c.esc(p_type));
  for(std::string v : rule) 
    values.append(c.esc(v));

  while(values.size() < 8)
    values.append(std::nullopt);

  txn.exec_prepared("insert_row", values);
  txn.commit();
}

// RemovePolicy removes a policy rule from the storage.
void PgAdapter::RemovePolicy(std::string sec, std::string p_type, std::vector<std::string> rule)
{

  pqxx::connection c{connectionString};
  pqxx::work txn{c};

  auto quote = [&c](std::string_view field) {
    return "'" + c.esc(field) + "'";
  };

  std::string query = "DELETE FROM polarity.casbin_rule where ";
  
  query += "ptype = " + quote(p_type);
  query += (rule.size() > 0)? " AND v0 = " + quote(rule[0]) : " AND v0 is NULL ";
  query += (rule.size() > 1)? " AND v1 = " + quote(rule[1]) : " AND v1 is NULL ";
  query += (rule.size() > 2)? " AND v2 = " + quote(rule[2]) : " AND v2 is NULL ";
  query += (rule.size() > 3)? " AND v3 = " + quote(rule[3]) : " AND v3 is NULL ";
  query += (rule.size() > 4)? " AND v4 = " + quote(rule[4]) : " AND v4 is NULL ";
  query += (rule.size() > 5)? " AND v5 = " + quote(rule[5]) : " AND v5 is NULL ";
  query += (rule.size() > 6)? " AND v6 = " + quote(rule[6]) : " AND v6 is NULL ";
  query += ";";

  txn.exec(query);
  txn.commit();
}

// RemoveFilteredPolicy removes policy rules that match the filter from the storage.
void PgAdapter::RemoveFilteredPolicy(std::string sec, std::string p_type, int field_index, std::vector<std::string> field_values)
{
  throw casbin::UnsupportedOperationException("not implemented");
}

// IsFiltered returns true if the loaded policy has been filtered.
bool PgAdapter::IsFiltered()
{
  return this->filtered;
}