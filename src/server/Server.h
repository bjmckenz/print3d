#ifndef SERVER_H_SEEN
#define SERVER_H_SEEN

#include <sys/un.h>
#include <string>
#include <vector>
#include "Logger.h"
#include "../drivers/AbstractDriver.h"

class Client;

class Server {
public:
	static const bool FORK_BY_DEFAULT;

	typedef std::vector<Client*> vec_ClientP;

	Server(const std::string& serialPortName, const std::string& socketPath);
	~Server();

	int start(bool fork = FORK_BY_DEFAULT);

	AbstractDriver* getDriver();
	const AbstractDriver* getDriver() const;

private:
	static const int SOCKET_MAX_BACKLOG;

	Server(const Server& o);
	void operator=(const Server& o);

	const std::string socketPath_;

	const Logger& log_;
	int socketFd_;
	AbstractDriver* printerDriver_;

	vec_ClientP clients_;

	bool openSocket();
	bool closeSocket();
	int forkProcess();
	int openPort();

  int driverDelay;
};

#endif /* ! SERVER_H_SEEN */
