#ifndef MSGCONN_T_HPP_INCLUDED
#define MSGCONN_T_HPP_INCLUDED
#include <boost/asio/ip/tcp.hpp>


class MsgConn_t
{
public:
    MsgConn_t () = delete;

    MsgConn_t (void*, boost::asio::ip::tcp::socket&);


    boost::asio::ip::tcp::socket& get_socket();
    void* get_context ();


private:
    void* m_context;                                //
    boost::asio::ip::tcp::socket& m_ConnSock;       //
};

#endif // MSGCONN_T_HPP_INCLUDED
