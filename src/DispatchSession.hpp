#ifndef DISPATCHSESSION_HPP_INCLUDED
#define DISPATCHSESSION_HPP_INCLUDED

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/streambuf.hpp>

#include <vector>
#include <array>
#include <tuple>
#include <map>

#include "Handler.hpp"
#include "MsgSvrClient.hpp"

using namespace boost::asio;

class DispatchSession:public Handler
{
public:
    using err_code = boost::system::error_code;

public:
    DispatchSession(ip::tcp::socket, std::vector<MsgSvrClient>& );

    void initialization();
    virtual void start();
    virtual void process_msg(int type_);


public:
    void M2RMsg_DispatchChat();
    void M2RMsg_UserLogin();
    void M2RMsg_UserLogout();
    void M2RMsg_AllocatePort();

private:

    // 随机获得【a,b】之间的数
    int random(int a,int b);

    // 获得区间如"9000-9500"的最小和最大值
    std::tuple<int,int> get_min_max_port(std::string);


private:
    ip::tcp::socket m_MsgSvrSock;

private:
    std::vector<MsgSvrClient>& m_vecMsgSvrs;
    // map<端口、是否被使用>
    std::map<int, bool> m_PortUses;

    std::tuple<int,int> m_min_and_max;

};
#endif // DISPATCHSESSION_HPP_INCLUDED
