#include "CMsg.hpp"

#include <arpa/inet.h>
#include <sstream>
#include <iostream>

CMsg::CMsg()
{
    //ctor
}

CMsg::~CMsg()
{
    //dtor
}

CMsg::CMsg(const CMsg& other)
{
    //copy ctor
}

CMsg& CMsg::operator=(const CMsg& rhs)
{
    if (this == &rhs) return *this; // handle self assignment
    //assignment operator
    return *this;
}





/****************************************
 *
 *  tool functions
 */
bool CMsg::encode(int type_, const google::protobuf::Message& message_)
{

    m_send_data.clear();

    // 为头部预留空间
    m_send_data.resize(sizeof(int32_t));

    // 消息类型
    int32_t be_type = ::htonl(type_);
    m_send_data.append(reinterpret_cast<char*>(&be_type), sizeof(be_type));


    // 类名长度
    const string& type_name = message_.GetTypeName();
    int32_t name_len = type_name.size() + 1;
    int32_t be_name_len = ::htonl(name_len);
    m_send_data.append(reinterpret_cast<char*>(&be_name_len), sizeof(be_name_len));

    cout << "encode type name: " << type_name << endl;

    // 类名
    m_send_data.append(type_name.c_str(), name_len);

    // 序列化数据
    bool succ_flag = message_.AppendToString(&m_send_data);

    if (succ_flag)
    {
        // 数据总长度 不包括头
        int32_t len = m_send_data.size() - sizeof(int32_t);
        int32_t be_len = ::htonl(len);
        std::copy(reinterpret_cast<char*>(&be_len),
            reinterpret_cast<char*>(&be_len) + sizeof(be_len),
            m_send_data.begin());

        return true;
    }
    else
    {
        m_send_data.clear();
        std::cout << "encode packet fail!" << std::endl;
        return false;
    }
}

string CMsg::get_send_data()
{
    return m_send_data;
}


