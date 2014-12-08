#include <iostream>
#include <string>

#include "zmq.hpp"
#include "zmq_utils.h"

struct TestMessage
{
    char p1[250];
    char p2[250];
};


/**
* Subscriber example
*/
int main()
{
    zmq::context_t zmqContext(1);
    zmq::socket_t * pZmqSocket = new zmq::socket_t(zmqContext, ZMQ_SUB);
    pZmqSocket->setsockopt(ZMQ_SUBSCRIBE, "", 0);

    std::string subscribeAddress = "tcp://127.0.0.1:5555";
    try
    {
        pZmqSocket->connect(subscribeAddress.c_str());
        std::cout << subscribeAddress << " bound" << std::endl;
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception caught during socket.connect(): " << e.what() << std::endl;
        Sleep(5000);
        return 1;
    }
    while (1)
    {
        std::cout << ".";
        //pZmqSocket->connected();
        zmq::message_t * pZmqMsgIn = new zmq::message_t();

        try
        {
            if (pZmqSocket->recv(pZmqMsgIn)) 
            {
                std::cout << "======== MESSAGE RECEIVED =========" << std::endl;
            }
            else 
            {
                std::cerr << "Error receiving message" << std::endl;
            }
        }
        catch (std::exception& e)
        {
            std::cerr << "Exception caught during socket.send(): " << e.what() << std::endl;
        }

        //std::string * szMessageData = static_cast<std::string *>(pZmqMsgIn->data());
        Sleep(1000);

        delete pZmqMsgIn;
    }
    delete pZmqSocket;

    return 0;
}
