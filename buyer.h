#ifndef _BUYER_H_
#define _BUYER_H_

#include "peer.h"
#include <ctime>
#include <vector>

const int MAX_WAIT_TIME = 5;
const int MAX_HOP_COUNT = 3;

class Buyer : public Peer {
public:
	Buyer(int peerId, const char *netFileName)
	: Peer(peerId, netFileName) {
		srand (time(NULL));
	}

	virtual int Run();

	friend void *startBuyerServer(void *arg);

protected:
	int lookUp();

	int sendRequest2Neighbor(int peerId, const char *msg);

	int getReply(int socketfd);

	int buy();

	void processMessage(int rfd) {
		std::cout << "This is a buyer.\n";
	}

protected:
	int _hopCount;
	Goods _interestGoods;

	std::vector<int> _sellers;
};

inline void *startBuyerServer(void *arg) {
	Buyer *p = (Buyer*)arg;
	p->startServer();
}

#endif