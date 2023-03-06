
#include <thread>
#include <chrono>
#include <iostream>
#include <mysql/mysql.h>
#include <ratio>
#include <string>
#include "MysqlConn.h"
#include "ConnectionPool.h"
#include <memory>
#include <stdio.h>

using namespace std::chrono;
using namespace std;
void query() {
    MysqlConn conn;
    bool f1 = conn.connect("root", "123456", "testDb", "172.20.168.40");
    
 

    //插入
    std::string sql = "insert into person values (4, 'tom', 19)";
    bool flag = conn.update(sql);
    std::cout << "flag value: " << flag << std::endl;

    //查询
    sql = "select * from person";
    conn.query(sql);
    std::cout << "第一次查询：" << std::endl;
    while(conn.next()) {
        std::cout << conn.value(0) << ","
        << conn.value(1) << ","
        << conn.value(2) <<std::endl;
    }

    //删除
    //sql = "delete from person where id=1";
    //conn.update(sql);

    //查询

    std::cout << "第二次查询：" << std::endl;
    sql = "select * from person";
    conn.query(sql);
    while(conn.next()) {
        std::cout << conn.value(0) << ","
        << conn.value(1) << ","
        << conn.value(2) <<std::endl;
    }

}

void notUsePool(int begin, int end) {
    for(int i = begin; i < end; i++) {
        MysqlConn mysqlConn;
        
        mysqlConn.connect("root", "123456", "testDb", "172.20.168.40");
        //插入
        char sql[1024] = {0};
        sprintf(sql, "insert into person values (%d, 'tom', 19)", i);
        mysqlConn.update(sql);
    }
}

void usePool(ConnectionPool* pool,int begin, int end) {
    for(int i = begin; i < end; i++) {
        std::shared_ptr<MysqlConn> mysqlConn = pool->getConnection();
        //插入
        char sql[1024] = {0};
        sprintf(sql, "insert into person values (%d, 'tom', 19)", i);
        mysqlConn->update(sql);
    }
}

void test1() {

#if 0
//非连接池单线程
//单线程+非连接池，消耗时间：51257773900ns,51257ms
    steady_clock::time_point begin = steady_clock::now();
    notUsePool(0, 5000);
    steady_clock::time_point end = steady_clock::now();
    nanoseconds length = end - begin;

    cout << "单线程+非连接池，消耗时间：" << length.count() <<"ns,"
    << length.count() / 1000000 << "ms" << endl; 
#else
//连接池单线程
//单线程+连接池，消耗时间：33673419600ns,33673ms
    steady_clock::time_point begin = steady_clock::now();
    //获取线程池实例
    ConnectionPool* pool = ConnectionPool::getConnectionPool();
    usePool(pool, 0, 5000);
    steady_clock::time_point end = steady_clock::now();

    nanoseconds length = end - begin;
    cout << "单线程+连接池，消耗时间：" << length.count() <<"ns,"
    << length.count() / 1000000 << "ms" << endl; 
#endif
}

void test2() {
#if 0
//多线程使用连接池
//多线程+非连接池，消耗时间：47423962100ns,47423ms
    MysqlConn mysqlConn;
    mysqlConn.connect("root", "123456", "testDb", "172.20.168.40");
    steady_clock::time_point begin = steady_clock::now();
    thread t1(notUsePool, 0, 1000);
    thread t2(notUsePool, 1000, 2000);
    thread t3(notUsePool, 2000, 3000);
    thread t4(notUsePool, 3000, 4000);
    thread t5(notUsePool, 4000, 5000);
    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();
    steady_clock::time_point end = steady_clock::now();
    nanoseconds length = end - begin;

    cout << "多线程+非连接池，消耗时间：" << length.count() <<"ns,"
    << length.count() / 1000000 << "ms" << endl; 

#else
//多线程 + 使用连接池，消耗时间：10302495600ns,10302ms
    ConnectionPool* pool = ConnectionPool::getConnectionPool();
    steady_clock::time_point begin = steady_clock::now();
    std::thread t1(usePool, pool, 0, 1000);
    std::thread t2(usePool, pool, 1000, 2000);
    std::thread t3(usePool, pool, 2000, 3000);
    std::thread t4(usePool, pool, 3000, 4000);
    std::thread t5(usePool, pool, 4000, 5000);
    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();
    steady_clock::time_point end = steady_clock::now();
    nanoseconds length = end - begin;

    cout << "多线程 + 使用连接池，消耗时间：" << length.count() <<"ns,"
    << length.count() / 1000000 << "ms" << endl; 

#endif
}


int main() {
    //测试数据库封装是否正确
    //query();

    //测试连接池效率
    //test1();
    //测试多线程下连接池效率
    test2();
    return 1;
}