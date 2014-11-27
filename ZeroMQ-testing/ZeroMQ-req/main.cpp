#include <iostream>
#include <string>

#include "zmq.hpp"

struct TestMessage
{
    char p1[250];
    char p2[250];
};

/**
* Request example
*/
int main()
{
    for (int i = 0; i < 10; i++)
    {
        // Set up the connection
        zmq::context_t context(1);
        zmq::socket_t * pZmqSocket = new zmq::socket_t(context, ZMQ_REQ);
        std::string connectAddress = "tcp://127.0.0.1:5555";
        pZmqSocket->connect(connectAddress.c_str());
        int linger = 0;
        pZmqSocket->setsockopt(ZMQ_LINGER, &linger, sizeof(linger));

        // Construct the REQUEST message
        TestMessage * pTestMsg = new TestMessage();
        strcpy(pTestMsg->p1, "Foo");
        strcpy(pTestMsg->p2, "Bar");

        zmq::message_t * pZmqMsgOut = new zmq::message_t(sizeof(TestMessage));
        memcpy((void *)pZmqMsgOut->data(), pTestMsg, sizeof(TestMessage));

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
