#ifndef _CONNECTION_POOL_H
#define _CONNECTION_POOL_H

#include <queue>
#include "MysqlConn.h"
#include <mutex>
#include <condition_variable>

using std::mutex;
using std::condition_variable;

class ConnectionPool {
public:
    ConnectionPool(const ConnectionPool& obj) = delete;
    ConnectionPool& operator=(const ConnectionPool& obj) = delete;
    ConnectionPool* getConnection();

private:
    ConnectionPool();

private:
    std::queue<MysqlConn*> m_connectionQueue;
    mutex m_mutex;
    condition_variable m_cond;
    string m_ip;
    string m_user;
    string m_passwd;
    string m_dbName;
    unsigned short port;
    int m_maxSize;
    int m_minSize;
    int m_nowTimeOut;
    int m_maxTimeOut; 

};

#endif