#include "buyer.h"
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <algorithm>
#include <fcntl.h>

int Buyer::Run() {
	// in case not all nodes are setup
	sleep(1);

/*	pthread_t serverThread;

	pthread_create(&serverThread, NULL, &startBuyerServer, (void*)this);
	std::cout << "server is running.\n";*/

	int iter = 0;
	while(1) {
		std::cout << "=========== iter " << ++iter << " ==============\n";
		lookUp();

		if(_sellers.size() > 0)
			buy();

		sleep(std::rand() % MAX_WAIT_TIME);
	}

/*	pthread_join(serverThread, NULL);*/

	return 0;
}

int Buyer::lookUp() {
	_sellers.clear();
	_interestGoods = static_cast<Goods>(std::rand() % GOODS_COUNT);

	char msg[MAXLEN];
	sprintf(msg, "check %d %d %d", _interestGoods, _peerId, MAX_HOP_COUNT);

	std::vector<int> nbSocketFds;
	for(size_t i = 0; i < _nbPeerIds.size(); ++i) {
		int socketfd = sendRequest2Neighbor(_nbPeerIds[i], msg);
		nbSocketFds.push_back(socketfd);
	}

	sleep(1);

	for(size_t i = 0; i < _nbPeerIds.size(); ++i) {
		int socketfd = nbSocketFds[i];
		int status = getReply(socketfd);		
		if(status == 0)
			_sellers.push_back(_nbPeerIds[i]);	
	}

	return 0;
}

int Buyer::sendRequest2Neighbor(int peerId, const char *msg) {
	int status;
    struct addrinfo host_info;       // The struct that getaddrinfo() fills up with data.
    struct addrinfo *host_info_list; // Pointer to the to the linked list of host_info's.

    // The MAN page of getaddrinfo() states "All  the other fields in the structure pointed
    // to by hints must contain either 0 or a null pointer, as appropriate." When a struct
    // is created in c++, it will be given a block of memory. This memory is not nessesary
    // empty. Therefor we use the memset function to make sure all fields are NULL.
    memset(&host_info, 0, sizeof host_info);

    std::cout << "Setting up the structs..."  << std::endl;

    host_info.ai_family = AF_UNSPEC;     // IP version not specified. Can be both.
    host_info.ai_socktype = SOCK_STREAM; // Use SOCK_STREAM for TCP or SOCK_DGRAM for UDP.

    // Now fill up the linked list of host_info structs with google's address information.
    status = getaddrinfo(_netIps[peerId].c_str(), _netPorts[peerId].c_str(), &host_info, &host_info_list);
    // status = getaddrinfo("127.0.0.1", "5555", &host_info, &host_info_list);
    // getaddrinfo returns 0 on succes, or some other value when an error occured.
    // (translated into human readable text by the gai_gai_strerror function).
    if (status != 0) {
    	std::cout << "getaddrinfo error" << gai_strerror(status) ;
    	return status;
    }


    std::cout << "Creating a socket..."  << std::endl;
    int socketfd ; // The socket descripter
    socketfd = socket(host_info_list->ai_family, host_info_list->ai_socktype,
                      host_info_list->ai_protocol);
    if (socketfd == -1) {
      std::cout << "socket error " ;
      return socketfd;
    }

    std::cout << "connect2Peer, socketfd: " << socketfd << std::endl;


    std::cout << "Connect()ing..."  << std::endl;
    status = connect(socketfd, host_info_list->ai_addr, host_info_list->ai_addrlen);
    if (status == -1) {
    	std::cout << "connect error" ;
    	return status;
    }


    std::cout << "send()ing message..."  << std::endl;
    int len;
    ssize_t bytes_sent;
    len = strlen(msg);
    std::cout << msg << std::endl;
    bytes_sent = send(socketfd, msg, len, 0);

    return socketfd;
}

