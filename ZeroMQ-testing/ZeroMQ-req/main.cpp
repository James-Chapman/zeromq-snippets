#include <iostream>
#include <cstring>
#include <vector>
#include <sstream>
#include <cstdint>

#include "zmq.hpp"

//typedef char BYTE;

// Struct representing the data to send down the wire
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


void printData(TestMessage & data)
{
    std::cout << std::endl;
    std::cout << "m_TotalSize: " << data.m_MessageSize << std::endl;
    std::cout << "m_p1Size: " << data.m_p1Size << std::endl;
    std::cout << "m_p2Size: " << data.m_p2Size << std::endl;
    std::cout << "m_p1: " << data.m_p1 << std::endl;
    std::cout << "m_p2: " << data.m_p2 << std::endl;
    std::cout << std::endl;
}


/**
* Request example
*/
int main()
{
    for (int i = 0; i < 10; i++)
    {
        const char * buffer = nullptr;

        // Set up the connection
        zmq::context_t context(1);
        zmq::socket_t * pZmqSocket = new zmq::socket_t(context, ZMQ_REQ);
        std::string connectAddress = "tcp://127.0.0.1:5555";
        pZmqSocket->connect(connectAddress.c_str());
        int linger = 0;
        pZmqSocket->setsockopt(ZMQ_LINGER, &linger, sizeof(linger));

        // Construct the REQUEST message
        TestMessage testMsg = TestMessage();
        testMsg.m_p1 = "Foo";
        testMsg.m_p2 = "Bar";
        testMsg.m_p1Size = sizeof(testMsg.m_p1);
        testMsg.m_p2Size = sizeof(testMsg.m_p2);
        testMsg.m_MessageSize = sizeof(testMsg);

        // create stream object and serialise
        std::ostringstream out;
        testMsg.serialize(out);
        std::string s(out.str());
        buffer = s.c_str();

        zmq::message_t * pZmqMsgOut = new zmq::message_t(sizeof(buffer));
        memcpy(pZmqMsgOut->data(), buffer, sizeof(buffer));

        printData(testMsg);
        std::cout << "sizeof(buffer): " << sizeof(buffer) << std::endl;

        // Send the message
        try
        {
            if (pZmqSocket->send(*pZmqMsgOut))
            {
                std::cout << "REQUEST Message " << i << " Sent" << std::endl;
            }
            else
            {
                std::cerr << "REQUEST Message " << i << " NOT Sent." << std::endl;
            }
        }
        catch (std::exception& e)
        {
            std::cerr << "Exception caught during socket.send(): " << e.what() << std::endl;
        }

        // Wait for a reponse (REPLY)
        int nRetries = 3;
        while (1)
        {
            //  Poll socket for a reply, with timeout
            zmq::pollitem_t items[] = { { *pZmqSocket, 0, ZMQ_POLLIN, 0 } };
            zmq::poll(&items[0], 1, 3000); // 3000 = 3 second timeout

            //  If we get a reply, process it
            if (items[0].revents & ZMQ_POLLIN)
            {
                zmq::message_t * pZmqMsgIn = new zmq::message_t();
                pZmqSocket->recv(pZmqMsgIn);
                std::string * pszMessageData = static_cast<std::string *>(pZmqMsgIn->data());
                std::cout << "REPLY Message Received" << std::endl;
                delete pZmqMsgIn;
                break;
            }
            else
            {
                --nRetries;
                if (nRetries == 0)
                {
                    std::cout << "REQUEST Message sent but no REPLY received" << std::endl;
                    break;
                }
            }
        } // end while

        // Close the connection
        pZmqSocket->disconnect(connectAddress.c_str());
        pZmqSocket->close();
        delete pZmqMsgOut;
        delete pZmqSocket;

        Sleep(1000);
    }

    return 0;
}
