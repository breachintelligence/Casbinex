#ifndef PGPOOL_H
#define PGPOOL_H

#include <mutex>
#include "pqxx/pqxx"
#include <memory>
#include <condition_variable>
#include <queue>

namespace casbinex {

class PGPool
{
public:
  PGPool(const char* connectionString);
  std::shared_ptr<pqxx::connection> connection();
  void freeConnection(std::shared_ptr<pqxx::connection>);

private:
  void createPool(const char* connectionString);
  
  std::mutex m_mutex;
  std::condition_variable m_condition;
  std::queue<std::shared_ptr<pqxx::connection>> m_pool;

  const int POOL_SIZE = 4;
};

class PoolConnection {  
  public: 
  
  PoolConnection(PGPool& pool);
  ~PoolConnection();

  std::shared_ptr<pqxx::connection> ptr();
  pqxx::connection& connection();

  private: 
  std::shared_ptr<pqxx::connection> _connection;
  std::function<void (void)> _checkin;
};

}

#endif
