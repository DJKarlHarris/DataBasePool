#include "ConnectionPool.h"
#include "MysqlConn.h"
#include <fstream>
#include <json/reader.h>
#include <json/value.h>
#include <thread>
#include <mutex>
#include <memory>

using std::ifstream;
using namespace Json;
using std::thread;
using std::mutex;
using std::unique_lock;
using namespace std::chrono;

ConnectionPool* ConnectionPool::getConnectionPool() {
    static ConnectionPool pool;
    return &pool;
}

ConnectionPool::ConnectionPool() {
    //解析json文件
    if(parseJasonFile()) {
        return;
    }
    for(int i = 0; i < m_minSize; i++) {
        addConnection();
    }
    //生产和回收线程
    thread producer(&ConnectionPool::produceConnection, this);
    thread recycler(&ConnectionPool::recycleConnection, this);
    //主线程不能有阻塞状态->不能调用join来回收线程，所以将线程设置为分离线程
    producer.detach();
    recycler.detach();

}

ConnectionPool::~ConnectionPool() {
    while(!m_connectionQueue.empty()) {
        MysqlConn* conn = m_connectionQueue.front();
        m_connectionQueue.pop();
        delete(conn);
    }
}

std::shared_ptr<MysqlConn> ConnectionPool::getConnection() {
    //队列为空->阻塞
    unique_lock<mutex> locker(m_mutex);
    while(m_connectionQueue.empty()) {
        //使用wait_for阻塞进程一段时间
        //线程面对两种唤醒
        //1.超时唤醒：直接回到循环起始点
        //2.新连接到来唤醒:直接break

        if(std::cv_status::timeout 
        == m_cond.wait_for(locker, milliseconds(m_timeOut))) {

            if(m_connectionQueue.empty()) {
                //回到循环起始点判断是否继续阻塞
                continue;
            } 
        } else {
            //非超时唤醒，直接退出循环
            break;
        }
    }
    
    //获取连接
    //注意模板中写的是指针指向的类型而不是指针！
    //利用RAII来管理连接资源
    std::shared_ptr<MysqlConn> connPtr (m_connectionQueue.front(), 
    [this](MysqlConn* conn){
        //利用shared_ptr的删除器来归还连接资源
        std::lock_guard<mutex> locker(m_mutex);
        conn->flushTimePoint();
        m_connectionQueue.push(conn);
    });
    m_connectionQueue.pop();

    //由于共用同一个条件变量，所以会唤醒生产者或消费者，但不影响逻辑
    m_cond.notify_all();
    
    return connPtr;
}

void ConnectionPool::produceConnection() {
    while(true) {
        unique_lock<mutex> locker(m_mutex);
        //阻塞条件：连接数 > 最小连接数 
        //唤醒：1.消费者被唤醒，2.不满足阻塞条件被唤醒
        while(m_connectionQueue.size() > m_minSize) {
            m_cond.wait(locker);
        }

        //有可能当前连接数为m_maxSize时被唤醒，此时不能再生产连接了
        if(m_connectionQueue.size() < m_maxSize) {
            addConnection();
        }

        //唤醒阻塞的消费者线程
        m_cond.notify_all();
    }
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
        m_maxConnTimeOut = root["maxConnTimeOut"].asInt();
        return true;
    }
    return false;
}

void ConnectionPool::recycleConnection() {

    while(true) {
        //每隔0.5s查一次队头连接是否超时
        std::this_thread::sleep_for(milliseconds(500));

        std::lock_guard<mutex> locker(m_mutex);
        while(m_connectionQueue.size() > m_minSize) {

            MysqlConn* mysqlConn = m_connectionQueue.front();
            //判断是否超时
            if(mysqlConn->getAliveTime() > m_maxConnTimeOut) {
                m_connectionQueue.pop();
                delete mysqlConn;
            } else {
                //队头元素还未超时，那么后面的肯定也未超时，所以跳出循环
                break;
            }
        }
    }
    
}

void ConnectionPool::addConnection() {
    MysqlConn* mysqlConn = new MysqlConn;
    mysqlConn->connect(m_userName, m_passWord, m_dbName, m_ip);

    //刷新新插入连接的时间戳
    mysqlConn->flushTimePoint();
    m_connectionQueue.push(mysqlConn);
}
