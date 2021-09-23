#ifndef CASBINEX_PG_ADAPTER
#define CASBINEX_PG_ADAPTER

#include "casbin/casbin.h"

namespace casbinex {

// Adapter is the file adapter for Casbin.
// It can load policy from file or save policy to file.
class PgAdapter : virtual public casbin::Adapter {
  std::string connectionString;
    public:

        // NewAdapter is the constructor for Adapter.
        PgAdapter(std::string connectionString);
        virtual ~PgAdapter() final {}

        // LoadPolicy loads all policy rules from the storage.
        virtual void LoadPolicy(const std::shared_ptr<casbin::Model>& model);

        // SavePolicy saves all policy rules to the storage.
        virtual void SavePolicy(const std::shared_ptr<casbin::Model>& model);

        // AddPolicy adds a policy rule to the storage.
        virtual void AddPolicy(std::string sec, std::string p_type, std::vector<std::string> rule);

        // RemovePolicy removes a policy rule from the storage.
        virtual void RemovePolicy(std::string sec, std::string p_type, std::vector<std::string> rule);

        // RemoveFilteredPolicy removes policy rules that match the filter from the storage.
        virtual void RemoveFilteredPolicy(std::string sec, std::string p_type, int field_index, std::vector<std::string> field_values);

        // IsFiltered returns true if the loaded policy has been filtered.
        virtual bool IsFiltered();

    private:
      void  LoadPolicyLine(std::vector<std::string> tokens, const std::shared_ptr<casbin::Model>& model);
};

};  // namespace casbin

#endif
