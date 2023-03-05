
#include <iostream>
#include <string>
#include "MysqlConn.h"

void query() {
    MysqlConn conn;
    conn.connect("root", "123456", "testDb", "172.23.132.132");


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
int main() {
    query();
    return 1;
}