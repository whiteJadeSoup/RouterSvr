


#include "DbSvrConn.hpp"


DBSvrConn* g_db_handler = nullptr;
enum class VALIDATE_STATUS
{
    SUCCESS = 0,
    FAIL    = 1,
    NO_SVR  = 2,
};



/***********************************
 *
 *
 */

DBSvrConn::DBSvrConn(io_service& io_)
  :Connection(io_)
{

}



void DBSvrConn::on_connect()
{
    g_db_handler = this;
}


void DBSvrConn::on_recv_msg(int type_, pb_message_ptr p_msg_)
{
}



void DBSvrConn::on_disconnect()
{

}




void send_to_db (CMsg& packet_)
{
    if (g_db_handler)
        g_db_handler->send(packet_);
}




