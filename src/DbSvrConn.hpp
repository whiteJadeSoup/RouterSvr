#ifndef DbSvrConnection_HPP_INCLUDED
#define DbSvrConnection_HPP_INCLUDED

#include "Connection.hpp"

#include <boost/asio/io_service.hpp>



class DBSvrConn: public Connection
{
public:
    DBSvrConn(io_service&);

public:
    virtual void on_connect() override;
    virtual void on_recv_msg(int, pb_message_ptr) override;
    virtual void on_disconnect() override;

};


void send_to_db (CMsg&);

#endif // DbSvrConnection_HPP_INCLUDED
