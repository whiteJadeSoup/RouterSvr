
#include "MsgConn_t.hpp"


MsgConn_t::MsgConn_t(void* context_, boost::asio::ip::tcp::socket& sock_)
    :m_context(context_), m_ConnSock (sock_)
{

}


void* MsgConn_t::get_context ()
{
    return m_context;
}


boost::asio::ip::tcp::socket& MsgConn_t::get_socket ()
{
    return m_ConnSock;
}
