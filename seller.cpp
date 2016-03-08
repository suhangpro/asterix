#include "seller.h"
#include <sstream>
#include <cstring>
#include <cstdio>

void Seller::randPickGoods(int amount) {
	_amount = amount;
	_goods = static_cast<Goods>(std::rand() % GOODS_COUNT);
    // std::cout << "I am now selling " << goodsNames[_goods] << ". " << _amount << " items left.\n";
    // std::cout << "[seller-" << _peerId << "] I just got _amount " << goodsNames[_goods] << ".\n";
    std::printf("[  seller-%03d] I just got %d %s.\n", _peerId, _amount, goodsNames[_goods]);
}

/// This function runs in a thread for every client, and reads incomming data.
/// It also writes the incomming data to all other clients.
void Seller::processMessage(int rfd) {
	// std::cout << "This is a seller (peer " << _peerId << "). I am selling " << goodsNames[_goods] << ". " << _amount << " items left.\n";

    char buf[MAXLEN];
    int buflen;

    //read incomming message.
    buflen = read(rfd, buf, sizeof(buf));

    pthread_mutex_lock(&_mutex_state);
    FD_CLR(rfd, &_the_state);      // free fd's from  clients
    pthread_mutex_unlock(&_mutex_state);

    if (buflen <= 0)
    {
        std::printf("[  seller-%03d] Client disconnected.\n", _peerId);
        close(rfd);
        pthread_exit(NULL);
        return;
    }

    // process the purchase request
    // check Fish 0
    // purchase Salt 1
    // std::cout << "[seller-" << _peerId << "] ";// << buf << std::endl;
    // printMessage(buf);

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
//                std::cout << "================================== end of the deal ==========================================\n";
        		// sendPeerMessage(originPeerId, msg.c_str());
                reply(rfd, msg.c_str());

                std::string msg = encodeMessage("deal", _goods, -1, -1);
                if(_amount > 1) {
                    std::printf("[  seller-%03d] I just #sold# %s to peer #%d. I still have %d %s.\n", _peerId, goodsNames[_goods], path.front(), _amount - 1, goodsNames[_goods]);
                }
                else {
                    std::printf("[  seller-%03d] I just #sold# %s to peer #%d. It's last one.\n", _peerId, goodsNames[_goods], path.front());
                }

        		_amount--;
                isCloseSocket = false;

        		if(_amount == 0) {
                    // std::cout << goodsNames[_goods] << " seld out.\n";
        			randPickGoods(goodsAmount);
                }
        	}
            else {
                std::string msg = encodeMessage("fail_deal", _goods, -1, -1);
                reply(rfd, msg.c_str());

                std::printf("[  seller-%03d] Peer #%d wants to buy %s from me. Too late!\n", _peerId, path.front(), goodsNames[goods]);
            }
        }
        else {
            std::string msg = encodeMessage("fail_deal", _goods, -1, -1);
            reply(rfd, msg.c_str());

            std::printf("[  seller-%03d] Peer #%d wants to buy %s from me. Too late!\n", _peerId, path.front(), goodsNames[goods]);
        }
    } 
    else if(requestType == "look_up") {
    	if(goods == _goods && _amount > 0) {
  //          std::cout << "\n============================ start of the deal (maybe) ====================================\n";
            std::string msg = encodeMessage("reply", _goods, _peerId, path.begin(), path.end() - 1);
    		sendPeerMessage(lastNbPeerId, msg.c_str());

            std::printf("[  seller-%03d] Trying to sell %s to buyer %d. Path back is ", _peerId, goodsNames[_goods], path.front());
            for(size_t i = path.size() - 1; i > 0; --i)
                std::printf("%d->", path[i]);
            std::printf("%d.\n", path[0]);
        }
        else {
        	floodingMessage(buf);

            //std::printf("[messager-%03d] Peer #%d wants to buy %s. Hop count: %d. Path is ", _peerId, path.front(), goodsNames[goods], var);
            std::printf("[messager-%03d] Peer #%d wants to buy %s. Path is ", _peerId, path.front(), goodsNames[goods]);
            for(size_t i = 0; i < path.size() - 1; ++i)
                std::printf("%d->", path[i]);
            std::printf("%d.\n", path.back());
        }
    } 
    else if(requestType == "reply") {
        int sellerPeerId = var;
        std::string msg = encodeMessage("reply", goods, sellerPeerId, path.begin(), path.end() - 1);
        sendPeerMessage(lastNbPeerId, msg.c_str());

        std::printf("[messager-%03d] Peer #%d wants to sell %s to peer #%d. Path back is ", _peerId, sellerPeerId, goodsNames[goods], path.front());
        for(size_t i = path.size() - 1; i > 0; --i)
            std::printf("%d->", path[i]);
        std::printf("%d.\n", path[0]);
    } 
    else {
        std::cout << "[Error] Unrecognized request type " << requestType << std::endl;
    }

    _activeConnect--;

    pthread_mutex_unlock(&_mutex_state);

    //std::cout << std::endl;

    if(isCloseSocket)
        close(rfd);

    pthread_exit(NULL);
}

int Seller::reply(int rfd, const char *msg) {
	return sendServerMessage(rfd, msg);
}
