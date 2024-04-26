#ifndef DB_H
#define DB_H

#include <mysql/mysql.h>
#include <string>
using namespace std;

// 数据库操作类
class MySQL
{
public:
    // 初始化数据库连接
    MySQL();
    // 释放数据库连接资源这里用UserModel示例，通过UserModel如何对业务层封装底层数据库的操作。代码示例如下：
    ~MySQL();
    // 连接数据库
    bool connect();
    // 更新操作
    bool update(string sql);

    // 查询操作
    MYSQL_RES *query(string sql);

    //获取连接
    MYSQL* getConnection();

private:
    MYSQL *_conn;
};

#endif