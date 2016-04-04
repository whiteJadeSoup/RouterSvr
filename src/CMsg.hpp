#ifndef CMSG_H
#define CMSG_H

#include <sstream>
#include <string>
#include <memory>

#include <google/protobuf/message.h>


using namespace std;


class CMsg
{
public:
    CMsg();
    ~CMsg();
    CMsg(const CMsg& other);
    CMsg& operator=(const CMsg& other);

public:
    bool encode(int, const google::protobuf::Message&);

    string get_send_data();

private:
    string m_send_data;
};

#endif // CMSG_H
