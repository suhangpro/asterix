#include "seller.h"
#include <sstream>

void Seller::randPickGoods(int amount) {
	_amount = amount;
	_goods = static_cast<Goods>(std::rand() % GOODS_COUNT);
}

// This loop will wait for a client to connect. When the client connects, it creates a
// new thread for the client and starts waiting again for a new client.
int Seller::Run() {
	startServer();
}

/// This function runs in a thread for every client, and reads incomming data.
/// It also writes the incomming data to all other clients.
void Seller::processMessage(int rfd) {
	std::cout << "This is a seller.\n";		
	std::cout << "I am selling " << goodsNames[_goods] << " of " << _amount << " items.\n";

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

/*        // send the data to the other connected clients
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

        pthread_mutex_unlock(&_mutex_state);*/

        // process the purchase request
        // check Fish 0
        // purchase Salt 1
        std::stringstream ss;
        std::string requestType;
        int goods, peerId;

        ss << buf;
        ss >> requestType >> goods >> peerId;
        std::cout << requestType << " request from peer " << peerId << " to buy " << goodsNames[goods] << std::endl;

        std::cout << "goods amout: " << _amount << std::endl << std::endl;

        pthread_mutex_lock(&_mutex_state);

        if(requestType == "purchase") {
	        if(goods == _goods) {
	        	if(_amount > 0) {
	        		reply(rfd, "DEAL");
	        		_amount--;

	        		if(_amount == 0) 
	        			randPickGoods(goodsAmount);
	        	}
	        }
	        else
	        	reply(rfd, "SELDOUT");
        }
        else if(requestType == "check") {
        	if(goods == _goods && _amount > 0)
        		reply(rfd, "PLENTY");
/*	        else
	        	sendOut();*/
        }
/*        else {
        	reply(rfd, "I do not have this goods.");
        }*/

       _activeConnect--;

        pthread_mutex_unlock(&_mutex_state);

/*        std::cout << rfd << std::endl;
        std::cout << buflen << std::endl;
        std::cout << buf << std::endl;
        std::cout << "Closing the socket and exist the thread.\n";*/

        // close(rfd);
        pthread_exit(NULL);
/*    }*/
}

int Seller::reply(int rfd, const char *msg) {
	return sendServerMessage(rfd, msg);
}