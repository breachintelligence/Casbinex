#include <erl_nif.h>
#include <string>
#include <pqxx/pqxx>
#include "casbin/casbin.h"
#include "pg_adapter.h"

#define PF_ATOM_TRUE enif_make_atom(env, "true")
#define PF_ATOM_FALSE enif_make_atom(env, "false")
#define PF_ATOM_OK enif_make_atom(env, "ok")
#define PF_ATOM_ERROR enif_make_atom(env, "error")
#define PF_MAKE_STRING(a) enif_make_string(env, a, ERL_NIF_LATIN1)
#define make_result_tuple(a,b) enif_make_tuple(env, 2, a, b)

using namespace std;
casbin::CachedEnforcer* enforcer = nullptr;


string ListToString(ErlNifEnv* env, ERL_NIF_TERM term) {
  ERL_NIF_TERM maybeList = term;

  unsigned int listLength = 0;
  if(!enif_is_list(env, maybeList) || !enif_get_list_length(env, maybeList, &listLength)){
    return string();
  }

  char stringData[++listLength];
  int res = 0;
  if ((res = enif_get_string(env, maybeList, stringData, sizeof(stringData), ERL_NIF_LATIN1)) < 1) {
    return string();
  }

  --res; // don't count terminator
  return string(stringData, res);
}

ERL_NIF_TERM VectorToList(ErlNifEnv* env, const vector<string> &input) {
  ERL_NIF_TERM enifList[input.size()];

  for(int ii = 0; ii < input.size(); ++ii) {
    enifList[ii] = enif_make_string(env, input.at(ii).c_str(), ERL_NIF_LATIN1);
  }

  return enif_make_list_from_array(env, enifList, input.size());
}

ERL_NIF_TERM VectorToList(ErlNifEnv* env, const vector<vector<string>>&input) {
  ERL_NIF_TERM enifList[input.size()];

  for(int ii = 0; ii < input.size(); ++ii) {
    enifList[ii] = VectorToList(env, input.at(ii));
  }

  return enif_make_list_from_array(env, enifList, input.size());
}

vector<string> ListToVector(ErlNifEnv* env, ERL_NIF_TERM term) {
  vector<string> resultVector;
  unsigned int length;
  if(!enif_is_list(env, term) || !enif_get_list_length(env, term, &length)){
    return resultVector;
  }
  
  int idx = 0;
  ERL_NIF_TERM item, items;
  items = term;

  while(enif_get_list_cell(env, items, &item, &items))    
    resultVector.push_back(ListToString(env, item));

  return resultVector;
}

ERL_NIF_TERM CreateEnforcer(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]) {
  string modelPath  = ListToString(env, argv[0]);
  string policyPath = ListToString(env, argv[1]);

  try{

    if(policyPath.substr(0, strlen("postgres")) == "postgres"){    
      enforcer = new casbin::CachedEnforcer(
        modelPath, 
        std::shared_ptr<casbinex::PgAdapter>(
          new casbinex::PgAdapter(policyPath)));
    } else {
      enforcer = new casbin::CachedEnforcer(modelPath, policyPath);
    }

  } catch(const casbin::MissingRequiredSections &e) {
    return make_result_tuple( PF_ATOM_ERROR, PF_MAKE_STRING("Missing required sections"));
  } catch(const casbin::CasbinEnforcerException &e) {
    return make_result_tuple( PF_ATOM_ERROR, PF_MAKE_STRING("Casbin Enforcer exception"));
  } catch(const casbin::CasbinRBACException &e) {
    return make_result_tuple( PF_ATOM_ERROR, PF_MAKE_STRING("RBAC exception"));
  } catch(std::logic_error e) {
    return  make_result_tuple( PF_ATOM_ERROR, PF_MAKE_STRING(e.what()));
  } catch(std::runtime_error e) {
    return  make_result_tuple( PF_ATOM_ERROR, PF_MAKE_STRING(e.what()));
  } catch(std::exception e) {
    return  make_result_tuple( PF_ATOM_ERROR, PF_MAKE_STRING(e.what()));
  }

  return make_result_tuple( PF_ATOM_OK, PF_MAKE_STRING("Created Enforcer"));
}

ERL_NIF_TERM DestroyEnforcer(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]) {
  if(enforcer) delete enforcer;
  enforcer = nullptr;
  return PF_ATOM_OK;
}

ERL_NIF_TERM Enforce(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]) {
  if(!enforcer) {
    return make_result_tuple( PF_ATOM_ERROR, PF_MAKE_STRING("Must create and enforcer first."));
  }

  string sub = ListToString(env, argv[0]);
  string obj = ListToString(env, argv[1]);
  string act = ListToString(env, argv[2]);

  try{
    return enforcer->Enforce({sub.c_str(), obj.c_str(), act.c_str()})
      ?PF_ATOM_TRUE
      :PF_ATOM_FALSE;
  } catch(...) {
    return PF_ATOM_FALSE;
  }
}

