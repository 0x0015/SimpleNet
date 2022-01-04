#pragma once
#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <vector>
#include <memory>
#include <thread>
#include <mutex>
#include <semaphore>
#include "cs_base.hpp"

class message;
class server;

class connection{
public:
	TCPsocket client;
	IPaddress* clientip;
	server* host;
	std::vector<std::shared_ptr<message>> outgoingMessages;
	std::mutex oM_mutex;
	std::binary_semaphore newMessages{0};
	std::vector<std::shared_ptr<message>> incomingMessages;
	std::mutex iM_mutex;
	std::thread readThread;
	std::thread writeThread;
	void sendMessage(std::shared_ptr<message> m);
	bool connected = false;
	void readThreadFunc();
	void writeThreadFunc();
	void open();
	void disconnect();
	connection();
	~connection();
};

class server : public cs_base{
public:
	void update();
	bool autoUpdate = false;
	void start(unsigned int port);
	void stop();
	int numOfConnections();
	void sendMessage(unsigned int connectionNumber, std::shared_ptr<message> m);
	std::vector<std::shared_ptr<connection>> clients;
	server();
	~server();
private:
	TCPsocket serverSock;
	std::mutex C_mutex;
	IPaddress ip;
	bool open = false;
	bool resolveHost(unsigned int port);
	bool openPort();
	bool startHost(unsigned int port);
	std::thread workingThread;
	void threadFunc(unsigned int port);
	std::thread autoUpdateThread;
	void autoUpdateFunc();
};
