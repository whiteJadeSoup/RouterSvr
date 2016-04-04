#ifndef DISPATCHSESSION_HPP_INCLUDED
#define DISPATCHSESSION_HPP_INCLUDED

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/streambuf.hpp>

#include <vector>
#include <array>
#include <string>
#include <tuple>
#include <map>

#include "Handler.hpp"
#include "MsgSvrClient.hpp"

using namespace boost::asio;
using namespace std;


class DispatchSession:public Handler
{
public:
    using err_code = boost::system::error_code;

public:
    DispatchSession(ip::tcp::socket);

    void initialization();
    virtual void start();
    virtual void process_msg(int, string);


public:
    void handle_dispatch_chat(string);
    void handle_user_login(string);
    void handle_user_logout(string);
    void handle_allocate_port(string);

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

};
#endif // DISPATCHSESSION_HPP_INCLUDED
