

#include "Handler.hpp"
#include <boost/archive/text_iarchive.hpp>

/**********************************************
 *
 * Constructor and Deconstructor
 *
 */

Handler::Handler(boost::asio::ip::tcp::socket& sock_)
  :m_sock (sock_)
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

    std::cout << "start read head info!" << std::endl;
    auto self = shared_from_this();
    async_read(m_sock, boost::asio::buffer(head_info),
      [this, self] (const err_code& ec, size_t len)
      {
            if (!ec)
            {
                std::cout << "read len: " << len << std::endl;
                std::cout << head_info.data() << std::endl;
                int nLen = get_len();
                int nType = get_type();


                std::cout << "data len: "<< nLen << "msg type: "<< nType << std::endl;
                read_body_from_socket(nLen, nType);
            }
            else
            {
                std::cout << "read head error: " << ec.message() << std::endl;
            }
      });
}

void Handler::read_body_from_socket(int len_, int type_)
{
    std::cout << "start read data!" << std::endl;
    auto self = shared_from_this();
    async_read(m_sock, m_rBuf, transfer_exactly(len_),
        [self, this, type_] (const err_code& ec, size_t len)
        {
            if(!ec)
            {
                std::cout << "readed data size: " << len <<std::endl;
                process_msg(type_);
            }
            else
            {
                std::cout <<"read body error: " << ec.message() << std::endl;
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
    std::string len;
    copy(head_info.begin(), head_info.begin()+4, back_inserter(len));
    return std::stoi(len);
}

int Handler::get_type()
{
    std::string type;
    copy(head_info.begin()+4, head_info.end(), back_inserter(type));
    return std::stoi(type);
}


void Handler::encode_msg(CMsg& msg)
{
        std::cout << "start encode msg" << std::endl;
        send_str.clear();

        char len[5];
        sprintf(len, "%4d", msg.get_data_len());

        char type[5];
        sprintf(type, "%4d", msg.get_msg_type());

        // data
        std::string data = msg.get_send_data();

        send_str.append(len);
        send_str.append(type);
        send_str.append(data);

        std::cout << "headinfo: "      << send_str.substr(0,8) << std::endl;
        std::cout << "head size: "      << data.size()<< std::endl;
        std::cout << "send str size: "      << send_str.size()      << std::endl;
}

void Handler::send_msg(ip::tcp::socket& sock_, CMsg& msg)
{
    encode_msg(msg);

    std::cout << "start send msg." << std::endl;
    auto self = shared_from_this();
    async_write(sock_, boost::asio::buffer(send_str),
        [this, self] (const err_code& ec, size_t len)
        {
            if (!ec)
            {
                std::cout << "send data to client len: " << len << std::endl;
            }
            else
            {
                std::cout << "send msg error: " <<ec.message() << std::endl;
            }

        });

}

void Handler::send_msg(CMsg& msg)
{
    encode_msg(msg);

    std::cout << "start send msg." << std::endl;
    auto self = shared_from_this();
    async_write(m_sock, boost::asio::buffer(send_str),
        [this, self] (const err_code& ec, size_t len)
        {
            if (!ec)
            {
                std::cout << "send data to client len: " << len << std::endl;
            }
            else
            {
                std::cout << "send msg error: " <<ec.message() << std::endl;
            }

        });
}


