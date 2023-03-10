#include "MysqlConn.h"
#include <cstdlib>
#include <mysql/mysql.h>
#include <chrono>
#include <iostream>

using namespace std::chrono;

MysqlConn::MysqlConn() {
    m_conn = mysql_init(nullptr);
    mysql_set_character_set(m_conn, "utf8");
}

MysqlConn::~MysqlConn() {
    if(m_conn != nullptr) {
        mysql_close(m_conn);
    }
    freeResult();
}

bool MysqlConn::connect(string user, string passwd, string dbName, string ip, unsigned short port) {

   MYSQL* ptr = mysql_real_connect(m_conn, ip.c_str(), user.c_str(), passwd.c_str(), dbName.c_str(), port, nullptr, 0);
    if(ptr == nullptr) {
        std::cout << mysql_error(m_conn) << std::endl;
    }
    return ptr != nullptr;
}

bool MysqlConn::update(string sql) {
   
    if(mysql_query(m_conn, sql.c_str())) {
        return false;
    }
    return true;
}

bool MysqlConn::query(string sql) {
    freeResult();
    if(mysql_query(m_conn, sql.c_str())) {
        return false;
    }
    m_result = mysql_store_result(m_conn);
    return true;
}

//依次获取一行
bool MysqlConn::next() {
    if(m_conn != nullptr) {
        m_row = mysql_fetch_row(m_result);
        if(m_row != nullptr) {
            return true;
        }
    }
    return false;
}


string MysqlConn::value(int index) {
    int rowSum = mysql_num_fields(m_result);
    if(index >= rowSum || index < 0) {
        return string(); 
    }
    char* val = m_row[index];
    unsigned long length = mysql_fetch_lengths(m_result)[index];
    return string(val, length);
}

bool MysqlConn::transaction() {
    return mysql_autocommit(m_conn, false);
}

bool MysqlConn::commit() {
    return mysql_commit(m_conn);
}

bool MysqlConn::rollback() {
    return mysql_rollback(m_conn);
}

void MysqlConn::freeResult() {
    if(m_result != nullptr) {
        mysql_free_result(m_result);
        m_result = nullptr;
    }
}

void MysqlConn::flushTimePoint() {
    m_timePoint = steady_clock::now(); 
}

long long MysqlConn::getAliveTime() {
    //获取超时时长
    nanoseconds nanosec = steady_clock::now() - m_timePoint;
    //转换为毫秒
    milliseconds millsec = duration_cast<milliseconds>(nanosec);
    return millsec.count();
}


