#ifndef CONNECTION_HPP_INCLUDED
#define CONNECTION_HPP_INCLUDED

#include <string>
#include <iostream>
#include <functional>
#include <iomanip>
#include <ctime>
#include <sstream>



#include <boost/noncopyable.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/placeholders.hpp>

#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>

#include "CMsg.hpp"


using namespace std;
using namespace boost::asio;



typedef boost::system::error_code   err_code;
typedef shared_ptr<google::protobuf::Message> pb_message_ptr;



#define TRY                                 \
    try                                     \
    {                                       \
        GOOGLE_PROTOBUF_VERIFY_VERSION;     \
        using namespace google::protobuf;   \

#define CATCH                               \
    }                                       \
    catch (exception& e)                    \
    {                                                                   \
        cout << "# ERR: exception in " << __FILE__;                     \
        cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;\
        cout << "# ERR: " << e.what() << endl;                          \
    }                                                                   \



class Connection
    :public enable_shared_from_this<Connection>,
     private boost::noncopyable
{
public:
    Connection (io_service& );
    virtual ~Connection();

    // 连接建立 子类实现具体逻辑
    virtual void on_connect() = 0;
    // 消息分发 子类实现具体逻辑
    virtual void on_recv_msg(int, pb_message_ptr) = 0;
    // 连接断开 子类实现具体逻辑
    virtual void on_disconnect() = 0;


    // 连接断开
    void disconnect();
    // 连接建立
    void connect(int conn_id_);
    // 收到消息
    void recv_msg();

public:
    ip::tcp::socket& socket();
    inline int get_conn_id () { return m_conn_id; }


    void send_and_shutdown(CMsg&, ip::tcp::socket&);
    // 发送数据包到指定 socket
    void send(CMsg&, ip::tcp::socket& sock_);
    void send(CMsg&);

protected:
    // 读取头部信息
    void read_head();
    // 获得当前时间
    string get_cur_time();


private:
    void handle_read_head(const err_code&, std::size_t);
    void handle_read_body(const err_code&, std::size_t);
    void handle_write(const err_code&, std::size_t);
    void handle_write_done_shutdown(const err_code&, std::size_t, ip::tcp::socket&);



private:
    // 读取指定长度数据
    void read_body(int len);
    // 解包
    tuple<int, pb_message_ptr> decode();
    // 将字符串解析为int32
    int32_t AsInt32 (const char* buf);

    // 根据类名构建protobuf消息体
    // 返回nullptr如果没有该类名信息
    shared_ptr<google::protobuf::Message> CreateMessage(const string&);

private:
    // 连接
    ip::tcp::socket m_sock;
    // 确保异步handler调用不会并发执行
    io_service::strand m_strand;
    // 保存从socket读取的数据
    boost::asio::streambuf m_rBuf;
    // 头部信息
    char head_info[sizeof(int32_t)];
    // 连接标志
    int m_conn_id;
};



typedef shared_ptr<Connection> connection_ptr;

#endif // Connection_HPP_INCLUDED
