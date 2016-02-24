
#include "Handler.hpp"
#include <arpa/inet.h>


/**********************************************
 *
 * Constructor and Deconstructor
 *
 */

Handler::Handler(ip::tcp::socket sock_)
    :m_sock (std::move(sock_))
{

}


Handler::~Handler()
{

}


/**********************************************
 *
 *
 *
 */


void Handler::read_head_from_socket()
{

    cout << "start read head info!" << endl;
    auto self = shared_from_this();
    async_read(m_sock, boost::asio::buffer(head_info),
            [this, self] (const err_code& ec, size_t len)
            {

                if (!ec)
                {

                    int32_t data_len = AsInt32(head_info);
                    cout << data_len <<endl;
                    // 开始读数据体
                    read_body_from_socket(data_len);

                }

                else
                {
                    cout << "# ERR: exception in " << __FILE__;
                    cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
                    cout << "# ERR: " << ec.message() << endl;

                    m_sock.close();
                }
            });
}

void Handler::read_body_from_socket(int len_)
{
    auto self = shared_from_this();
    async_read(m_sock, m_rBuf, transfer_exactly(len_),
            [self, this] (const err_code& ec, size_t len)
            {

                if(!ec)
                {
                    cout << "readed data size: " << len <<endl;
                    decode();
                }

                else
                {
                    cout << "# ERR: exception in " << __FILE__;
                    cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
                    cout << "# ERR: " << ec.message() << endl;

                    m_sock.close();
                }

                read_head_from_socket();
            });
}


ip::tcp::socket& Handler::get_socket()
{
    return m_sock;
}

/**********************************************
 *
 * tool function
 *
 */

int32_t Handler::AsInt32 (const char* buf)
{
    int32_t buf_len = 0;
    std::copy(buf, buf + sizeof(int32_t), reinterpret_cast<char*>(&buf_len));
    return ::ntohl(buf_len);
}



shared_ptr<google::protobuf::Message> Handler::CreateMessage(const string& type_name)
{
    using namespace google::protobuf;

    const Descriptor* dr = DescriptorPool::generated_pool()->FindMessageTypeByName(type_name);
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


void Handler::encode(CMsg& msg)
{
    cout << "start encode msg" << endl;

    send_str.clear();

    // 为头部预留空间
    send_str.resize(sizeof(int32_t));

    // 消息类型
    int32_t type = msg.get_msg_type();
    int32_t be_type = ::htonl(type);
    send_str.append(reinterpret_cast<char*>(&be_type), sizeof(be_type));

    // 序列化数据
    send_str.append(msg.get_send_data().c_str(), msg.send_data_len() + 1);


    // 数据总长度
    int32_t len = sizeof(int32_t) + msg.send_data_len() + 1;
    int32_t be_len = ::htonl(len);
    std::copy(reinterpret_cast<char*>(&be_len),
              reinterpret_cast<char*>(&be_len) + sizeof(be_len),
              send_str.begin());


    cout << "send data len: " << msg.send_data_len() + 1 << endl;
    cout << "send str size: " << send_str.size() << endl;
}


void Handler::decode()
{
    ostringstream os;
    os << &m_rBuf;


    // 收到的数据
    string trans_data = os.str();

    // 消息类型
    int32_t type = AsInt32(trans_data.c_str());
    cout << "type: " << type << endl;


    // 序列化数据
    std::string buf = trans_data.substr(sizeof(int32_t));
    process_msg(type, buf);
}



void Handler::send_msg(ip::tcp::socket& sock_, CMsg& msg)
{
    encode(msg);

    cout << "start send msg." << endl;
    auto self = shared_from_this();
    async_write(sock_, boost::asio::buffer(send_str),
            [this, self, &sock_] (const err_code& ec, size_t len)
            {
                if (!ec)
                {
                    cout << "send data to client len: " << len << endl;
                }
                else
                {
                    cout << "# ERR: exception in " << __FILE__;
                    cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
                    cout << "# ERR: " << ec.message() << endl;
                    sock_.close();
                }
            });

}

void Handler::send_msg(CMsg& msg)
{
    encode(msg);

    cout << "start send msg." << endl;
    auto self = shared_from_this();
    async_write(m_sock, boost::asio::buffer(send_str),
                [this, self] (const err_code& ec, size_t len)
                {
                    if (!ec)
                    {
                        cout << "send data to client len: " << len << endl;
                    }
                    else
                    {
                        cout << "# ERR: exception in " << __FILE__;
                        cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
                        cout << "# ERR: " << ec.message() << endl;

                        m_sock.close();
                    }

                });
}



