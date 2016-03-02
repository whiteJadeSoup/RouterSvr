#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/io_service.hpp>
#include <signal.h>

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

private:

    /// Wait for a request to stop the server.
    void await_stop();

    // 等待消息服务器连接
    void wait_accept ();
private:
    io_service m_io_service;
    ip::tcp::socket   m_sockMsg;
    ip::tcp::acceptor m_accMsg;

    signal_set m_signals;
};

extern Server* g;

#endif // SERVER_HPP
