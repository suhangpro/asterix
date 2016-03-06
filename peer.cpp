#include "peer.h"
#include <fstream>

#include <iostream>
#include <cstring> 	// used for memset.
#include <vector>
#include <list>
#include <iterator>
#include <sstream>

int Peer::readNetworkFile(const char *netFileName) {
	std::ifstream in(netFileName);
	if( !in ) {
		std::cerr << "Error in reading network file " << netFileName << std::endl;
		return -1;
	}

	int numPeers;
	in >> numPeers;

	for(int i = 0; i < numPeers; ++i )
	{
		int peerId = -1;
		in >> peerId;

		std::string ip, port;
		in >> ip;
		in >> port;

		_netIps.push_back(ip);
		_netPorts.push_back(port);

		if(_peerId == peerId) {
			_port = port;
			_ip = ip;
		}

		int numNbPeers = 0;
		in >> numNbPeers;
		for(int n = 0; n < numNbPeers; ++n) {
			int nbPeerId = -1;
			in >> nbPeerId;

			if(_peerId == peerId)
				_nbPeerIds.push_back(nbPeerId);
		}
	}

	for(size_t n = 0; n < _nbPeerIds.size(); ++n) {
		int nbPeerId = _nbPeerIds[n];
		_nbIps.push_back(_netIps[nbPeerId]);
		_nbPorts.push_back(_netPorts[nbPeerId]);
	}

	return 0;
}

int Peer::setUpServer() {
	struct addrinfo hostinfo, *res;

	int sock_fd;

	int server_fd; // the fd the server listens on
	int ret;
	int yes = 1;

	// first, load up address structs with getaddrinfo():

	memset(&hostinfo, 0, sizeof(hostinfo));

	hostinfo.ai_family = AF_UNSPEC;  // use IPv4 or IPv6, whichever
	hostinfo.ai_socktype = SOCK_STREAM;
	hostinfo.ai_flags = AI_PASSIVE;     // fill in my IP for me

	getaddrinfo(_ip.c_str(), _port.c_str(), &hostinfo, &res);


    server_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    //if(server_fd < 0) throw some error;

    //prevent "Error Address already in use"
    ret = setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
    // if(ret < 0) throw some error;

    ret = bind(server_fd, res->ai_addr, res->ai_addrlen);

    if(ret != 0)
    {
        std::cout << "error :" << strerror(errno) << std::endl;
        return -1 ;
    }

    ret = listen(server_fd, BACKLOG);
    //if(ret < 0) throw some error;

	return server_fd;
}

// This function will establish a connection between the server and the
// client. It will be executed for every new client that connects to the server.
// This functions returns the socket filedescriptor for reading the clients data
// or an error if it failed.
int Peer::establishServerConnection() {
    char ipstr[INET6_ADDRSTRLEN];
    int port;

    int new_sd;
    struct sockaddr_storage remote_info ;
    socklen_t addr_size;

    addr_size = sizeof(addr_size);
    new_sd = accept(_server_fd, (struct sockaddr *) &remote_info, &addr_size);
    //if (fd < 0) throw some error here;

    getpeername(new_sd, (struct sockaddr*)&remote_info, &addr_size);

   // deal with both IPv4 and IPv6:
	if (remote_info.ss_family == AF_INET) {
	    struct sockaddr_in *s = (struct sockaddr_in *)&remote_info;
	    port = ntohs(s->sin_port);
	    inet_ntop(AF_INET, &s->sin_addr, ipstr, sizeof ipstr);
	} else { // AF_INET6
	    struct sockaddr_in6 *s = (struct sockaddr_in6 *)&remote_info;
	    port = ntohs(s->sin6_port);
	    inet_ntop(AF_INET6, &s->sin6_addr, ipstr, sizeof ipstr);
	}

	std::cout << "Connection accepted from "  << ipstr <<  " using port " << port << std::endl;

    return new_sd;
}

// This function will send data to the clients fd.
// data contains the message to be send
int Peer::sendServerMessage(int fd, const std::string &data) {
    int ret;

    ret = send(fd, data.c_str(), data.length(), 0);
    //if(ret != strlen(data.c_str()) throw some error;
    return 0;
}

// This function will start to process a server to process purchase request in a multi-thread manner
int Peer::startServer() {
	std::string welcome_msg("Welcome to this telnet chess server.\n");

    pthread_t threads[MAXFD]; //create 10 handles for threads.

    FD_ZERO(&_the_state); // FD_ZERO clears all the filedescriptors in the file descriptor set fds.

    _activeConnect = 0;

    Message *pMsg = new Message;

    while(1) // start looping here
    {
        int rfd;
        void *arg;

        // if a client is trying to connect, establish the connection and create a fd for the client.
        rfd = establishServerConnection();

        if (rfd >= 0)
        {
            std::cout << "Client connected. Using file desciptor " << rfd << std::endl;
            if (_activeConnect > MAXFD)
            {
                std::cout << "To many clients trying to connect." << std::endl;
                close(rfd);
                continue;
            }

            pthread_mutex_lock(&_mutex_state);  // Make sure no 2 threads can create a fd simultanious.

            FD_SET(rfd, &_the_state);  // Add a file descriptor to the FD-set.

            _activeConnect++;

            pthread_mutex_unlock(&_mutex_state); // End the mutex lock

            // std::cout << "Run, rfd: " << rfd << std::endl;
            pMsg->p = this;
        	pMsg->rfd = rfd;
        	arg = (void*)(pMsg);
/*        	std::cout << "Run, pMsg: " << pMsg << std::endl;
        	std::cout << "Run, arg: " << arg << std::endl;*/
            // arg = (void *)(&rfd);

            // sendServerMessage(rfd, welcome_msg); // send a welcome message/instructions.

            // now create a thread for this client to intercept all incomming data from it.
            pthread_create(&threads[rfd], NULL, readTcpServer, arg);
        }
    }

    delete pMsg;
}