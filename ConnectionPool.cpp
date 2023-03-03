#include "ConnectionPool.h"
#include "MysqlConn.h"
#include <fstream>
#include <json/reader.h>
#include <json/value.h>
#include <thread>
#include <mutex>

using std::ifstream;
using namespace Json;
using std::thread;
using std::mutex;
using std::unique_lock;



ConnectionPool* ConnectionPool::getConnection() {
    static ConnectionPool pool;
    return &pool;
}

ConnectionPool::ConnectionPool() {
    if(parseJasonFile()) {
        return;
    }
    for(int i = 0; i < m_minSize; i++) {
        addConnection();
    }
    thread producer(&ConnectionPool::produceConnection, this);
    thread recycler(&ConnectionPool::recycleConnection, this);
    producer.detach();
    recycler.detach();

}

bool ConnectionPool::parseJasonFile() {
    ifstream ifs("dbconf.json");
    Reader rd;
    Value root;
    rd.parse(ifs, root);
    if(root.isObject()) {
        m_ip = root["ip"].asString();
        m_port = root["port"].asUInt();
        m_userName = root["userName"].asString();
        m_passWord = root["passWord"].asString();
        m_dbName = root["dbName"].asString();
        m_maxSize = root["maxSize"].asInt();
        m_minSize = root["minSize"].asInt();
        m_timeOut = root["timeOut"].asInt();
        m_maxTimeOut = root["maxTimeOut"].asInt();
        return true;
    }
    return false;
}

//添加连接线程
void ConnectionPool::produceConnection() {
    while(true) {
        unique_lock<mutex> locker(m_mutex);
        while(m_connectionQueue.size() > m_minSize) {
            m_cond.wait(locker);
        }
        addConnection();
    }
}

void ConnectionPool::recycleConnection() {
    while(true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        while(m_connectionQueue.size() > m_minSize) {
            MysqlConn* mysqlConn = m_connectionQueue.front();
            if(mysqlConn->getAliveTime() > m_maxTimeOut) {
                m_connectionQueue.pop();
                delete mysqlConn;
            } else {
                //队头元素还未超时，那么后面的肯定也未超时，所以跳出循环
                break;
            }
        }
    }
    
}

//添加数据库连接
void ConnectionPool::addConnection() {
    MysqlConn* mysqlConn = new MysqlConn;
    mysqlConn->connect(m_userName, m_passWord, m_dbName, m_ip);
    mysqlConn->flushTimePoint();
    m_connectionQueue.push(mysqlConn);

}
