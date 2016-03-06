#include "seller.h"
#include <sstream>

void Seller::randPickGoods(int amount) {
	_amount = amount;
	_goods = static_cast<Goods>(std::rand() % GOODS_COUNT);
}

/// This function runs in a thread for every client, and reads incomming data.
/// It also writes the incomming data to all other clients.
void Seller::processMessage(int rfd) {
	std::cout << "This is a seller.\n";		
	std::cout << "I am selling " << _goods << " of " << _amount << " items.\n";

    char buf[MAXLEN];
    int buflen;
    int wfd;

    for(;;)
    {
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
        std::cout << buf << std::endl;
    }
}