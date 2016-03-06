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

	virtual int Run();

protected:
	void processMessage(int rfd);

	void randPickGoods(int amount);

	int reply(int rfd, const char *msg);

protected:
	Goods _goods;
	int _amount;
};

#endif