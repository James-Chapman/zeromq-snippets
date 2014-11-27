#include <iostream>
#include <string>

#include "zmq.hpp"

std::string eth0IP = "10.172.132.58";

/**
* Subscriber example
*/
int main()
{
    zmq::context_t zmqContext(1);
    zmq::socket_t * pZmqSocket = new zmq::socket_t(zmqContext, ZMQ_SUB);
    std::string subscribeAddress = "epgm://" + eth0IP + ";239.192.1.1:5555";
    try
    {
        pZmqSocket->connect(subscribeAddress.c_str());
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception caught during socket.connect(): " << e.what() << std::endl;
    }
    pZmqSocket->setsockopt(ZMQ_SUBSCRIBE, "", 0);
    std::cout << "Heartbeat responder BOUND" << std::endl;
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
