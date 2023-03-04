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
    
    //获取线程池
    ConnectionPool* getConnectionPool(); 

    //获取连接->(消费者调用)
    std::shared_ptr<MysqlConn> getConnection();  

private:
    ConnectionPool();  

    //解析json文件
    bool parseJasonFile(); 

    //生产连接线程函数（生产者线程）
    void produceConnection(); 

    // 回收连接线程函数
    void recycleConnection();

    //往连接队列添加连接 
    void addConnection(); 

private:
    //连接队列
    std::queue<MysqlConn*> m_connectionQueue;
    //互斥锁
    mutex m_mutex;
    //条件变量
    condition_variable m_cond;
    string m_ip;
    string m_userName;
    string m_passWord;
    string m_dbName;
    unsigned short m_port;
    int m_maxSize;
    int m_minSize;
    int m_timeOut;  //用于阻塞获取连接的线程
    int m_maxConnTimeOut; //用于销毁超时连接

};

#endif