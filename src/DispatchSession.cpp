#include "DispatchSession.hpp"
#include "M2RMsgTypeDefine.hpp"
#include "MsgStruct.hpp"

#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

/**********************************************
 *
 *
 *
 */



DispatchSession::DispatchSession(ip::tcp::socket socket_)
    :Handler(std::move(socket_)),
     m_min_and_max(std::make_tuple(0,0))
{
    initialization();

}




/**********************************************
 *
 *
 *
 */

void DispatchSession::initialization()
{
    std::string ports = "9500-9999";
    m_min_and_max = get_min_max_port(ports);

    int nMinPort = std::get<0>(m_min_and_max);
    int nMaxPort = std::get<1>(m_min_and_max);

    for (int port=nMinPort; port <= nMaxPort; port++)
    {
        m_PortUses.insert(std::make_pair(port, false));
    }
}


void DispatchSession::start()
{
    read_head();
}

void DispatchSession::process_msg(int type_,string buf_)
{
    std::cout << "begin process msg. " << std::endl;
    std::cout << "msg type: " << type_ << std::endl;
    switch (type_)
    {
    case (int)M2R::DISPATCH_CHAT:
        std::cout<< "chat!" << std::endl;
        handle_dispatch_chat(buf_);
        break;

    case (int)M2R::USER_LOGIN:
        std::cout<< "user login!" << std::endl;
        handle_user_login(buf_);
        break;

    case (int)M2R::USER_LOGOUT:
        std::cout<< "user logout!" << std::endl;
        handle_user_logout(buf_);
        break;

    case (int)M2R::ALLOCATE_PORT:
        std::cout<< "allocate port!" << std::endl;
        handle_allocate_port(buf_);
        break;
    }

}




/**********************************************
 *
 *  msg process function
 *
 */

void DispatchSession::handle_allocate_port(string buf_)
{
    std::ostringstream stream;
    stream <<&m_rBuf;



    int nAllocatePort = 0;
    int nMinPort = std::get<0>(m_min_and_max);
    int nMaxPort = std::get<1>(m_min_and_max);


    do
    {
        nAllocatePort = random(nMinPort, nMaxPort);
        cout << "m_PortUses["
             << nAllocatePort <<"] ="
             << m_PortUses[nAllocatePort] << endl;

    }
    while(m_PortUses[nAllocatePort]);

    m_PortUses[nAllocatePort] = true;


    std::cout << "allocate port: " << nAllocatePort << std::endl;

    Msg_allocate_port msg_result;
    msg_result.m_allocate_port = nAllocatePort;

    CMsg packet;
    packet.set_msg_type((int)M2R::ALLOCATE_PORT);
    packet.serialization_data_Asio(msg_result);

    send(packet);
}





void DispatchSession::handle_user_login(string buf_)
{

    Msg_Login ml;
    deserialization(ml, buf_);

    std::cout << "login id: " << ml.m_id << std::endl;


    auto it = std::find_if(m_vecMsgSvrs.begin(), m_vecMsgSvrs.end(),
            [this] (MsgSvrClient& m)
            {
                return (DispatchSession*)m.get_context() == this;
            });

    if (it == m_vecMsgSvrs.end())
    {
        std::cout << "error! this msgsvr is not connect!" << std::endl;
        return;
    }

    it->add_user(ml.m_id);
}

void DispatchSession::handle_user_logout(string buf_)
{
    Msg_Logout ml;
    deserialization(ml, buf_);

    auto it = std::find_if(m_vecMsgSvrs.begin(), m_vecMsgSvrs.end(),
        [this] (MsgSvrClient& m)
        {
            return (DispatchSession*)m.get_context() == this;
        });

    if (it == m_vecMsgSvrs.end())
    {
        std::cout << "del error,this isn't exist!" << std::endl;
        return;
    }

    it->del_user(ml.m_id);
}

void DispatchSession::handle_dispatch_chat(string buf_)
{
//
    Msg_chat recv_chat;
    deserialization(recv_chat, buf_);

    std::cout << "sendid: "     << recv_chat.m_send_id
              << "recvid: "     << recv_chat.m_recv_id
              << "content: "    << recv_chat.m_content << std::endl;

    auto it = find_if(m_vecMsgSvrs.begin(), m_vecMsgSvrs.end(),
        [=] (MsgSvrClient& m)
        {
            return m.is_in_svr(recv_chat.m_recv_id);
        });


    if (it == m_vecMsgSvrs.end())
    {
        std::cout << "userid: " << recv_chat.m_recv_id << "offline!" << std::endl;
        return;
    }

    CMsg packet;
    packet.set_msg_type((int)M2R::SEND_CHAT);
    packet.serialization_data_Asio(recv_chat);

    send(packet, it->get_socket());
}



/**********************************************
 *
 *
 *
 */


int DispatchSession::random(int min, int max)
{
    srand((int)time(NULL));
    return min + rand() % (max-min+1);
}

std::tuple<int,int> DispatchSession::get_min_max_port(std::string ports)
{
    int min=0,max=0;
    int index = ports.find('-');
    min = std::stoi(ports.substr(0, index));
    max = std::stoi(ports.substr(index+1, ports.size()));

    return std::make_tuple(min, max);
}

