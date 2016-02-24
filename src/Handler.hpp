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
    ip::tcp::socket& get_socket();
    void read_head_from_socket();
    /*
     * 读取指定大小长度的数据
     * @parm len  : 数据长度
     * @parm type : 消息类型
     */
    void read_body_from_socket(int len);

    void encode(CMsg&);
    void decode();

    void send_msg(CMsg&);
    void send_msg(ip::tcp::socket&, CMsg&);



    template <class T>
    void deserialization(T& t, boost::asio::streambuf& buf)
    {

        ostringstream os;
        os << &buf;

        string ser_data (os.str());
        istringstream is(ser_data);
        boost::archive::text_iarchive ia(is);
        ia & t;

        cout << "buf size: " <<buf.size() <<endl;
    }

    template <class T>
    void parse_pb_message(T& t, const string& buf_)
    {
        t.ParseFromString(buf_);
        cout << "buf size: " <<buf_.size() <<endl;
    }

protected:
    int32_t AsInt32 (const char* buf);

protected:
    shared_ptr<google::protobuf::Message> CreateMessage(const string&);


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
