#ifndef _SELLER_H_
#define _SELLER_H_

#include "peer.h"
#include <ctime>

const int goodsAmount = 2;

class Seller : public Peer {
public:
	Seller(int peerId, const char *netFileName)
	: Peer(peerId, netFileName) {
		srand (time(NULL));
		randPickGoods(goodsAmount);
	}

	// This loop will wait for a client to connect. When the client connects, it creates a
	// new thread for the client and starts waiting again for a new client.
	virtual int Run() {
		startServer();
	}

protected:
	void processMessage(int rfd);

	void randPickGoods(int amount);

	int reply(int rfd, const char *msg);

protected:
	Goods _goods;
	int _amount;
};

#endif