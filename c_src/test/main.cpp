

#define CATCH_CONFIG_MAIN
#include <casbin/data_types.h>
#include <casbin/casbin.h>
#include <pqxx/pqxx>
#include "../pg_adapter.h"
#include <iostream>
#include "catch.hpp"
#include "config/config.h"


using namespace std;
using namespace casbin;

#define VALID_GROUP_CHANNEL_ACTION { "g:1", "c:-1", "read" }
#define INVALID_GROUP_CHANNEL_ACTION { "g:0", "c:-1", "write" }

TEST_CASE("PGAdapter is created and injected correctly") {
  try{    
    std::shared_ptr<casbin::CachedEnforcer> enforcer = 
    std::shared_ptr<casbin::CachedEnforcer>(new casbin::CachedEnforcer(
      MODEL_PATH, 
      std::shared_ptr<casbinex::PgAdapter>(new casbinex::PgAdapter(POSTGRES_URL))));

    // no exception thrown is a pass
    REQUIRE(!enforcer->IsFiltered()); 
  } catch(std::exception e) {
    std::cout << e.what() << std::endl;
    REQUIRE(false);
  }
}

TEST_CASE("enforces group channel associations correctly") {
  try{    
    std::shared_ptr<casbin::CachedEnforcer> enforcer = 
    std::shared_ptr<casbin::CachedEnforcer>(new casbin::CachedEnforcer(
      MODEL_PATH, 
      std::shared_ptr<casbinex::PgAdapter>(new casbinex::PgAdapter(POSTGRES_URL))));

    REQUIRE(enforcer->Enforce(VALID_GROUP_CHANNEL_ACTION));
    REQUIRE(!enforcer->Enforce(INVALID_GROUP_CHANNEL_ACTION));
  } catch(std::exception e) {
    std::cout << e.what() << std::endl;
    REQUIRE(false);
  }
}
