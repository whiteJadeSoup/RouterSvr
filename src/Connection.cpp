
#include "Connection.hpp"
#include <arpa/inet.h>


#include <boost/bind.hpp>


/**********************************************
 *
 * Constructor and Deconstructor
 *
 */

Connection::Connection(io_service& io_service_)
  :m_sock(io_service_), m_strand(io_service_)
{
}


Connection::~Connection()
{

}


/**********************************************
 *
 *
 *
 */

void Connection::read_head()
{

    cout << "start read head info!" << endl;
    auto self = shared_from_this();


    async_read(m_sock, boost::asio::buffer(head_info),
        m_strand.wrap(
            boost::bind(&Connection::handle_read_head, shared_from_this(),
                      boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred)
        ));
}

void Connection::read_body(int len_)
{
    auto self = shared_from_this();

    async_read(m_sock, m_rBuf, transfer_exactly(len_),
        m_strand.wrap(
            boost::bind(&Connection::handle_read_body, shared_from_this(),
                      boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred)
        ));
}



void Connection::send(CMsg& msg, ip::tcp::socket& sock_)
{
    cout << "start send msg." << endl;
    auto self = shared_from_this();

    string data = msg.get_send_data();
    async_write(sock_, boost::asio::buffer(data),
        m_strand.wrap(
            boost::bind(&Connection::handle_write, shared_from_this(),
                      boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred)
        ));
}


void Connection::send(CMsg& msg)
{


    cout << "start send msg." << endl;
    auto self = shared_from_this();

    string data = msg.get_send_data();
    async_write(m_sock, boost::asio::buffer(data),
        m_strand.wrap(
            boost::bind(&Connection::handle_write, shared_from_this(),
                      boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred)
        ));
}


void Connection::send_and_shutdown(CMsg& pkt, ip::tcp::socket& sock_)
{

    auto self = shared_from_this();
    async_write(sock_, boost::asio::buffer(pkt.get_send_data()),
        m_strand.wrap(
            boost::bind(&Connection::handle_write_done_shutdown, shared_from_this(),
                      boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred,
                      ref(sock_))
        ));
}


ip::tcp::socket& Connection::socket()
{
    return m_sock;
}



void Connection::disconnect()
{

    if (m_sock.is_open())
    {
        m_sock.close();
    }

    on_disconnect();
}




void Connection::connect(int conn_id_)
{
    // 保存连接标志
    m_conn_id = conn_id_;

    //
    on_connect();
}


void Connection::recv_msg()
{
    int type = 0;
    pb_message_ptr p_message;

    tie(type, p_message) = decode();


    if (p_message != nullptr)
    {
        on_recv_msg(type, p_message);
    }
    else
    {
        cout << "decode error! message = nullptr! type: " << type << endl;
    }

}



/**********************************************
 *
 * tool function
 *
 */

int32_t Connection::AsInt32 (const char* buf)
{
    int32_t buf_len = 0;
    std::copy(buf, buf + sizeof(int32_t), reinterpret_cast<char*>(&buf_len));
    return ::ntohl(buf_len);
}


pb_message_ptr Connection::CreateMessage(const string& type_name)
{
    using namespace google::protobuf;

    try
    {
        const DescriptorPool* pDescPool = DescriptorPool::generated_pool();

        if(pDescPool == nullptr)
        {
            return nullptr;
        }

        const Descriptor* dr = pDescPool->FindMessageTypeByName(type_name);
        if (dr == nullptr)
        {
            return nullptr;
        }

        const Message* proto = MessageFactory::generated_factory()->GetPrototype(dr);

        if (proto == nullptr)
        {
            return nullptr;
        }

        shared_ptr<Message> p_message(proto->New());
        return p_message;

    }
    catch (exception ec)
    {
        cout << "# ERR: exception in " << __FILE__;
        cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
        cout << "# ERR: " << ec.what() << endl;
        return nullptr;
    }
}


tuple<int, pb_message_ptr> Connection::decode()
{

    GOOGLE_PROTOBUF_VERIFY_VERSION;
    using namespace google::protobuf;

    ostringstream os;
    os << &m_rBuf;

    // 收到的数据
    string trans_data = os.str();

    // 消息类型
    int32_t type = AsInt32(trans_data.c_str());
    trans_data = trans_data.substr(sizeof(int32_t));

    // 类名长度
    int32_t name_len = AsInt32(trans_data.c_str());
    cout << "Decode Type Name Len: " << name_len << endl;
    trans_data = trans_data.substr(sizeof(int32_t));

    // 类名
    const char* chr_name = trans_data.c_str();
    string type_name = string(chr_name, name_len-1);
    cout << "Decode Type Name: " << type_name << endl;


    shared_ptr<google::protobuf::Message> p_ms = CreateMessage(type_name);

    if (p_ms == nullptr)
    {
        return make_tuple(type, nullptr);
    }

    else
    {
        // 反序列化
        trans_data = trans_data.substr(name_len);

        const char* buf_ = trans_data.c_str();
        int size = trans_data.size();
        cout << "Decode Data Len: " << size << endl;

        p_ms->ParseFromArray(buf_, size);
        return make_tuple(type, p_ms);
    }

}



string Connection::get_cur_time()
{
    time_t t = time(nullptr);
    ostringstream os;

    char tmp[128];
    strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S", localtime(&t));
    return string(tmp);
}




/**********************************************
 *
 *  callback
 */

void Connection::handle_read_head(const err_code& ec, std::size_t byte_trans)
{
    if (!ec)
    {

        int32_t data_len = AsInt32(head_info);
        cout << "Total data len: " << data_len <<endl;

        // 开始读数据体
        read_body(data_len);
    }
    else
    {
        cout << "# ERR: exception in " << __FILE__;
        cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
        cout << "# ERR: " << ec.message() << endl;

        // 关闭连接
        disconnect();
    }
}




void Connection::handle_read_body(const err_code& ec, std::size_t byte_trans)
{

    if (!ec)
    {
        cout << "Readed data size: " << byte_trans <<endl;

        recv_msg();

        // 继续读取数据
        read_head();
    }

    else
    {
        cout << "# ERR: exception in " << __FILE__;
        cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
        cout << "# ERR: " << ec.message() << endl;

        disconnect();

    }
}



void Connection::handle_write(const err_code& ec, std::size_t byte_trans)
{
    if (!ec)
    {
        cout << "Send Packet Len: " << byte_trans << endl;
    }

    else
    {
        cout << "# ERR: exception in " << __FILE__;
        cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
        cout << "# ERR: " << ec.message() << endl;

        disconnect();
    }
}


void Connection::handle_write_done_shutdown(const err_code& ec, std::size_t byte_trans,
    ip::tcp::socket& sock_)
{

    if (!ec)
    {
        cout << "Send Packet Len: " << byte_trans << endl;

        // 发送验证结果后断开连接
        if (sock_.is_open())
        {
            err_code ignore_ec;
            sock_.shutdown(ip::tcp::socket::shutdown_both, ignore_ec);
        }
    }

    else
    {
        cout << "# ERR: exception in " << __FILE__;
        cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
        cout << "# ERR: " << ec.message() << endl;
    }
}

