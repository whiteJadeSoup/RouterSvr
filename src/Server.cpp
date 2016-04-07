#include "Server.hpp"
#include "MsgConn.hpp"
#include "DbSvrConn.hpp"


#include <boost/asio/buffer.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/connect.hpp>



Server *g = nullptr;
int Server::g_count = 1;

/**********************************************
 *
 *  构造函数
 */

Server::Server(unsigned short port_)
  :m_io_service(),
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
    wait_accept();
    connect_db();
}


void Server::run()
{
    m_io_service.run();
}


int Server::allocate_conn_id()
{
    return (g_count++) % 65535;
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

    m_msg_conn.reset(new MsgConn(m_io_service));
    m_accMsg.async_accept(m_msg_conn->socket(), [this] (const err_code& ec)
        {
            if (!ec)
            {
                cout << "MsgSvr address: " << m_msg_conn->socket().remote_endpoint().address().to_string()
                     << "MsgSVr port: " << m_msg_conn->socket().remote_endpoint().port() << endl;


                int id = allocate_conn_id();
                m_msg_conn->connect(id);
            }
            wait_accept();
        });
}


void Server::connect_db()
{
    string address = "127.0.0.1";
    string port  = "13000";

    ip::tcp::resolver resolver(m_io_service);
    ip::tcp::resolver::iterator it = resolver.resolve({address, port});


    m_db_conn.reset(new DBSvrConn(m_io_service));

    async_connect(m_db_conn->socket(), it,
        [this] (const err_code& ec, ip::tcp::resolver::iterator it )
        {
            if (!ec)
            {
                cout << "connect db!" << endl;
                int conn_id = allocate_conn_id();
                m_db_conn->connect(conn_id);

            }
            else
            {
                cout << "error. try connect db..." << endl;
                boost::asio::deadline_timer t(m_io_service, boost::posix_time::seconds(5));
                t.wait();
                connect_db();
            }
        });

}


