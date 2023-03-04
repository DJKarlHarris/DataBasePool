#ifndef _MYSQL_CONN_H_
#define _MYSQL_CONN_H_ 

#include <mysql/mysql.h>
#include <string>
#include <chrono>

using std::string;

class MysqlConn {
public:
    //初始化数据库连接
    MysqlConn();

    //释放数据库连接
    ~MysqlConn();

    //连接数据库
    bool connect(string user, string passwd, string dbName, string ip, unsigned short port = 3306);

    //更新数据库
    bool update(string sql);

    //查询数据库
    bool query(string sql);

    //遍历结果集
    bool next();

    //得到结果集字段值
    string value(int index);

    //事务操作
    bool transaction();

    //提交事务
    bool commit();

    //事务回滚
    bool rollback();

    //刷新时间戳
    void flushTimePoint();

    //获取存活时间
    long long getAliveTime();

private:
    //释放result资源
    void freeResult();


private:
    MYSQL* m_conn = nullptr;
    MYSQL_RES* m_result = nullptr;
    MYSQL_ROW m_row = nullptr;
    std::chrono::steady_clock::time_point m_timePoint;//记录连接生成的时间戳
};

#endif