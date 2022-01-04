#include "client.hpp"
#include "message.hpp"
#include "service.hpp"

bool client::resolveHost(std::string ipAddr, unsigned int port){
	if(SDLNet_ResolveHost(&ip, ipAddr.c_str(), port) == -1){
		std::cout<<"Error resolving host: "<<SDLNet_GetError()<<std::endl;
		return(false);
	}
	return(true);
}

bool client::openPort(){
	tcpsock = SDLNet_TCP_Open(&ip);
	if(!tcpsock){
		std::cout<<"Error opening host: "<<SDLNet_GetError()<<std::endl;
		return(false);
	}
	open = true;
	return(true);
}

bool client::connectToServer(std::string ip, unsigned int port){//safely resolves host, and opens
	if(resolveHost(ip, port)){
		if(openPort()){
			return(true);
		}
	}
	return(false);
}

void client::sendRaw(const char* data, unsigned int l){
	unsigned int len = l;
	if(len == 0){
		len = strlen(data);
	}
	std::shared_ptr<message> tempM = std::make_shared<message>();
	tempM->setData(data, l);
	sendMessage(tempM);
}

void client::sendString(std::string s){
	std::shared_ptr<message> tempM = std::make_shared<message>();
	tempM->setData(s);
	sendMessage(tempM);
}

void client::sendMessage(std::shared_ptr<message> m){
	if(!m->senderId){
		m->senderId = ID;
	}
	oM_mutex.lock();
	outgoingMessages.push_back(m);
	oM_mutex.unlock();
	newMessages.release();
}

void client::connect(std::string ip, unsigned int port){
	workingThread = std::thread(&client::threadFunc, this, ip, port);
}

void client::disconnect(){
	open = false;
	newMessages.release();
	workingThread.join();
}

void client::threadFunc(std::string ip, unsigned int port){
	if(!connectToServer(ip, port)){
		return;
	}
	
	if(autoUpdate){
		autoUpdateThread = std::thread(&client::autoUpdateFunc, this);
	}
	writeThread = std::thread(&client::writeThreadFunc, this);
	readThread = std::thread(&client::readThreadFunc, this);
	writeThread.join();
	readThread.join();

	SDLNet_TCP_Close(tcpsock);
}

void client::writeThreadFunc(){
	while(open){
		newMessages.acquire();
		oM_mutex.lock();
		for(int i=0;i<outgoingMessages.size();i++){
			std::vector<char> outgoingData = outgoingMessages[i]->toBuffer();
			int result = SDLNet_TCP_Send(tcpsock, outgoingData.data(), outgoingData.size());
			if(result < outgoingData.size()){
				std::cout<<"Failed to send message: "<<SDLNet_GetError()<<std::endl;
			}
		}
		outgoingMessages.clear();
		oM_mutex.unlock();
	}
}

void client::readThreadFunc(){
	while(open){
		char buffer[1024];
		int len = SDLNet_TCP_Recv(tcpsock, buffer, 1024);
		if(!len){
			std::cout<<"Failed to get length of message"<<std::endl;
			continue;
		}
		//std::cout<<"Received message len: "<<len<<std::endl;
		//for(int i=0;i<len;i++){
		//	std::cout<<buffer[i];
		//}
		//std::cout<<std::endl;

		std::vector<char> bufferVec(buffer, buffer+len);
		std::shared_ptr<message> incomingM = message::fromBuffer(bufferVec);
		iM_mutex.lock();
		incomingMessages.push_back(incomingM);
		iM_mutex.unlock();
	}
}

void client::update(){
	iM_mutex.lock();
	for(int i=0;i<incomingMessages.size();i++){
		bool foundCorrectService = false;
		service* foundService = nullptr;
		for(auto& j : services){
			uint32_t nameHash = hash_str(j.first);
			if(incomingMessages[i]->serviceId == nameHash){
				foundCorrectService = true;
				foundService = j.second.get();
			}
		}
		if(!foundCorrectService){
			continue;
		}
		if(foundService->getFunc){
			//void* incomingData = incomingMessages[i]->getData<std::pair<void*, unsigned int>>().first;
			foundService->getFunc(incomingMessages[i]);
		}

	}
	incomingMessages.clear();
	iM_mutex.unlock();
}

void client::autoUpdateFunc(){
	while(open){
		update();
		SDL_Delay(100);
	}
}

client::client(){

}

client::~client(){
	if(open){
		disconnect();
	}
}
