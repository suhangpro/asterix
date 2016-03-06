#include "seller.h"
#include <sstream>

void Seller::randPickGoods(int amount) {
	_amount = amount;
	_goods = static_cast<Goods>(std::rand() % GOODS_COUNT);
}

// This loop will wait for a client to connect. When the client connects, it creates a
// new thread for the client and starts waiting again for a new client.
int Seller::Run() {
	std::string welcome_msg("Welcome to this telnet chess server.\n");

    pthread_t threads[MAXFD]; //create 10 handles for threads.

    FD_ZERO(&_the_state); // FD_ZERO clears all the filedescriptors in the file descriptor set fds.

    Message *pMsg = new Message;

    while(1) // start looping here
    {
        int rfd;
        void *arg;

        // if a client is trying to connect, establish the connection and create a fd for the client.
        rfd = establishServerConnection();

        if (rfd >= 0)
        {
            std::cout << "Client connected. Using file desciptor " << rfd << std::endl;
            if (rfd > MAXFD)
            {
                std::cout << "To many clients trying to connect." << std::endl;
                close(rfd);
                // continue;
            }

            pthread_mutex_lock(&_mutex_state);  // Make sure no 2 threads can create a fd simultanious.

            FD_SET(rfd, &_the_state);  // Add a file descriptor to the FD-set.

            pthread_mutex_unlock(&_mutex_state); // End the mutex lock

            // std::cout << "Run, rfd: " << rfd << std::endl;
            pMsg->p = this;
        	pMsg->rfd = rfd;
        	arg = (void*)(pMsg);
/*        	std::cout << "Run, pMsg: " << pMsg << std::endl;
        	std::cout << "Run, arg: " << arg << std::endl;*/
            // arg = (void *)(&rfd);

            sendServerMessage(rfd, welcome_msg); // send a welcome message/instructions.

            // now create a thread for this client to intercept all incomming data from it.
            pthread_create(&threads[rfd], NULL, readTcpServer, arg);
        }
    }

    delete pMsg;
}

/// This function runs in a thread for every client, and reads incomming data.
/// It also writes the incomming data to all other clients.
void Seller::processMessage(int rfd) {
	std::cout << "This is a seller.\n";		
	std::cout << "I am selling " << _goods << " of " << _amount << " items.\n";

    char buf[MAXLEN];
    int buflen;
    int wfd;

/*    for(;;)
    {*/
        //read incomming message.
        buflen = read(rfd, buf, sizeof(buf));
        if (buflen <= 0)
        {
            std::cout << "client disconnected. Clearing fd. " << rfd << std::endl ;
            pthread_mutex_lock(&_mutex_state);
            FD_CLR(rfd, &_the_state);      // free fd's from  clients
            pthread_mutex_unlock(&_mutex_state);
            close(rfd);
            pthread_exit(NULL);
        }

        // send the data to the other connected clients
        pthread_mutex_lock(&_mutex_state);

        for (wfd = 3; wfd < MAXFD; ++wfd)
        {
            if (FD_ISSET(wfd, &_the_state) && (rfd != wfd))
            {
                  // add the users FD to the message to give it an unique ID.
                    std::string userfd;
                    std::stringstream out;
                    out << wfd;
                    userfd = out.str();
                    userfd = userfd + ": ";

                sendServerMessage(wfd, userfd);
                sendServerMessage(wfd, buf);
            }



        }

        pthread_mutex_unlock(&_mutex_state);

        // do_command (buf,rfd) ;
        std::cout << buf;

        close(rfd);
        pthread_exit(NULL);
/*    }*/
}