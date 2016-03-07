#include "seller.h"
#include <sstream>
#include <cstring>

void Seller::randPickGoods(int amount) {
	_amount = amount;
	_goods = static_cast<Goods>(std::rand() % GOODS_COUNT);
    std::cout << "I am now selling " << goodsNames[_goods] << " of " << _amount << " items.\n";
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

    //read incomming message.
    buflen = read(rfd, buf, sizeof(buf));

    pthread_mutex_lock(&_mutex_state);
    FD_CLR(rfd, &_the_state);      // free fd's from  clients
    pthread_mutex_unlock(&_mutex_state);
    close(rfd);

    if (buflen <= 0)
    {
        std::cout << "client disconnected. Clearing fd. " << rfd << std::endl ;
/*        pthread_mutex_lock(&_mutex_state);
        FD_CLR(rfd, &_the_state);      // free fd's from  clients
        pthread_mutex_unlock(&_mutex_state);
        close(rfd);*/
        pthread_exit(NULL);
        return;
    }

    // process the purchase request
    // check Fish 0
    // purchase Salt 1
    std::cout << buf << std::endl;
    std::string requestType;
    Goods goods;
    int var;
    std::vector<int> path;
    decodeMessage(buf, requestType, goods, var, path);

    int originPeerId = path[0];
    int lastNbPeerId = path.back();
    std::cout << requestType << " request from peer " << originPeerId << " to buy " << goodsNames[goods] << std::endl;
    std::cout << "goods amout: " << _amount << std::endl << std::endl;

    pthread_mutex_lock(&_mutex_state);

    if(requestType == "purchase") {
        if(goods == _goods) {
        	if(_amount > 0) {
        		reply(originPeerId, "DEAL");
        		_amount--;

        		if(_amount == 0) {
                    std::cout << goodsNames[_goods] << " seld out.\n";
        			randPickGoods(goodsAmount);
                }
        	}
        }
    } 
    else if(requestType == "look_up") {
    	if(goods == _goods && _amount > 0) {
            std::string msg = encodeMessage("reply", _goods, _peerId, path.begin(), path.end() - 1);
    		reply(lastNbPeerId, msg.c_str());
        }
        else {
        	floodingMessage(buf);
        }
    } 
    else if(requestType == "reply") {
        int sellerPeerId = var;
        std::string msg = encodeMessage("reply", goods, sellerPeerId, path.begin(), path.end() - 1);
        reply(lastNbPeerId, msg.c_str());
    } 
    else {
        std::cout << "Unrecognized request type " << requestType << std::endl;
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
}

/*int Seller::reply(int rfd, const char *msg) {
	return sendServerMessage(rfd, msg);
}*/