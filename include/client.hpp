#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <iostream>
#include <vector>
#include <memory>
#include <thread>
#include <mutex>
#include <semaphore>
#include "cs_base.hpp"

class message;

class client : public cs_base{
public:
	void update();
	unsigned int errorsBeforeDisconnect = 10;
	unsigned int autoUpdateTime = 100;
	bool autoUpdate = false;
	void sendRaw(const char* data, unsigned int len = 0);
	void sendMessage(std::shared_ptr<message> m);
	void sendString(std::string s);
	void connect(std::string ip, unsigned int port);
	void disconnect();
	client();
	~client();
private:
	unsigned int disconnectErrors = 0;
	IPaddress ip;
	TCPsocket tcpsock;
	std::vector<std::shared_ptr<message>> outgoingMessages;
	std::mutex oM_mutex;
	std::binary_semaphore newMessages{0};//wierd syntax, but ok
	std::vector<std::shared_ptr<message>> incomingMessages;
	std::mutex iM_mutex;
	bool open = false;
	bool resolveHost(std::string ip, unsigned int port);
	bool openPort();
	bool connectToServer(std::string ip, unsigned int port);
	std::thread workingThread;
	void threadFunc(std::string ip, unsigned int port);
	std::thread writeThread;
	std::thread readThread;
	void writeThreadFunc();
	void readThreadFunc();
	std::thread autoUpdateThread;
	void autoUpdateFunc();
	std::thread errorDisconnectThread;
};
