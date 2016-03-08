#include "buyer.h"
#include "seller.h"
#include <cstdlib>
#include <cstdio>

// ./run_peer peerId netFile isSeller
int main(int argc, char *argv[]) {
	setbuf(stdout, NULL);

	int peerId = std::atoi(argv[1]);
	const char *netFileName = argv[2];
	int isSeller = std::atoi(argv[3]);

	Peer *p = 0;
	if(isSeller)
		p = new Seller(peerId, netFileName);
	else
		p = new Buyer(peerId, netFileName);

	// std::cout << "Before run p: " o<< p << std::endl;
	p->Run();

	delete p;

	return 0;
}