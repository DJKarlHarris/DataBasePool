#include "ConnectionPool.h"
#include "MysqlConn.h"
#include <fstream>
#include <json/reader.h>
#include <json/value.h>
#include <thread>

using std::ifstream;
using namespace Json;
using std::thread;


ConnectionPool* ConnectionPool::getConnection() {
    static ConnectionPool pool;
    return &pool;
}

ConnectionPool::ConnectionPool() {
    if(parseJasonFile()) {
        return;
    }
    for(int i = 0; i < m_minSize; i++) {
        MysqlConn* mysqlConn = new MysqlConn;
        mysqlConn->connect(m_userName, m_passWord, m_dbName, m_ip);
        m_connectionQueue.push(mysqlConn);
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