ERL_NIF_TERM AddPolicy(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]) {
  if(!enforcer) {
    return make_result_tuple( PF_ATOM_ERROR, PF_MAKE_STRING("Must create and enforcer first."));
  }

  string sub = ListToString(env, argv[0]);
  string obj = ListToString(env, argv[1]);
  string act = ListToString(env, argv[2]);

  try{
    enforcer->InvalidateCache();
    return enforcer->AddPolicy({sub.c_str(), obj.c_str(), act.c_str()})
      ?PF_ATOM_TRUE
      :PF_ATOM_FALSE;
  } catch(...) {
    return PF_ATOM_FALSE;
  }
}

ERL_NIF_TERM RemovePolicy(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]) {
  if(!enforcer) {
    return make_result_tuple( PF_ATOM_ERROR, PF_MAKE_STRING("Must create and enforcer first."));
  }

  string sub = ListToString(env, argv[0]);
  string obj = ListToString(env, argv[1]);
  string act = ListToString(env, argv[2]);

  try{
    enforcer->InvalidateCache();
    return enforcer->RemovePolicy({sub.c_str(), obj.c_str(), act.c_str()})
      ? PF_ATOM_TRUE
      : PF_ATOM_FALSE;
  } catch(...) {
    return PF_ATOM_FALSE;
  }
}

ERL_NIF_TERM AddGroupingPolicy(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]) {
  if(!enforcer) {
    return make_result_tuple( PF_ATOM_ERROR, PF_MAKE_STRING("Must create and enforcer first."));
  }

  string user  = ListToString(env, argv[0]);
  string group = ListToString(env, argv[1]);

  try{
    enforcer->InvalidateCache();
    return enforcer->AddGroupingPolicy({user, group})
      ?PF_ATOM_TRUE
      :PF_ATOM_FALSE;
  } catch(...) {
    return PF_ATOM_FALSE;
  }
}

ERL_NIF_TERM GetUsersForRole(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]) {
  if(!enforcer) {
    return make_result_tuple( PF_ATOM_ERROR, PF_MAKE_STRING("Must create and enforcer first."));
  }

  string role = ListToString(env, argv[0]);
  try{
    return VectorToList(env, enforcer->GetUsersForRole(role));
  } catch(const casbin::CasbinRBACException &e) {
    return make_result_tuple( PF_ATOM_ERROR, PF_MAKE_STRING("RBAC exception"));
  } catch(...) {
    return make_result_tuple( PF_ATOM_ERROR, PF_MAKE_STRING("Unexpected exception"));
  }
}

ERL_NIF_TERM GetRolesForUser(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]) {
  if(!enforcer) {
    return make_result_tuple( PF_ATOM_ERROR, PF_MAKE_STRING("Must create and enforcer first."));
  }

  string user = ListToString(env, argv[0]);
  try{
    return VectorToList(env, enforcer->GetRolesForUser(user));
  } catch(...) {
    return make_result_tuple( PF_ATOM_ERROR, PF_MAKE_STRING("Unexpected exception"));
  }
}

ERL_NIF_TERM DeleteRole(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]) {
  if(!enforcer) {
    return make_result_tuple( PF_ATOM_ERROR, PF_MAKE_STRING("Must create and enforcer first."));
  }

  string role = ListToString(env, argv[0]);
  try{
    enforcer->InvalidateCache();
    return enforcer->DeleteRole({role})
      ?PF_ATOM_TRUE
      :PF_ATOM_FALSE;
  } catch(const casbin::CasbinRBACException &e) {
    return make_result_tuple( PF_ATOM_ERROR, PF_MAKE_STRING("RBAC exception"));
  } catch(...) {
    return make_result_tuple( PF_ATOM_ERROR, PF_MAKE_STRING("Unexpected exception"));
  }
}

ERL_NIF_TERM AddRoleForUser(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]) {
  if(!enforcer) {
    return make_result_tuple( PF_ATOM_ERROR, PF_MAKE_STRING("Must create and enforcer first."));
  }

  string user = ListToString(env, argv[0]);
  string role = ListToString(env, argv[1]);
  try{
    enforcer->InvalidateCache();
    return enforcer->AddRoleForUser(user, role)
      ?PF_ATOM_TRUE
      :PF_ATOM_FALSE;
  } catch(const casbin::CasbinRBACException &e) {
    return make_result_tuple( PF_ATOM_ERROR, PF_MAKE_STRING("RBAC exception"));
  } catch(...) {
    return make_result_tuple( PF_ATOM_ERROR, PF_MAKE_STRING("Unexpected exception"));
  }
}

