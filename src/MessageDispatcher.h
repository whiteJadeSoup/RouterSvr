#ifndef MESSAGEDISPATCHER_H
#define MESSAGEDISPATCHER_H



#include <map>
#include <functional>
#include <memory>

#include <google/protobuf/message.h>

using namespace std;



class Connection;

class Callback
{
public:
    typedef function<void (shared_ptr<google::protobuf::Message>)> ProtobufMessageCallback;

    Callback(const ProtobufMessageCallback& callback_);
    void on_message(shared_ptr<google::protobuf::Message> p_msg_);

private:
    ProtobufMessageCallback m_callback;
};


class MessageDispatcher
{
    typedef function<void (shared_ptr<google::protobuf::Message>)> MessageCallback;
    typedef map<int, shared_ptr<Callback> > CallbackMap;

public:
    MessageDispatcher();
    virtual ~MessageDispatcher();

    // 注册回调
    void register_message_callback(int, const Callback::ProtobufMessageCallback& );
    // 调用回调
    void on_message(int, shared_ptr<google::protobuf::Message>);

private:
    CallbackMap m_callbacks;
};

#endif // MESSAGEDISPATCHER_H
