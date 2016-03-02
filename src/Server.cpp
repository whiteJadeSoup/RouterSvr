#include "Server.hpp"
#include "DispatchSession.hpp"
#include <boost/asio/buffer.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/read.hpp>


Server *g = nullptr;

/**********************************************
 *
 *  构造函数
 */

Server::Server(unsigned short port_)
  :m_io_service(),
   m_sockMsg(m_io_service),
   m_accMsg(m_io_service),
   m_signals(m_io_service)
{
    m_signals.add(SIGINT);
    m_signals.add(SIGTERM);
#if defined(SIGQUIT)
    m_signals.add(SIGQUIT);
#endif // defined(SIGQUIT)

    // Open the acceptor with the option to reuse the address (i.e. SO_REUSEADDR).
    ip::tcp::endpoint endpoint = ip::tcp::endpoint(ip::tcp::v4(), port_);
    m_accMsg.open(endpoint.protocol());
    m_accMsg.set_option(ip::tcp::acceptor::reuse_address(true));
    m_accMsg.bind(endpoint);
    m_accMsg.listen();

    await_stop();
}

Server::~Server()
{
    //dtor
}




/**********************************************
 *
 *
 */


void Server::initialization()
{

}


void Server::run()
{
    wait_accept();
    m_io_service.run();
}




/**********************************************
 *
 *
 */

void Server::await_stop()
{

    m_signals.async_wait([this](boost::system::error_code /*ec*/, int /*signo*/)
        {
        // The server is stopped by cancelling all outstanding asynchronous
        // operations. Once all operations have finished the io_service::run()
        // call will exit.
            m_accMsg.close();
        });
}

void Server::wait_accept ()
{
    m_accMsg.async_accept(m_sockMsg, [this] (const err_code& ec)
        {
            if (!ec)
            {
                cout << "address: " << m_sockMsg.remote_endpoint().address().to_string()
                    << "port: " << m_sockMsg.remote_endpoint().port() << endl;


                std::make_shared<DispatchSession>(std::move(m_sockMsg))->start();
            }
            wait_accept();
        });
}


