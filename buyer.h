#ifndef _BUYER_H_
#define _BUYER_H_

#include "peer.h"

class Buyer : public Peer {
public:
	Buyer(int peerId, const char *netFileName)
	: Peer(peerId, netFileName) {
		_hopCount = 3;
	}

protected:
	void processMessage(int rfd) {
		std::cout << "This is a buyer.\n";
	}

protected:
	int _hopCount;
};

#endif