#ifndef CMSG_H
#define CMSG_H

#include <boost/archive/text_oarchive.hpp>
#include <string>
#include <iostream>
#include <sstream>

class CMsg
{
public:
    CMsg();
    ~CMsg();
    CMsg(const CMsg& other);
    CMsg& operator=(const CMsg& other);

    void set_msg_type (int);
    int get_msg_type();


    template<typename T>
    void set_send_data(const T& t)
    {
        std::ostringstream os;
        boost::archive::text_oarchive oa(os);
        oa & t;

        m_send_data.clear();
        m_send_data = os.str();
        std::cout << "now, string: " << m_send_data << std::endl;
    }


    std::string get_send_data();

    int send_data_len();

protected:
private:
    int m_type;
    int m_data_len;
    std::string m_send_data;

};

#endif // CMSG_H
