#include "server.hpp"
#include "message.hpp"
#include "service.hpp"

void server::start(unsigned int port){
	workingThread = std::thread(&server::threadFunc, this, port);
}

void server::stop(){
	open = false;
	clients.clear();
	workingThread.join();
}

void server::update(){
	C_mutex.lock();
	for(int y=0;y<clients.size();y++){
		clients[y]->iM_mutex.lock();
		for(int i=0;i<clients[y]->incomingMessages.size();i++){
			bool foundCorrectService = false;
			service* foundService = nullptr;
			for(auto& j : services){
				uint32_t nameHash = hash_str(j.first);
				//std::cout<<"Service hash: "<<nameHash<<std::endl;
				//std::cout<<"Found hashe: "<<clients[i]->incomingMessages[i]->serviceId<<std::endl;
				if(clients[y]->incomingMessages[i]->serviceId == nameHash){
					foundCorrectService = true;
					foundService = j.second.get();
				}
			}
			if(!foundCorrectService){
				continue;
			}
			if(foundService->getFunc){
				//void* incomingData = incomingMessages[i]->getData<std::pair<void*, unsigned int>>().first;
				foundService->getFunc(clients[y]->incomingMessages[i]);
			}
			if(foundService->autoForward){
				for(int x=0;x<clients.size();x++){
					if(i != x){
						clients[x]->sendMessage(clients[y]->incomingMessages[i]);
					}
				}
			}
	
		}
		clients[y]->incomingMessages.clear();
		clients[y]->iM_mutex.unlock();
	}
	C_mutex.unlock();
}

void server::autoUpdateFunc(){
	while(open){
		update();
		SDL_Delay(100);
	}
}

bool server::resolveHost(unsigned int port){
	if(SDLNet_ResolveHost(&ip, NULL, port) == -1){
		std::cout<<"Error resolving host: "<<SDLNet_GetError()<<std::endl;
		return(false);
	}
	return(true);
}

bool server::openPort(){
	serverSock = SDLNet_TCP_Open(&ip);
	if(!serverSock){
		std::cout<<"Error opening host: "<<SDLNet_GetError()<<std::endl;
		return(false);
	}
	open = true;
	return(true);
}

bool server::startHost(unsigned int port){
	if(resolveHost(port)){
		if(openPort()){
			return(true);
		}
	}
	return(false);
}

void server::threadFunc(unsigned int port){
	if(!startHost(port)){
		return;
	}
	if(autoUpdate){
		autoUpdateThread = std::thread(&server::autoUpdateFunc, this);
	}
	while(open){
		TCPsocket client = SDLNet_TCP_Accept(serverSock);
		if(!client){//wait until a client connects
			C_mutex.lock();
			for(int i=0;i<clients.size();i++){
				if(clients[i]->connected == false){
					clients.erase(clients.begin()+i);
					i--;
				}
			}
			C_mutex.unlock();
			SDL_Delay(100);
			continue;
		}

		IPaddress* remoteIp;
		remoteIp = SDLNet_TCP_GetPeerAddress(client);
		if(!remoteIp){
			std::cout<<"Failed to get remote IP address: "<<SDLNet_GetError()<<std::endl;
			continue;
		}
		//found a connection
		std::shared_ptr<connection> clientConnection = std::make_shared<connection>();
		clientConnection->client = client;
		clientConnection->clientip = remoteIp;
		clientConnection->host = this;
		clientConnection->connected = true;
		C_mutex.lock();
		clients.push_back(clientConnection);
		C_mutex.unlock();
		clientConnection->open();
		
	}
}

int server::numOfConnections(){
	C_mutex.lock();
	return(clients.size());
	C_mutex.unlock();
}

void server::sendMessage(unsigned int connectionNum, std::shared_ptr<message> m){
	C_mutex.lock();
	if(clients.size() > connectionNum){
		clients[connectionNum]->sendMessage(m);
	}
	C_mutex.unlock();
}

server::server(){
	ID = 0;//if the ID is 0, than all the clients can know when messages are coming from the server
}

server::~server(){
	if(open){
		stop();
	}
}

void connection::open(){
	uint32_t ipaddr;
	ipaddr = SDL_SwapBE32(clientip->host);
	printf("Accepted a connection from %d.%d.%d.%d port %hu\n", ipaddr >> 24,
             (ipaddr >> 16) & 0xff, (ipaddr >> 8) & 0xff, ipaddr & 0xff,
             clientip->port);//bit shift magic
	readThread = std::thread(&connection::readThreadFunc, this);
	writeThread = std::thread(&connection::writeThreadFunc, this);
}

void connection::readThreadFunc(){
	while(connected){
		char buffer[1024];//should be enough.  anything longer should be split up
		int len = SDLNet_TCP_Recv(client, buffer, 1024);
		if(!len){
			std::cout<<"Failed to get length of message: "<<SDLNet_GetError()<<std::endl;
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

void connection::writeThreadFunc(){
	while(connected){
		newMessages.acquire();
		oM_mutex.lock();
		for(int i=0;i<outgoingMessages.size();i++){//change to get all the buffers than send if the list of messages is going to get too big.
			std::vector<char> outgoingData = outgoingMessages[i]->toBuffer();
			int result = SDLNet_TCP_Send(client, outgoingData.data(), outgoingData.size());
			if(result < outgoingData.size()){
				std::cout<<"Failed to send message: "<<SDLNet_GetError()<<std::endl;
			}
		}
		outgoingMessages.clear();
		oM_mutex.unlock();
	}
}

void connection::disconnect(){
	SDLNet_TCP_Close(client);
	connected = false;
	readThread.join();
	newMessages.release();
	writeThread.join();
}

void connection::sendMessage(std::shared_ptr<message> m){
	if(!m->senderId){
		m->senderId = host->ID;
	}
	oM_mutex.lock();
	outgoingMessages.push_back(m);
	oM_mutex.unlock();
	newMessages.release();
}

connection::connection(){

}

connection::~connection(){
	if(connected){
		disconnect();
	}
}
