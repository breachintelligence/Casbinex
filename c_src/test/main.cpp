

#define CATCH_CONFIG_MAIN
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
  } catch(const pqxx::sql_error& e) {
    std::cout << e.what() << std::endl;
    REQUIRE(false);
  } catch(const std::exception& e) {
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
  } catch(const pqxx::sql_error& e) {
    std::cout << e.what() << std::endl;
    REQUIRE(false);
  } catch(const std::exception& e) {
    std::cout << e.what() << std::endl;
    REQUIRE(false);
  }
}

TEST_CASE("Adds and removes the policy row in the casbin db table") {
  try{    
    std::shared_ptr<casbin::CachedEnforcer> enforcer = 
    std::shared_ptr<casbin::CachedEnforcer>(new casbin::CachedEnforcer(
      MODEL_PATH, 
      std::shared_ptr<casbinex::PgAdapter>(new casbinex::PgAdapter(POSTGRES_URL))));

    enforcer->AddPolicy({"tester", "data1", "read"});
    enforcer->InvalidateCache();
    enforcer->LoadPolicy();
    REQUIRE(enforcer->Enforce({"tester", "data1", "read"}));

    enforcer->RemovePolicy({"tester", "data1", "read"});
    enforcer->InvalidateCache();
    enforcer->LoadPolicy();
    REQUIRE(!enforcer->Enforce({"tester", "data1", "read"}));

    REQUIRE(enforcer->Enforce(VALID_GROUP_CHANNEL_ACTION));
    REQUIRE(!enforcer->Enforce(INVALID_GROUP_CHANNEL_ACTION));
  } catch(const pqxx::sql_error& e) {
    std::cout << e.what() << std::endl;
    REQUIRE(false);
  } catch(const std::exception& e) {
    std::cout << e.what() << std::endl;
    REQUIRE(false);
  }
}

TEST_CASE("Adds and removes groups in the casbin db table") {
  try{    
    std::shared_ptr<casbin::CachedEnforcer> enforcer = 
    std::shared_ptr<casbin::CachedEnforcer>(new casbin::CachedEnforcer(
      MODEL_PATH, 
      std::shared_ptr<casbinex::PgAdapter>(new casbinex::PgAdapter(POSTGRES_URL))));

    enforcer->AddPolicy({"admin_data1", "data1", "write"});
    enforcer->AddRoleForUser("tester", "admin_data1");
    enforcer->InvalidateCache();
    enforcer->LoadPolicy();
    REQUIRE(enforcer->Enforce({"tester", "data1", "write"}));

    REQUIRE(enforcer->Enforce(VALID_GROUP_CHANNEL_ACTION));
    REQUIRE(!enforcer->Enforce(INVALID_GROUP_CHANNEL_ACTION));
  } catch(const pqxx::sql_error& e) {
    std::cout << e.what() << std::endl;
    REQUIRE(false);
  } catch(const std::exception& e) {
    std::cout << e.what() << std::endl;
    REQUIRE(false);
  }
}

TEST_CASE("Properly saves the entire policy to the db") {
  try{    
    std::shared_ptr<casbin::CachedEnforcer> enforcer = 
    std::shared_ptr<casbin::CachedEnforcer>(new casbin::CachedEnforcer(
      MODEL_PATH, 
      std::shared_ptr<casbinex::PgAdapter>(new casbinex::PgAdapter(POSTGRES_URL))));

      REQUIRE(enforcer->Enforce(VALID_GROUP_CHANNEL_ACTION));
      enforcer->SavePolicy();
      enforcer->InvalidateCache();
      enforcer->LoadPolicy();
      REQUIRE(enforcer->Enforce(VALID_GROUP_CHANNEL_ACTION));
  } catch(const pqxx::sql_error& e) {
    std::cout << e.what() << std::endl;
    REQUIRE(false);
  } catch(const std::exception& e) {
    std::cout << e.what() << std::endl;
    REQUIRE(false);
  }
}
