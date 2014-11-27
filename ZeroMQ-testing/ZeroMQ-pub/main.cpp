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
    zmq::socket_t * socket = new zmq::socket_t(context, ZMQ_PUB);
    std::string publishAddress = "epgm://" + eth0IP + ";239.192.1.1:5555";
    //std::string publishAddress = "tcp://*:5555";
    socket->bind(publishAddress.c_str());
    //int linger = 0;
    //socket->setsockopt(ZMQ_LINGER, &linger, sizeof(linger));

    // Send the message
    while (1)
    {
        std::cout << "Sending Heartbeat Message" << std::endl;
        std::string szMessageData = "Foo";
        
        zmq::message_t zmqMsgOut(sizeof(szMessageData) + 1);
        memcpy((void *)zmqMsgOut.data(), szMessageData.c_str(), sizeof(szMessageData));
        
        bool status = socket->send(zmqMsgOut);
        if (status) {
            std::cout << "Heartbeat Message Sent." << std::endl;
        } 
        else {
            std::cerr << "Heartbeat Message NOT Sent." << std::endl;
        }

        Sleep(1000);
    }

    // Close and delete the socket
    socket->disconnect(publishAddress.c_str());
    socket->close();
    delete socket;

    return 0;
}
