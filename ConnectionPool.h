#ifndef _CONNECTION_POOL_H
#define _CONNECTION_POOL_H

#include <queue>
#include "MysqlConn.h"
#include <mutex>
#include <condition_variable>
#include "json/json.h"


using std::mutex;
using std::condition_variable;

class ConnectionPool {
public:
    ConnectionPool(const ConnectionPool& obj) = delete;
    ConnectionPool& operator=(const ConnectionPool& obj) = delete;
    ConnectionPool* getConnection();

private:
    ConnectionPool();
    bool parseJasonFile();
    void produceConnection();
    void recycleConnection();

private:
    std::queue<MysqlConn*> m_connectionQueue;
    mutex m_mutex;
    condition_variable m_cond;
    string m_ip;
    string m_userName;
    string m_passWord;
    string m_dbName;
    unsigned short m_port;
    int m_maxSize;
    int m_minSize;
    int m_timeOut;
    int m_maxTimeOut; 

};

#endif