#include "pg_adapter.h"
#include <casbin/util/util.h>
#include <pqxx/pqxx>

using namespace casbinex;

// LoadPolicyLine loads a text line as a policy rule to model.
void PgAdapter::LoadPolicyLine(std::vector<std::string> tokens, casbin::Model* model) {
    for (int i = 0; i < tokens.size(); i++)
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
  
}

// LoadPolicy loads all policy rules from the storage.
void PgAdapter::LoadPolicy(casbin::Model* model)
{
  pqxx::connection c{connectionString};
  pqxx::work txn{c};
  pqxx::result r{txn.exec("select ptype, v0, v1, v2 from polarity.casbin_rule")};

  for(auto row: r) 
    if(row[3].is_null())
    LoadPolicyLine({
      row[0].c_str(),
      row[1].c_str(),
      row[2].c_str()}, model);
    else
    LoadPolicyLine({
      row[0].c_str(),
      row[1].c_str(),
      row[2].c_str(),
      row[3].c_str()}, model);
}

// SavePolicy saves all policy rules to the storage.
void PgAdapter::SavePolicy(casbin::Model* model)
{


}

// AddPolicy adds a policy rule to the storage.
void PgAdapter::AddPolicy(std::string sec, std::string p_type, std::vector<std::string> rule)
{

}

// RemovePolicy removes a policy rule from the storage.
void PgAdapter::RemovePolicy(std::string sec, std::string p_type, std::vector<std::string> rule)
{

}

// RemoveFilteredPolicy removes policy rules that match the filter from the storage.
void PgAdapter::RemoveFilteredPolicy(std::string sec, std::string p_type, int field_index, std::vector<std::string> field_values)
{

}

// IsFiltered returns true if the loaded policy has been filtered.
bool PgAdapter::IsFiltered()
{
  return this->filtered;
}