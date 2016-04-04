#include "MessageDispatcher.h"
#include <iostream>


MessageDispatcher::MessageDispatcher()
{
    //ctor
}

MessageDispatcher::~MessageDispatcher()
{
    //dtor
}


void MessageDispatcher::register_message_callback(int type_, const Callback::ProtobufMessageCallback& callback_)
{
    auto it = m_callbacks.find(type_);
    if (it != m_callbacks.end())
    {
        cout << "error!" << "type: " << type_ << " callback is registered!" << endl;
    }
    else
    {
        cout << "register type: " << type_ << endl;
        shared_ptr<Callback> pd (new Callback(callback_));
        m_callbacks.insert(make_pair(type_, pd));
    }
}



void MessageDispatcher::on_message(int type_, shared_ptr<google::protobuf::Message> p_msg)
{
    auto it = m_callbacks.find(type_);

    if (it != m_callbacks.end())
    {
        it->second->on_message(p_msg);
    }
    else
    {
        cout << "error! Not found type! type: " << type_ << endl;
    }
}










///////////////////////////////////////
//
//
//

Callback::Callback(const ProtobufMessageCallback& callback_)
  : m_callback(callback_)
{

}


void Callback::on_message(shared_ptr<google::protobuf::Message> p_msg_)
{
    assert(p_msg_ != nullptr);
    m_callback(p_msg_);
}














