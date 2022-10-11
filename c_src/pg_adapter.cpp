#include "pg_adapter.h"
#include "migrator.h"
#include <casbin/casbin.h>
#include <strings.h>
#include <iostream>
#include <optional>

using namespace casbinex;

// LoadPolicyLine loads a text line as a policy rule to model.
void PgAdapter::LoadPolicyLine(std::vector<std::string> tokens, const std::shared_ptr<casbin::Model>& model) {
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
connectionString(connectionString),
_pgPool(connectionString.c_str())
{
  this->file_path = connectionString;
  this->filtered = false;

  PoolConnection c{_pgPool};
  casbinex::Migrator::RunMigrations(c.connection());
}

// LoadPolicy loads all policy rules from the storage.
void PgAdapter::LoadPolicy(const std::shared_ptr<casbin::Model>& model)
{

  PoolConnection c{_pgPool};
  pqxx::work txn{ c.connection() };
  pqxx::result r{txn.exec("select ptype, v0, v1, v2, v3, v4, v5, v6 from casbin_rule")};

  #ifdef DEBUG_LOG_QUERIES
    std::cout << "select ptype, v0, v1, v2, v3, v4, v5, v6 from casbin_rule" << std::endl;
  #endif

  model->ClearPolicy();

  for(int jj = 0; jj < r.size(); ++jj){    
    std::vector<std::string> policy_line;

    for(int ii = 0; ii < r.at(jj).size(); ++ii)
      if(!r.at(jj).at(ii).is_null()) policy_line.push_back(r.at(jj).at(ii).c_str());

    LoadPolicyLine(policy_line, model);
  }
}

// SavePolicy saves all policy rules to the storage.
void PgAdapter::SavePolicy(const std::shared_ptr<casbin::Model>& model)
{
  if(!model) return;

  PoolConnection c{_pgPool};
  pqxx::work txn{ c.connection() };

  #ifdef DEBUG_LOG_QUERIES
    std::cout << "truncate table casbin_rule;" << std::endl;
  #endif

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
            ?row.push_back(c.ptr()->esc(rule[ii]))
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
            ?row.push_back(c.ptr()->esc(rule[ii]))
            :row.push_back(std::nullopt);

        stream << row;
      }
    }
  }


  #ifdef DEBUG_LOG_QUERIES
    std::cout << "streamed to table..." << std::endl;
  #endif

  stream.complete();
  txn.commit();
}

// AddPolicy adds a policy rule to the storage.
void PgAdapter::AddPolicy(std::string sec, std::string p_type, std::vector<std::string> rule)
{
  PoolConnection c{_pgPool};
  pqxx::work txn{ c.connection() };
  std::string columns;
  std::string values;

  auto quote = [&c](std::string_view field) {
    return "'" + c.ptr()->esc(field) + "'";
  };
  
  columns = ("ptype");
  values = (quote(p_type));

  if(rule.size() > 0){ columns += ",v0"; values += ", " + quote(rule[0]); }
  if(rule.size() > 1){ columns += ",v1"; values += ", " + quote(rule[1]); }
  if(rule.size() > 2){ columns += ",v2"; values += ", " + quote(rule[2]); }
  if(rule.size() > 3){ columns += ",v3"; values += ", " + quote(rule[3]); }
  if(rule.size() > 4){ columns += ",v4"; values += ", " + quote(rule[4]); }
  if(rule.size() > 5){ columns += ",v5"; values += ", " + quote(rule[5]); }
  if(rule.size() > 6){ columns += ",v6"; values += ", " + quote(rule[6]); }

  std::string query = "INSERT INTO casbin_rule (" + columns +") values (" + values + ");";

  #ifdef DEBUG_LOG_QUERIES
    std::cout << query << std::endl;
  #endif
  
  txn.exec(query);
  txn.commit();
}

// RemovePolicy removes a policy rule from the storage.
void PgAdapter::RemovePolicy(std::string sec, std::string p_type, std::vector<std::string> rule)
{
  PoolConnection c{_pgPool};
  pqxx::work txn{ c.connection() };

  auto quote = [&c](std::string_view field) {
    return "'" + c.ptr()->esc(field) + "'";
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


  #ifdef DEBUG_LOG_QUERIES
    std::cout << query << std::endl;
  #endif

  txn.exec(query);
  txn.commit();
}

// RemoveFilteredPolicy removes policy rules that match the filter from the storage.
void PgAdapter::RemoveFilteredPolicy(std::string sec, std::string p_type, int field_index, std::vector<std::string> field_values)
{
  PoolConnection c{_pgPool};
  pqxx::work txn{ c.connection() };

  auto quote = [&c](std::string_view field) {
    return "'" + c.ptr()->esc(field) + "'";
  };

  std::string query = "DELETE FROM polarity.casbin_rule where ";  

  query += "ptype = " + quote(p_type);
  for( unsigned int ii = static_cast<unsigned int>(field_index) ; ii < field_values.size() + field_index ; ++ii ) {
    query += " AND v" + std::to_string(ii) + " = " +  quote(field_values[ii - field_index]);
  }
  query += ";";

  #ifdef DEBUG_LOG_QUERIES
    std::cout << query << std::endl;
  #endif

  txn.exec(query);
  txn.commit();
}

// IsFiltered returns true if the loaded policy has been filtered.
bool PgAdapter::IsFiltered()
{
  return this->filtered;
}