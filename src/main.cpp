#include <iostream>
#include <vector>

#include <boost/asio/io_service.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/basic_socket_acceptor.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/asio/ip/address.hpp>
#include <boost/asio/ip/basic_endpoint.hpp>

#include <signal.h>

#include "DispatchSession.hpp"

using namespace std;
using namespace boost::asio;


class RouterSvr
{

public:
    using err_code = boost::system::error_code;

public:
    RouterSvr () = delete;

    RouterSvr(io_service& io, unsigned short port)
        :m_MsgSvrSock(io),
         m_MsgSvrAcceptor(io),
         m_signals (io)

    {
        m_signals.add(SIGINT);
        m_signals.add(SIGTERM);
#if defined(SIGQUIT)
        m_signals.add(SIGQUIT);
#endif // defined(SIGQUIT)

        // Open the acceptor with the option to reuse the address (i.e. SO_REUSEADDR).

        ip::tcp::endpoint endpoint = ip::tcp::endpoint(ip::tcp::v4(), port);
        m_MsgSvrAcceptor.open(endpoint.protocol());
        m_MsgSvrAcceptor.set_option(ip::tcp::acceptor::reuse_address(true));
        m_MsgSvrAcceptor.bind(endpoint);
        m_MsgSvrAcceptor.listen();


        await_stop();
        wait_accept();
    }

private:


    /// Wait for a request to stop the server.
    void await_stop()
    {

        m_signals.async_wait(
            [this](boost::system::error_code /*ec*/, int /*signo*/)
        {
            // The server is stopped by cancelling all outstanding asynchronous
            // operations. Once all operations have finished the io_service::run()
            // call will exit.
            m_MsgSvrAcceptor.close();
        });
    }

    void wait_accept ()
    {
        m_MsgSvrAcceptor.async_accept(m_MsgSvrSock,
                                      [this] (const err_code& ec)
        {
            if (!ec)
            {
                cout << "address: " << m_MsgSvrSock.remote_endpoint().address().to_string()
                     << "port: " << m_MsgSvrSock.remote_endpoint().port() << endl;


                std::make_shared<DispatchSession>(std::move(m_MsgSvrSock), m_vecMsgSvrs)->start();
            }
            wait_accept();
        });
    }
private:
    ip::tcp::socket   m_MsgSvrSock;
    ip::tcp::acceptor m_MsgSvrAcceptor;

    signal_set m_signals;
private:
    vector<MsgSvrClient> m_vecMsgSvrs;
};



int main()
{
    io_service io;
    RouterSvr s (io, 10000);
    io.run();
    return 0;
}
