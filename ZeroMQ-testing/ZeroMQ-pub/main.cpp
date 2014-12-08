#include <iostream>
#include <string>
#include <thread>

#include "zmq.hpp"

#define PING_PORT_NUMBER 9999
#define PING_MSG_SIZE    1
#define PING_INTERVAL    1000  //  Once per second

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
    // Set up the connection
    zmq::context_t context(1);
    zmq::socket_t * pZmqSocket = new zmq::socket_t(context, ZMQ_PUB);
    std::string publishAddress = "tcp://*:5555";

    try
    {
        pZmqSocket->bind(publishAddress.c_str());
        std::cout << publishAddress << " bound" << std::endl;
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception caught during socket.bind(): " << e.what() << std::endl;
        Sleep(5000);
        return 1;
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
