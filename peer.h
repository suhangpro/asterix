#ifndef _PEER_H_
#define _PEER_H_

#include <cstdlib>
#include <string>
#include <vector>
#include <iostream>
#include <arpa/inet.h> 	// for inet_ntop function
#include <netdb.h>
#include <sys/socket.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>

const int MAXLEN = 1024 ;   // Max length of a message.
const int MAXFD = 7 ;       // Maximum file descriptors to use. Equals maximum clients.

enum Goods{Fisher = 0, Salt, Boars, GOODS_COUNT};

void *readTcpServer(void *arg);

class Peer{
public:
	Peer( const std::string &ip, const std::string &port, int peerId)
		: _ip(ip), _port(port)
	{
		_peerId = peerId;
	}

	Peer(int peerId, const char *networkFileName) {
		_peerId = peerId;

		_mutex_state = PTHREAD_MUTEX_INITIALIZER;
		BACKLOG = 5;

		readNetworkFile(networkFileName);

		_server_fd = setUpServer();

		if(_server_fd < 0) {
			std::cerr << "Error in starting the peer " << _peerId << ". Terminating the process.\n";
			return;
		}

/*		std::cout << _peerId << std::endl;
		std::cout << _ip << std::endl;
		std::cout << _port << std::endl;*/
		// setUpClient();
	}

	int Run();

	int lookUp();

	virtual void processMessage(int rfd) = 0;

protected:
	int readNetworkFile(const char *netFileName);

	int setUpServer();

	int establishServerConnection();

	int sendServerMessage(int fd, const std::string &data);

	double randomSample() {
		return static_cast<double>(std::rand()) / static_cast<double>(RAND_MAX);
	}

protected:
	std::string _ip;
	std::string _port;
	int _peerId;

	std::vector<std::string> _nbIps;
	std::vector<std::string> _nbPorts;
	std::vector<int> _nbPeerIds;

	// socket vars
	volatile fd_set _the_state;
	pthread_mutex_t _mutex_state;
	int _server_fd;
	int BACKLOG;     // Number of connections that can wait in que before they be accept()ted
};

struct Message {
	Peer *p;
	int rfd;
};

inline void *readTcpServer(void *arg) {
	std::cout << "arg: " << arg << std::endl;
	Message *msg = (Message*)arg;
	std::cout << "rfd: " << msg->rfd << std::endl;
	std::cout << "peer: " << msg->p << std::endl;
	int rfd = msg->rfd;
	std::cout << "start processMessage\n";
	msg->p->processMessage(rfd);
}

#endif