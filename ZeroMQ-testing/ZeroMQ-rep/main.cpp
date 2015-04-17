#include <iostream>
#include <string>
#include <vector>
#include <sstream>

#include "Message.hpp"
#include "zmq.hpp"




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
* Reply example
*/
int main()
{
    // Set up the network binding
    zmq::context_t zmqContext(1);
    zmq::socket_t * pZmqSocket = new zmq::socket_t(zmqContext, ZMQ_REP);
    std::string listenAddress = "tcp://*:5555";
    pZmqSocket->bind(listenAddress.c_str());
    while (1)
    {
        if (pZmqSocket->connected())
        {
            // Create a REQUEST message pointer
            zmq::message_t * pZmqMsgIn = new zmq::message_t();

            char * buffer = nullptr;

            // Receive the REQUEST message
            {
                try
                {
                    if (pZmqSocket->recv(pZmqMsgIn))
                    {
                        std::cout << "REQUEST Message Received" << std::endl;

                        buffer = (char *)(pZmqMsgIn->data());
                        std::istringstream iss(buffer);

                        TestMessage recvData = TestMessage();
                        recvData.deserialize(iss);

                        printData(recvData);

                    }
                    else
                    {
                        std::cout << "REQUEST Message NOT Received" << std::endl;
                    }
                }
                catch (std::exception& e)
                {
                    std::cerr << "Exception caught during socket.recv(): " << e.what() << std::endl;
                }


            }

            // Create the REPLY message and copy data from the REQUEST message
            zmq::message_t * pZmqMsgOut = new zmq::message_t(sizeof(pZmqMsgIn->data()));
            memcpy((void *)pZmqMsgOut->data(), pZmqMsgIn->data(), sizeof(pZmqMsgIn->data()));

            // Send the REPLY message
            {
                try
                {
                    if (pZmqSocket->send(*pZmqMsgOut))
                    {
                        std::cout << "REPLY Message Sent" << std::endl;
                    }
                    else
                    {
                        std::cerr << "REPLY Message NOT Sent." << std::endl;
                    }
                }
                catch (std::exception& e)
                {
                    std::cerr << "Exception caught during socket.send(): " << e.what() << std::endl;
                }
            }

            delete pZmqMsgIn;
        }
    }

    delete pZmqSocket;

    return 0;
}
