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



DispatchSession::DispatchSession(ip::tcp::socket socket_,
                                 std::vector<MsgSvrClient>& svrs_)
    :Handler(std::move(socket_)),
     m_vecMsgSvrs(svrs_),
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

    std::cout<< "add msgsvr." <<std::endl;
    //m_vecMsgSvrs.emplace_back(this, m_MsgSvrSock);

    read_head_from_socket();
}

void DispatchSession::process_msg(int type_,string buf_)
{
    std::cout << "begin process msg. " << std::endl;
    std::cout << "msg type: " << type_ << std::endl;
    switch (type_)
    {
    case (int)TypeDefine::M2R_DispatchChat:
        std::cout<< "chat!" << std::endl;
        M2RMsg_DispatchChat();
        break;

    case (int)TypeDefine::M2R_UserLogin:
        std::cout<< "user login!" << std::endl;
        M2RMsg_UserLogin();
        break;

    case (int)TypeDefine::M2R_UserLogout:
        std::cout<< "user logout!" << std::endl;
        M2RMsg_UserLogout();
        break;

    case (int)TypeDefine::M2R_AllocatePort:
        std::cout<< "allocate port!" << std::endl;
        M2RMsg_AllocatePort();
        break;
    }

}




/**********************************************
 *
 *  msg process function
 *
 */

void DispatchSession::M2RMsg_AllocatePort()
{
    std::ostringstream stream;
    stream <<&m_rBuf;



    int nAllocatePort = 0;
    int nMinPort = std::get<0>(m_min_and_max);
    int nMaxPort = std::get<1>(m_min_and_max);


    do
    {
        nAllocatePort = random(nMinPort, nMaxPort);
        std::cout << "m_PortUses["<< nAllocatePort <<"] ="  << m_PortUses[nAllocatePort] << std::endl;

    }
    while(m_PortUses[nAllocatePort]);

    m_PortUses[nAllocatePort] = true;


    std::cout << "allocate port: " << nAllocatePort << std::endl;

    Msg_allocate_port msg_result;
    msg_result.m_allocate_port = nAllocatePort;

    CMsg msg;
    msg.set_msg_type((int)TypeDefine::M2R_AllocatePort);
    msg.set_send_data(msg_result);

    send_msg(msg);

}





void DispatchSession::M2RMsg_UserLogin()
{

    Msg_Login ml;
    deserialization(ml, m_rBuf);


    std::cout << "login id: " << ml.m_id << std::endl;


    auto it = std::find_if(m_vecMsgSvrs.begin(), m_vecMsgSvrs.end(),
            [this] (MsgSvrClient& m)
            {
                return (DispatchSession*)m.get_context() == this;
            });

    if (it == m_vecMsgSvrs.end())
    {
        std::cout << "add error,this isn't exist!" << std::endl;
        return;
    }

    it->add_user(ml.m_id);
}

void DispatchSession::M2RMsg_UserLogout()
{
    int nUserId = 0;
    boost::archive::binary_iarchive ia(m_rBuf);
    ia & nUserId;

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

    it->del_user(nUserId);

}

void DispatchSession::M2RMsg_DispatchChat()
{

    Msg_chat recv_chat;
    deserialization(recv_chat, m_rBuf);

    std::cout << "sendid: "     << recv_chat.m_send_id
              << "recvid: "     << recv_chat.m_recv_id
              << "content: "    << recv_chat.m_content << std::endl;

    auto it = find_if(m_vecMsgSvrs.begin(), m_vecMsgSvrs.end(),
                      [=] (MsgSvrClient& m)
    {
        return m.is_InSvr_ById(recv_chat.m_recv_id);
    });


    if (it == m_vecMsgSvrs.end())
    {
        std::cout << "offline!" << std::endl;
        return;
    }



    CMsg msg;
    msg.set_msg_type((int)TypeDefine::M2R_DispatchChat);
    msg.set_send_data(recv_chat);

    send_msg(it->get_socket(), msg);
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

