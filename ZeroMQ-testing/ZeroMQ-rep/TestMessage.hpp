#ifndef _TESTMESSAGE_H_
#define _TESTMESSAGE_H_

#include <iostream>
#include <sstream>
#include <cstdint>
#include <vector>

/**
* Struct representing the data to send down the wire
*/ 
struct TestMessage
{
    uint32_t m_MessageSize;
    uint16_t m_p1Size;
    uint16_t m_p2Size;
    std::string m_p1;
    std::string m_p2;

    std::ostream& serialize(std::ostream &out) const
    {
        out << m_MessageSize;
        out << ",";
        out << m_p1Size;
        out << ",";
        out << m_p2Size;
        out << ",";
        out << m_p1;
        out << ",";
        out << m_p2;
        return out;
    }

    std::istream& deserialize(std::istream &in)
    {
        if (in)
        {
            char comma;
            in >> m_MessageSize;
            in >> comma;
            in >> m_p1Size;
            in >> comma;
            in >> m_p2Size;
            in >> comma;
            {
                std::vector<char> tmp(m_p1Size);
                in.read(&tmp[0], m_p1Size);
                m_p1.assign(&tmp[0], m_p1Size);
            }
            in >> comma;
            {
                std::vector<char> tmp(m_p2Size);
                in.read(&tmp[0], m_p2Size);
                m_p2.assign(&tmp[0], m_p2Size);
            }
        }
        return in;
    }
};

#endif // _TESTMESSAGE_H_
