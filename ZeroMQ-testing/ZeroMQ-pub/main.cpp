#include <iostream>
#include <string>

#include "zmq.hpp"

std::string eth0IP = "10.172.132.58";

/**
* Publisher example
*/
int main()
{
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
        std::string szMessageData = "Foo";
        
        zmq::message_t * zmqMsgOut = new zmq::message_t(sizeof(szMessageData) + 1);
        memcpy((void *)zmqMsgOut->data(), szMessageData.data(), sizeof(szMessageData));
        
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
