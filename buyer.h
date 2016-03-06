#ifndef _BUYER_H_
#define _BUYER_H_

#include "peer.h"

class Buyer : public Peer {
public:
	Buyer(int peerId, const char *netFileName)
	: Peer(peerId, netFileName) {
		_hopCount = 3;
	}

	virtual int Run();

protected:
	int lookUp();

	int connect2Peer(int peerId, const char *msg);

	void processMessage(int rfd) {
		std::cout << "This is a buyer.\n";
	}

protected:
	int _hopCount;
	Goods _interestGoods;
};

#endif