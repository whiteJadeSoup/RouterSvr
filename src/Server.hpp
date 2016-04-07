#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <memory>


#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/io_service.hpp>
#include <signal.h>



class Connection;


using namespace std;
using namespace boost::asio;

using err_code = boost::system::error_code;

class Server
{
public:
    Server() = delete;
    Server(unsigned short port);

    virtual ~Server();

public:
    void initialization();
    void run();
    // 分配连接id
    int allocate_conn_id();



private:
    /// Wait for a request to stop the server.
    void await_stop();
    // 等待消息服务器连接
    void wait_accept ();
    // 主动连接dbsvr
    void connect_db();

private:
    io_service m_io_service;

    ip::tcp::acceptor m_accMsg;

    signal_set m_signals;
    //  connection for msgsvr
    shared_ptr<Connection> m_msg_conn;
    //  connection for dbsvr
    shared_ptr<Connection> m_db_conn;

private:
    static int g_count;
};

extern Server* g;

#endif // SERVER_HPP