ERL_NIF_TERM AddRolesForUser(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]) {
  if(!enforcer) {
    return make_result_tuple( PF_ATOM_ERROR, PF_MAKE_STRING("Must create and enforcer first."));
  }

  string user = ListToString(env, argv[0]);
  vector<string> roles = ListToVector(env, argv[1]);
  try{
    enforcer->InvalidateCache();
    return enforcer->AddRolesForUser(user, roles)
      ?PF_ATOM_TRUE
      :PF_ATOM_FALSE;
  } catch(const casbin::CasbinRBACException &e) {
    return make_result_tuple( PF_ATOM_ERROR, PF_MAKE_STRING("RBAC exception"));
  } catch(...) {
    return make_result_tuple( PF_ATOM_ERROR, PF_MAKE_STRING("Unexpected exception"));
  }
}

ERL_NIF_TERM DeleteRolesForUser(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]) {
  if(!enforcer) {
    return make_result_tuple( PF_ATOM_ERROR, PF_MAKE_STRING("Must create and enforcer first."));
  }

  string user = ListToString(env, argv[0]);
  try{
    enforcer->InvalidateCache();
    return enforcer->DeleteRolesForUser(user)
      ?PF_ATOM_TRUE
      :PF_ATOM_FALSE;
  } catch(const casbin::CasbinRBACException &e) {
    return make_result_tuple( PF_ATOM_ERROR, PF_MAKE_STRING("RBAC exception"));
  } catch(...) {
    return make_result_tuple( PF_ATOM_ERROR, PF_MAKE_STRING("Unexpected exception"));
  }
}


ERL_NIF_TERM DeleteRoleForUser(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]) {
  if(!enforcer) {
    return make_result_tuple( PF_ATOM_ERROR, PF_MAKE_STRING("Must create and enforcer first."));
  }

  string user = ListToString(env, argv[0]);
  string role = ListToString(env, argv[1]);
  try{
    enforcer->InvalidateCache();
    return enforcer->DeleteRoleForUser(user, role)
      ?PF_ATOM_TRUE
      :PF_ATOM_FALSE;
  } catch(const casbin::CasbinRBACException &e) {
    return make_result_tuple( PF_ATOM_ERROR, PF_MAKE_STRING("RBAC exception"));
  } catch(std::exception e2) {
    return make_result_tuple( PF_ATOM_ERROR, PF_MAKE_STRING(e2.what()));
  }
}

ERL_NIF_TERM RemoveFilteredPolicy(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]) {
  if(!enforcer) {
    return make_result_tuple( PF_ATOM_ERROR, PF_MAKE_STRING("Must create and enforcer first."));
  }

  int fieldIndex = 0;
  if(!enif_get_int(env, argv[0], &fieldIndex)) 
    return enif_make_badarg(env);

  vector<string> resourceId = ListToVector(env, argv[1]);

  try{
    enforcer->InvalidateCache();
    return enforcer->RemoveFilteredPolicy(fieldIndex, resourceId)
      ?PF_ATOM_TRUE
      :PF_ATOM_FALSE;
  } catch(...) {
    return PF_ATOM_FALSE;
  }
}


ERL_NIF_TERM GetFilteredPolicy(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]) {
  if(!enforcer) {
    return make_result_tuple( PF_ATOM_ERROR, PF_MAKE_STRING("Must create and enforcer first."));
  }

  int fieldIndex = 0;
  if(!enif_get_int(env, argv[0], &fieldIndex)) 
    return enif_make_badarg(env);

  vector<string> resourceId = ListToVector(env, argv[1]);

  try{
    return VectorToList(env, enforcer->GetFilteredPolicy(fieldIndex, resourceId));
  } catch(...) {
    return PF_ATOM_FALSE;
  }
}

ErlNifFunc casbinex_nif_funcs[] = 
{ 
    {"createEnforcer",       2, CreateEnforcer},
    {"destroyEnforcer",      0, DestroyEnforcer},
    {"enforce",              3, Enforce},
    {"addPolicy",            3, AddPolicy},
    {"removePolicy",         3, RemovePolicy},
    {"addGroupingPolicy",    2, AddGroupingPolicy},
    {"getUsersForRole",      1, GetUsersForRole},
    {"getRolesForUser",      1, GetRolesForUser},
    {"deleteRole",           1, DeleteRole},
    {"addRoleForUser",       2, AddRoleForUser},
    {"addRolesForUser",      2, AddRolesForUser},
    {"deleteRoleForUser",    2, DeleteRoleForUser},
    {"deleteRolesForUser",   1, DeleteRolesForUser},
    {"removeFilteredPolicy", 2, RemoveFilteredPolicy},
    {"getFilteredPolicy",    2, GetFilteredPolicy},
};

ERL_NIF_INIT(Elixir.Casbinex, casbinex_nif_funcs, 0, 0, 0, 0);
