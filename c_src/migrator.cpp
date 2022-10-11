#include "migrator.h"

using namespace casbinex;

void createTable(pqxx::connection& connection) {
  pqxx::work txn{connection};
  txn.exec( R"(
    CREATE TABLE IF NOT EXISTS casbin_rule (
      id serial NOT NULL,
      ptype character varying,
      v0 character varying,
      v1 character varying,
      v2 character varying,
      v3 character varying,
      v4 character varying,
      v5 character varying,
      v6 character varying
    ) WITH ( OIDS=FALSE );
  )");
  txn.commit();
}

void addV6Column(pqxx::connection& connection) {
  pqxx::work txn{connection};
  txn.exec("ALTER TABLE casbin_rule ADD COLUMN v6 character varying;");
  txn.commit();  
}

void Migrator::RunMigrations(pqxx::connection& connection) {
  pqxx::work txn{ connection };
  pqxx::result r{txn.exec("select column_name from information_schema.columns where table_name = 'casbin_rule';")};
  txn.commit();

  { // check of the table exists and has columns
    if(r.size() == 0) {
      createTable(connection);
    }
  }

  { // check if the table has the v6 column
    if(r.size() == 8) {
      addV6Column(connection);
    }
  }

}