int Buyer::getReply(int socketfd) {
	// set the socket non-blocking
	int flags = fcntl(socketfd, F_GETFL, 0);
    fcntl(socketfd, F_SETFL, flags | O_NONBLOCK);

    std::cout << "Waiting to recieve data..."  << std::endl;
    ssize_t bytes_recieved;
    char incomming_data_buffer[1000];
    bytes_recieved = recv(socketfd, incomming_data_buffer,1000, 0);
    // If no data arrives, the program will just wait here until some data arrives.
    if (bytes_recieved == 0) {
    	std::cout << "host shut down." << std::endl ;
    	return -1;
    }

    if (bytes_recieved == -1) {
    	std::cout << "recieve error!" << std::endl ;
    	return -2;
    }

    std::cout << bytes_recieved << " bytes recieved :" << std::endl ;
    incomming_data_buffer[bytes_recieved] = '\0' ;
    std::cout << incomming_data_buffer << std::endl;
    std::cout << "Receiving complete. Closing socket...\n" << std::endl;

    // freeaddrinfo(host_info_list);
    close(socketfd);

    return 0;
}

int Buyer::buy() {
	if(!_sellers.empty())
		std::random_shuffle(_sellers.begin(), _sellers.end());

	int sellerId = _sellers[0];
	std::cout << "randomly pick a seller " << sellerId << std::endl;

    int status;
    struct addrinfo host_info;       // The struct that getaddrinfo() fills up with data.
    struct addrinfo *host_info_list; // Pointer to the to the linked list of host_info's.

    // The MAN page of getaddrinfo() states "All  the other fields in the structure pointed
    // to by hints must contain either 0 or a null pointer, as appropriate." When a struct
    // is created in c++, it will be given a block of memory. This memory is not nessesary
    // empty. Therefor we use the memset function to make sure all fields are NULL.
    memset(&host_info, 0, sizeof host_info);

    host_info.ai_family = AF_UNSPEC;     // IP version not specified. Can be both.
    host_info.ai_socktype = SOCK_STREAM; // Use SOCK_STREAM for TCP or SOCK_DGRAM for UDP.

    // Now fill up the linked list of host_info structs with google's address information.
    status = getaddrinfo(_netIps[sellerId].c_str(), _netPorts[sellerId].c_str(), &host_info, &host_info_list);
    // status = getaddrinfo("127.0.0.1", "5555", &host_info, &host_info_list);
    // getaddrinfo returns 0 on succes, or some other value when an error occured.
    // (translated into human readable text by the gai_gai_strerror function).
    if (status != 0)  {
    	std::cout << "getaddrinfo error" << gai_strerror(status) ;
    	return -1;
    }


    std::cout << "Creating a socket..."  << std::endl;
    int socketfd ; // The socket descripter
    socketfd = socket(host_info_list->ai_family, host_info_list->ai_socktype,
                      host_info_list->ai_protocol);
    if (socketfd == -1) {
    	std::cout << "socket error " ;
    	return -2;
    }


    std::cout << "Connect()ing..."  << std::endl;
    status = connect(socketfd, host_info_list->ai_addr, host_info_list->ai_addrlen);
    if (status == -1)  {
    	std::cout << "connect error" ;
    	return -3;
    }

    std::cout << "send()ing message..."  << std::endl;
    char msg[MAXLEN];
	sprintf(msg, "purchase %d %d", _interestGoods, _peerId);
    int len;
    ssize_t bytes_sent;
    len = strlen(msg);
    bytes_sent = send(socketfd, msg, len, 0);

    std::cout << "Waiting to recieve data..."  << std::endl;
    ssize_t bytes_recieved;
    char incomming_data_buffer[1000];
    bytes_recieved = recv(socketfd, incomming_data_buffer,1000, 0);
    // If no data arrives, the program will just wait here until some data arrives.
    if (bytes_recieved == 0) {
    	std::cout << "host shut down." << std::endl ;
    	return -4;
    }

    if (bytes_recieved == -1) {
    	std::cout << "recieve error!" << std::endl ;
    	return -5;
    }

    std::cout << bytes_recieved << " bytes recieved :" << std::endl ;
    incomming_data_buffer[bytes_recieved] = '\0' ;
    std::cout << incomming_data_buffer << std::endl;
    std::cout << "Receiving complete. Closing socket...\n" << std::endl;

    freeaddrinfo(host_info_list);

    return 0;
}