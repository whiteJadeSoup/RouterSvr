#ifndef CMSG_H
#define CMSG_H

#include <boost/archive/text_oarchive.hpp>
#include <sstream>
#include <string>

using namespace std;

class CMsg
{
public:
    CMsg();
    ~CMsg();
    CMsg(const CMsg& other);
    CMsg& operator=(const CMsg& other);

public:

    const string& get_send_data() const;
    int send_data_len();

    void set_msg_type (int);
    int get_msg_type();

    // 重置到默认值
    void clear();

public:
    template <class T >
    void serialization_data_Asio(const T& t)
    {
        ostringstream os;
        boost::archive::text_oarchive oa(os);
        oa & t;

        m_send_data.clear();
        m_send_data = os.str();
        cout << "now, string: " << m_send_data << endl;
    }

    template <class T>
    void serialization_data_protobuf(const T& t)
    {
        m_send_data.clear();
        bool result = t.SerializeToString(&m_send_data);

        cout << "serialize result: " << result << endl;
    }



private:
    int m_type;
    std::string m_send_data;

};

#endif // CMSG_H
