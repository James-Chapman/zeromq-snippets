#include <iostream>
#include <string>

#include "zmq.hpp"

std::string eth0IP = "10.172.132.58";

struct TestMessage
{
    char p1[250];
    char p2[250];
};

/**
* Publisher example
*/
int main()
{
    std::string astring("12345678901234567890123456789012345678901234567890");

    // Set up the connection
    zmq::context_t context(1);
    zmq::socket_t * pZmqSocket = new zmq::socket_t(context, ZMQ_PUB);
    std::string publishAddress = "epgm://" + eth0IP + ";239.192.1.1:5555";
    try
    {
        pZmqSocket->bind(publishAddress.c_str());
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception caught during socket.bind(): " << e.what() << std::endl;
    }

    // Send the message
    while (1)
    {
        std::cout << "Sending Heartbeat Message" << std::endl;
        TestMessage * pTestMsg = new TestMessage();
        strcpy(pTestMsg->p1, "Foo");
        strcpy(pTestMsg->p2, "Bar");
        
        zmq::message_t * zmqMsgOut = new zmq::message_t(sizeof(TestMessage));
        memcpy((void *)zmqMsgOut->data(), pTestMsg, sizeof(TestMessage));
        
        try
        {
            if (pZmqSocket->send(*zmqMsgOut))
            {
                std::cout << "Heartbeat Message Sent." << std::endl;
            }
            else
            {
                std::cerr << "Heartbeat Message NOT Sent." << std::endl;
            }
        }
        catch (std::exception& e)
        {
            std::cerr << "Exception caught during socket.send(): " << e.what() << std::endl;
        }

        Sleep(1000);
    }

    // Close and delete the socket
    pZmqSocket->disconnect(publishAddress.c_str());
    pZmqSocket->close();
    delete pZmqSocket;

    return 0;
}
