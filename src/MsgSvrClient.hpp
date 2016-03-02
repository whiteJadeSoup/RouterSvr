#ifndef MSGSVRCLIENT_HPP_INCLUDED
#define MSGSVRCLIENT_HPP_INCLUDED

#include <boost/asio/ip/tcp.hpp>
#include <set>
#include <iostream>
#include "MsgConn_t.hpp"

class MsgSvrClient : public MsgConn_t
{
public:
    MsgSvrClient(void* context_, boost::asio::ip::tcp::socket&);

public:
    // 指定id的用户是否在该服务器上
    bool is_in_svr(int32_t);
    void add_user(int32_t);
    void del_user(int32_t);

private:
    std::set<int32_t> m_users;           //  该msgsvr中所有的用户ID
};


#endif // MSGSVRCLIENT_HPP_INCLUDED
