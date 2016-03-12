#ifndef HANDLER_HPP_INCLUDED
#define HANDLER_HPP_INCLUDED

#include <array>
#include <string>
#include <iostream>
#include <boost/archive/text_iarchive.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/write.hpp>
#include <boost/asio/streambuf.hpp>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>

#include "CMsg.hpp"

using namespace std;
using namespace boost::asio;

class Handler: public enable_shared_from_this<Handler>
{


public:
    Handler (ip::tcp::socket);
    virtual ~Handler();

public:
    virtual void start() = 0;
    virtual void process_msg(int type_, string buf_) = 0;

public:
    ip::tcp::socket& socket();
    void read_head();
    void read_body(int len);

private:

    void encode(CMsg&);
    void decode();
    int32_t AsInt32 (const char* buf);


public:
    void send(CMsg&, ip::tcp::socket& sock_);
    void send(CMsg&);


protected:
    template <class T>
    void deserialization(T& t, string buf_)
    {
        istringstream is(buf_);
        boost::archive::text_iarchive ia(is);
        ia & t;
    }

    template <class T>
    void parse_pb_message(T& t, const string& buf_)
    {
        t.ParseFromString(buf_);
        cout << "buf size: " <<buf_.size() <<endl;
    }


protected:
    using err_code = boost::system::error_code;

    /*
     * 对clienthandler来说意味着一个客户端的socket的引用
     * 对routerhandler来说意味着连接routersvr的socket的引用
     */
    ip::tcp::socket m_sock;

    boost::asio::streambuf m_rBuf;
    boost::asio::streambuf m_wBuf;

    char head_info[4];
    string send_str;
};



#endif // HANDLER_HPP_INCLUDED
