#include "chatserver.hpp"
#include "chatservice.hpp"
#include "json.hpp"

#include <functional> //绑定器
#include <string>
using namespace std;
using namespace placeholders;
using json=nlohmann::json;

// 初始化聊天服务器对象
ChatServer::ChatServer(EventLoop *loop,               // 事件循环
                       const InetAddress &listenAddr, // IP+Port
                       const string &nameArg)         // 服务器的名字
    : _server(loop, listenAddr, nameArg), _loop(loop)
{
    // 注册链接回调
    _server.setConnectionCallback(std::bind(&ChatServer::onConnection, this, _1)); //_1参数占位符，命名空间placeholders,onConnection只有一个参数

    // 注册消息回调
    _server.setMessageCallback(std::bind(&ChatServer::onMessage, this, _1, _2, _3)); //_1,_2,_3，onMessage有3个参数

    // 设置线程数量
    _server.setThreadNum(4);
}

// 启动服务
void ChatServer::start()
{
    _server.start();
}

// 上报链接相关信息的回调函数
void ChatServer::onConnection(const TcpConnectionPtr &conn)
{
    //客户端断开连接
    if(!conn->connected())
    {
        ChatService::instance()->clientCloseException(conn);
        conn->shutdown();
    }
}
// 上报读写事件信息相关的回调函数
void ChatServer::onMessage(const TcpConnectionPtr &conn, // 连接
                           Buffer *buffer,               // 缓冲区
                           Timestamp time)               // 接收到数据的时间信息
{
    string buf=buffer->retrieveAllAsString();
    //数据的反序列化
    json js=json::parse(buf);

    //达到的目的：完全解耦网络模块的代码和业务模块的代码
    //通过js["msgid"]获取=》业务handler(处理器)=》 conn  js  time
    //获取服务的唯一实例，获取消息id所对应的消息处理器
    auto msgHandler = ChatService::instance()->getHandler(js["msgid"].get<int>());//js["msgid"].get<int>(),将js["msgid"]键值强转成int类型
    //回调消息绑定好的事件处理器，来执行相应的业务处理
    msgHandler(conn,js,time);
}
