#include "buyer.h"
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <algorithm>
#include <fcntl.h>

int Buyer::Run() {
	// in case not all nodes are setup
	sleep(1);

	pthread_t serverThread;

	pthread_create(&serverThread, NULL, &startBuyerServer, (void*)this);
	std::cout << "server is running.\n";

	int iter = 0;
	while(1) {
		std::cout << "=========== iter " << ++iter << " ==============\n";
        _isPurchaseSuccess = false;
		lookUp();

        sleep(1);

		if(_sellers.size() > 0) {
			buy();
        }

		sleep(std::rand() % MAX_WAIT_TIME);
	}

	pthread_join(serverThread, NULL);

	return 0;
}

int Buyer::lookUp() {
	_sellers.clear();
	_interestGoods = static_cast<Goods>(std::rand() % GOODS_COUNT);

	// check [Goods] #hopCount pathLength pathQueue
    std::string msg = encodeMessage("look_up", _interestGoods, MAX_HOP_COUNT, _peerId);

	floodingMessage(msg);

	return 0;
}

int Buyer::buy() {
	if(!_sellers.empty())
		std::random_shuffle(_sellers.begin(), _sellers.end());

	int sellerId = _sellers[0];
	std::cout << "[purchase] randomly pick a seller " << sellerId << std::endl;

    std::string msg = encodeMessage("purchase", _interestGoods, -1, _peerId);

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
        std::cout << "[buy] getaddrinfo error" << gai_strerror(status) << std::endl;
        return -1;
    }

    int socketfd ; // The socket descripter
    socketfd = socket(host_info_list->ai_family, host_info_list->ai_socktype,
                      host_info_list->ai_protocol);
    if (socketfd == -1) {
        std::cout << "[buy] socket error " << std::endl;
        return -2;
    }


    status = connect(socketfd, host_info_list->ai_addr, host_info_list->ai_addrlen);
    if (status == -1)  {
        std::cout << "[buy] connect error" ;
        return -3;
    }

    int bytes_sent = send(socketfd, msg.c_str(), msg.size(), 0);

    ssize_t bytes_recieved;
    char incomming_data_buffer[1000];
    bytes_recieved = recv(socketfd, incomming_data_buffer,1000, 0);
    // If no data arrives, the program will just wait here until some data arrives.
    if (bytes_recieved == 0) {
        std::cout << "[buy] host shut down." << std::endl ;
        return -4;
    }

    if (bytes_recieved == -1) {
        std::cout << "[buy] recieve error!" << std::endl ;
        return -5;
    }

    incomming_data_buffer[bytes_recieved] = '\0' ;
    std::cout << "[buy] Reply received: " << incomming_data_buffer << std::endl;

    std::string requestType;
    Goods goods;
    int var;
    std::vector<int> path;
    decodeMessage(incomming_data_buffer, requestType, goods, var, path);
    if(requestType == "deal")
        std::cout << "Buyer " << _peerId << " purchase " << goodsNames[_interestGoods] << " from Seller " << sellerId << std::endl;

    // freeaddrinfo(host_info_list);
    close(socketfd);

    return sellerId;
}

void Buyer::processMessage(int rfd) {
    std::cout << "I am a buyer. I am interested in " << goodsNames[_interestGoods] << std::endl;

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

    std::cout << "[Buyer - processMessage] " << buf << std::endl;

    std::string requestType;
    Goods goods;
    int var;
    std::vector<int> path;
    decodeMessage(buf, requestType, goods, var, path);

    pthread_mutex_lock(&_mutex_state);

    if(requestType == "purchase") {
        std::cout << "[Buyer - processMessage] There must be something wrong. A buyer should not receive this purchase request.\n";
    } else if(requestType == "look_up") {
        floodingMessage(buf);
    } else if(requestType == "reply") {
        int sellerPeerId = var;
        if(path.empty()) {
            _sellers.push_back(sellerPeerId);
        }
        else {       
            int lastNbPeerId = path.back(); 
            std::string msg = encodeMessage("reply", goods, sellerPeerId, path.begin(), path.end() - 1);
            sendPeerMessage(lastNbPeerId, msg.c_str());
        }
    } else if(requestType == "deal") {
        _isPurchaseSuccess = true;
    }

   _activeConnect--;

    pthread_mutex_unlock(&_mutex_state);

/*        std::cout << rfd << std::endl;
    std::cout << buflen << std::endl;
    std::cout << buf << std::endl;
    std::cout << "Closing the socket and exist the thread.\n";*/

    // close(rfd);
    pthread_exit(NULL);
}