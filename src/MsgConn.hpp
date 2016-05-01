#ifndef MSGCONN_HPP_INCLUDED
#define MSGCONN_HPP_INCLUDED

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/io_service.hpp>

#include <vector>
#include <array>
#include <string>
#include <tuple>
#include <map>

#include "Connection.hpp"
#include "MsgSvrClient.hpp"
#include "MessageDispatcher.h"


using namespace boost::asio;
using namespace std;


class MsgConn :public Connection
{
public:
    using err_code = boost::system::error_code;

public:
    MsgConn(io_service&);

    void initialization();

    virtual void on_connect();
    virtual void on_recv_msg(int, pb_message_ptr);
    virtual void on_disconnect();

public:
    void handle_dispatch_chat(pb_message_ptr);
    /// 转发频道消息
    void handle_dispatch_channel_chat(pb_message_ptr);
    void handle_user_login(pb_message_ptr);
    void handle_user_logout(pb_message_ptr);
    void handle_allocate_port(pb_message_ptr);

private:
    // 随机获得【a,b】之间的数
    int random(int a,int b);

    // 获得区间如"9000-9500"的最小和最大值
    tuple<int,int> get_min_max_port(string);


private:
    vector<MsgSvrClient> m_vecMsgSvrs;
    // map<端口、是否被使用>
    map<int, bool> m_PortUses;

    tuple<int,int> m_min_and_max;

private:
    MessageDispatcher m_dispatcher;

};
#endif // MSGCONN_HPP_INCLUDED
