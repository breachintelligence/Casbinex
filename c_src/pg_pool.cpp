#include "pg_pool.h"

using namespace casbinex;

PoolConnection::PoolConnection(PGPool& pool):
  _connection{pool.connection()},
  _checkin{[this, &pool](){pool.freeConnection(_connection);}}{}
PoolConnection::~PoolConnection(){ _checkin(); }
std::shared_ptr<pqxx::connection> PoolConnection::ptr() {return _connection;}
pqxx::connection& PoolConnection::connection() {return reinterpret_cast<pqxx::connection&>(*_connection.get());}


PGPool::PGPool(const char* connectionString)
{
  createPool(connectionString);
}

void PGPool::createPool(const char* connectionString)
{
  std::lock_guard<std::mutex> locker_( m_mutex );

  for (auto i=0; i < POOL_SIZE; i++) {
    m_pool.emplace(std::make_shared<pqxx::connection>(connectionString));
  }
}

std::shared_ptr<pqxx::connection> PGPool::connection()
{
  std::unique_lock<std::mutex> lock_(m_mutex);

  // if pool is empty, then wait until it notifies back
  while (m_pool.empty()) {
    m_condition.wait(lock_);
  }

  // get new connection in queue
  auto conn_ = m_pool.front();
  // immediately pop as we will use it now
  m_pool.pop();

  return conn_;
}

void PGPool::freeConnection(std::shared_ptr<pqxx::connection> conn_)
{
  std::unique_lock<std::mutex> lock_(m_mutex);

  // push a new connection into a pool
  m_pool.push(conn_);

  // unlock mutex
  lock_.unlock();

  // notify one of thread that is waiting
  m_condition.notify_one();
}
