#include "MsgSvrClient.hpp"



MsgSvrClient::MsgSvrClient(void* context_,boost::asio::ip::tcp::socket& socket_)
    : MsgConn_t(context_, socket_)
{

}


bool MsgSvrClient::is_in_svr(int32_t id_)
{
    auto it = find_if(m_users.begin(), m_users.end(),
                      [=] (int32_t id)
    {
        return id == id_;
    });
    return it == m_users.end() ? false : true;
}


void MsgSvrClient::add_user(int32_t id_)
{
    std::cout << "add user id: " << id_ << std::endl;
    m_users.insert(id_);
}

void MsgSvrClient::del_user(int32_t id_)
{
    std::cout << "del user id: " << id_ << std::endl;
    m_users.erase(id_);
}
