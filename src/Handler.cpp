
#include "Handler.hpp"


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
                    try
                    {
                        int nLen = get_len();
                        int nType = get_type();

                        cout << "data len: "<< nLen << "msg type: "<< nType << endl;
                        read_body_from_socket(nLen, nType);
                    }
                    catch (exception& e)
                    {
                        cout << "# ERR: exception in " << __FILE__;
                        cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
                        cout << "# ERR: " << e.what() << endl;

                        read_head_from_socket();
                    }
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

void Handler::read_body_from_socket(int len_, int type_)
{
    auto self = shared_from_this();
    async_read(m_sock, m_rBuf, transfer_exactly(len_),
            [self, this, type_] (const err_code& ec, size_t len)
            {

                if(!ec)
                {
                    cout << "readed data size: " << len <<endl;
                    process_msg(type_);
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

int Handler::get_len()
{
    string len;
    copy(head_info.begin(), head_info.begin()+4, back_inserter(len));
    return std::stoi(len);
}

int Handler::get_type()
{
    string type;
    copy(head_info.begin()+4, head_info.end(), back_inserter(type));
    return std::stoi(type);
}


void Handler::encode_msg(CMsg& msg)
{
    cout << "start encode msg" << endl;
    send_str.clear();

    char len[5];
    sprintf(len, "%4d", msg.send_data_len());

    char type[5];
    sprintf(type, "%4d", msg.get_msg_type());

    // data
    string data = msg.get_send_data();

    send_str.append(len);
    send_str.append(type);
    send_str.append(data);

    cout << "headinfo: "      << send_str.substr(0,8) << endl;
    cout << "data size: "      << data.size()          << endl;
    cout << "send str size: "      << send_str.size()      << endl;
}

void Handler::send_msg(ip::tcp::socket& sock_, CMsg& msg)
{
    encode_msg(msg);

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
    encode_msg(msg);

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



