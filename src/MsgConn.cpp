#include "MsgConn.hpp"
#include "M2RMsgTypeDefine.hpp"
#include "MsgStruct.hpp"

#include "allocate.pb.h"


#include <functional>


/**********************************************
 *
 *
 *
 */



MsgConn::MsgConn(io_service& io_)
  :Connection(io_),
   m_min_and_max(std::make_tuple(0,0))
{
    initialization();

}



/**********************************************
 *
 *
 *
 */

void MsgConn::initialization()
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


void MsgConn::on_connect()
{

    m_dispatcher.register_message_callback((int)M2R::DISPATCH_CHAT,
        bind(&MsgConn::handle_dispatch_chat, this, std::placeholders::_1));

    m_dispatcher.register_message_callback((int)M2R::USER_LOGIN,
        bind(&MsgConn::handle_user_login, this, std::placeholders::_1));

    m_dispatcher.register_message_callback((int)M2R::USER_LOGOUT,
        bind(&MsgConn::handle_user_logout, this, std::placeholders::_1));

    m_dispatcher.register_message_callback((int)M2R::ALLOCATE_PORT,
        bind(&MsgConn::handle_allocate_port, this, std::placeholders::_1));


    read_head();

    m_vecMsgSvrs.emplace_back(this, socket());
}


void MsgConn::on_disconnect()
{


}


void MsgConn::on_recv_msg(int type_,pb_message_ptr p_msg_)
{

    std::cout << "Recv msg type: " << type_ << std::endl;

    m_dispatcher.on_message(type_, p_msg_);

}




/**********************************************
 *
 *  msg process function
 *
 */

void MsgConn::handle_allocate_port(pb_message_ptr p_msg_)
{


    int nAllocatePort = 0;
    int nMinPort = std::get<0>(m_min_and_max);
    int nMaxPort = std::get<1>(m_min_and_max);


    do
    {
        nAllocatePort = random(nMinPort, nMaxPort);
    }
    while(m_PortUses[nAllocatePort]);

    m_PortUses[nAllocatePort] = true;


    std::cout << "allocate port: " << nAllocatePort << std::endl;


    IM::Allocate allocate;
    allocate.set_port(nAllocatePort);

    CMsg packet;
    packet.encode((int)M2R::ALLOCATE_PORT, allocate);
    send(packet);
}





void MsgConn::handle_user_login(pb_message_ptr p_msg_)
{
    try
    {
        GOOGLE_PROTOBUF_VERIFY_VERSION;
        using namespace google::protobuf;

        auto descriptor = p_msg_->GetDescriptor();
        const Reflection* rf = p_msg_->GetReflection();
        const FieldDescriptor* f_id = descriptor->FindFieldByName("id");

        assert(f_id && f_id->type()==FieldDescriptor::TYPE_INT64);


        int64_t id = rf->GetInt64(*p_msg_, f_id);
        cout << "login id: " << id << endl;

        auto it = std::find_if(m_vecMsgSvrs.begin(), m_vecMsgSvrs.end(),
            [this] (MsgSvrClient& m)
            {
                return (MsgConn*)m.get_context() == this;
            });

        if (it == m_vecMsgSvrs.end())
        {
            cout << "error! this msgsvr is not connect! conn id: " << get_conn_id() << endl;
            return;
        }

        it->add_user(id);
    }
    catch (exception& e)
    {
        cout << "# ERR: exception in " << __FILE__;
        cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
        cout << "# ERR: " << e.what() << endl;
    }

}

void MsgConn::handle_user_logout(pb_message_ptr p_msg_)
{
    try
    {

        GOOGLE_PROTOBUF_VERIFY_VERSION;
        using namespace google::protobuf;

        auto descriptor = p_msg_->GetDescriptor();
        const Reflection* rf = p_msg_->GetReflection();
        const FieldDescriptor* f_id = descriptor->FindFieldByName("id");


        assert(f_id && f_id->type()==FieldDescriptor::TYPE_INT64);


        int64_t id = rf->GetInt64(*p_msg_, f_id);
        cout << "logout id: " << id << endl;

        auto it = std::find_if(m_vecMsgSvrs.begin(), m_vecMsgSvrs.end(),
            [this] (MsgSvrClient& m)
            {
                return (MsgConn*)m.get_context() == this;
            });

        if (it == m_vecMsgSvrs.end())
        {
            cout << "error! this msgsvr is not connect! conn id: " << get_conn_id() << endl;
            return;
        }

        it->del_user(id);
    }
    catch (exception& e)
    {
        cout << "# ERR: exception in " << __FILE__;
        cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
        cout << "# ERR: " << e.what() << endl;
    }
}



void MsgConn::handle_dispatch_chat(pb_message_ptr p_msg_)
{

    try
    {

        GOOGLE_PROTOBUF_VERIFY_VERSION;
        using namespace google::protobuf;


        auto descriptor = p_msg_->GetDescriptor();
        const Reflection* rf = p_msg_->GetReflection();
        const FieldDescriptor* f_send_id = descriptor->FindFieldByName("send_id");
        const FieldDescriptor* f_recv_id = descriptor->FindFieldByName("recv_id");
        const FieldDescriptor* f_content = descriptor->FindFieldByName("content");


        assert(f_send_id && f_send_id->type()==FieldDescriptor::TYPE_INT64);
        assert(f_recv_id && f_recv_id->type()==FieldDescriptor::TYPE_INT64);
        assert(f_content && f_content->type()==FieldDescriptor::TYPE_STRING);


        int64_t send_id = rf->GetInt64(*p_msg_, f_send_id);
        int64_t recv_id = rf->GetInt64(*p_msg_, f_recv_id);
        string  content = rf->GetString(*p_msg_, f_content);



        cout << "chat sendid: "     << send_id
             << "chat recvid: "     << recv_id
             << "content: "         << content << endl;

        auto it = find_if(m_vecMsgSvrs.begin(), m_vecMsgSvrs.end(),
            [=] (MsgSvrClient& m)
            {
                return m.is_in_svr(recv_id);
            });

        if (it == m_vecMsgSvrs.end())
        {
            std::cout << "userid: " << recv_id << " offline!" << std::endl;
            return;
        }


        CMsg packet;
        packet.encode((int)M2R::SEND_CHAT, *p_msg_);
        send(packet, it->get_socket());
    }
    catch (exception& e)
    {
        cout << "# ERR: exception in " << __FILE__;
        cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
        cout << "# ERR: " << e.what() << endl;
    }
}





/**********************************************
 *
 *
 *
 */


int MsgConn::random(int min, int max)
{
    srand((int)time(NULL));
    return min + rand() % (max-min+1);
}

std::tuple<int,int> MsgConn::get_min_max_port(std::string ports)
{
    int min=0,max=0;
    int index = ports.find('-');
    min = std::stoi(ports.substr(0, index));
    max = std::stoi(ports.substr(index+1, ports.size()));

    return std::make_tuple(min, max);
}

