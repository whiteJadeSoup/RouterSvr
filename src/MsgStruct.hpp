#ifndef MSGSTRUCT_HPP_INCLUDED
#define MSGSTRUCT_HPP_INCLUDED

#include <string>

struct Msg_Login
{
    int m_id;
    Msg_Login (): m_id(0)
    {

    }


    template <typename Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
        ar & m_id;
    }

};

struct Msg_Logout
{
    int m_id;
    Msg_Logout (): m_id(0)
    {

    }


    template <typename Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
        ar & m_id;
    }

};


struct Msg_chat
{
    Msg_chat ()
        :m_send_id(0), m_recv_id(0)
    {

    }

    int m_send_id;
    int m_recv_id;
    std::string m_content;


    template <typename Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
        ar & m_send_id;
        ar & m_recv_id;
        ar & m_content;
    }
};



struct Msg_allocate_port
{
    int m_allocate_port;

    Msg_allocate_port ():m_allocate_port(0)
    {

    }

    template <typename Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
        ar & m_allocate_port;
    }
};

#endif // MSGSTRUCT_HPP_INCLUDED
