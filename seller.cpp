#include "seller.h"
#include <sstream>
#include <cstring>

void Seller::randPickGoods(int amount) {
	_amount = amount;
	_goods = static_cast<Goods>(std::rand() % GOODS_COUNT);
    std::cout << "I am now selling " << goodsNames[_goods] << ". " << _amount << " items left.\n";
}

/// This function runs in a thread for every client, and reads incomming data.
/// It also writes the incomming data to all other clients.
void Seller::processMessage(int rfd) {
	std::cout << "This is a seller (peer " << _peerId << "). I am selling " << goodsNames[_goods] << ". " << _amount << " items left.\n";

    char buf[MAXLEN];
    int buflen;

    //read incomming message.
    buflen = read(rfd, buf, sizeof(buf));

    pthread_mutex_lock(&_mutex_state);
    FD_CLR(rfd, &_the_state);      // free fd's from  clients
    pthread_mutex_unlock(&_mutex_state);

    if (buflen <= 0)
    {
        std::cout << "[Seller-processMessage] Client disconnected." << std::endl ;
        close(rfd);
        pthread_exit(NULL);
        return;
    }

    // process the purchase request
    // check Fish 0
    // purchase Salt 1
    std::cout << "[Seller-processMessage] ";// << buf << std::endl;
    printMessage(buf);

    std::string requestType;
    Goods goods;
    int var;
    std::vector<int> path;
    decodeMessage(buf, requestType, goods, var, path);

    int originPeerId = path[0];
    int lastNbPeerId = path.back();

    pthread_mutex_lock(&_mutex_state);

    bool isCloseSocket = true;

    if(requestType == "purchase") {
        if(goods == _goods) {
        	if(_amount > 0) {
                std::string msg = encodeMessage("deal", _goods, -1, -1);

                std::cout << "Deal made.\n================================== end of the deal ==========================================\n";
        		// sendPeerMessage(originPeerId, msg.c_str());
                reply(rfd, msg.c_str());
        		_amount--;
                isCloseSocket = false;

        		if(_amount == 0) {
                    std::cout << goodsNames[_goods] << " seld out.\n";
        			randPickGoods(goodsAmount);
                }
        	}
        }
    } 
    else if(requestType == "look_up") {
    	if(goods == _goods && _amount > 0) {
            std::cout << "\n============================ start of the deal (maybe) ====================================\n";
            std::string msg = encodeMessage("reply", _goods, _peerId, path.begin(), path.end() - 1);
    		sendPeerMessage(lastNbPeerId, msg.c_str());
        }
        else {
        	floodingMessage(buf);
        }
    } 
    else if(requestType == "reply") {
        int sellerPeerId = var;
        std::string msg = encodeMessage("reply", goods, sellerPeerId, path.begin(), path.end() - 1);
        sendPeerMessage(lastNbPeerId, msg.c_str());
    } 
    else {
        std::cout << "Unrecognized request type " << requestType << std::endl;
    }

    _activeConnect--;

    pthread_mutex_unlock(&_mutex_state);

    std::cout << std::endl;

    if(isCloseSocket)
        close(rfd);

    pthread_exit(NULL);
}

int Seller::reply(int rfd, const char *msg) {
	return sendServerMessage(rfd, msg);
}