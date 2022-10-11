#ifndef CASBINEX_MIGRATOR_H
#define CASBINEX_MIGRATOR_H


#include "pqxx/pqxx"
#include "string"

namespace casbinex {

  class Migrator {
    public:
    static void RunMigrations(pqxx::connection& connection);
  };


};

#endif
