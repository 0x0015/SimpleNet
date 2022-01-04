#pragma once
#include <iostream>
#include <memory>
#include <optional>
#include <functional>
#include "server.hpp"
#include "client.hpp"
#include "message.hpp"

uint32_t hash_str(const char* s, int len);
uint32_t hash_str(std::string s);

class service{
public:
	bool isServer = false;
	server* _server = nullptr;
	client* _client = nullptr;
	
	bool autoForward = false;//only effects server

	std::string serviceName;
	template<class T> void send(T& data, std::optional<std::vector<unsigned int>> toU = std::nullopt){
		if(isServer && _server){
			if(!toU){
				std::cout<<"You must send the message to 1 or more users"<<std::endl;
			}else{
				std::vector<unsigned int> toUsers = toU.value();
				std::shared_ptr<message> tempM = std::make_shared<message>(true);
				tempM->setData<T>(data);
				tempM->serviceId = hash_str(serviceName);
				if(toUsers.size() > 0){
					for(int i=0;i<toUsers.size();i++){
						if(toUsers[i] < _server->clients.size()){
							_server->clients[toUsers[i]]->sendMessage(tempM);
						}else{
							std::cout<<"unknown client number: "<<toUsers[i]<<" of "<<_server->clients.size()<<" connected clients."<<std::endl;
						}
					}
				}else{
					for(int i=0;i<_server->clients.size();i++){
						_server->clients[i]->sendMessage(tempM);
					}
				}
			}
		}else if(_client){
			std::shared_ptr<message> tempM = std::make_shared<message>(true);
			tempM->setData<T>(data);
			tempM->serviceId = hash_str(serviceName);
			_client->sendMessage(tempM);
		}else{
			std::cout<<"Cannot send to service not associated with a client or server."<<std::endl;
			return;
		}
	}
	std::function<void(std::shared_ptr<message>)> getFunc;//this is called with the data from the incoming message
	template<class T> void setServerGet(std::function<void(T)> onGet){
		setServerGet(onGet, identity<T>());
	}
	service();
	template<class T> static std::shared_ptr<service> createService(std::string name, std::optional<std::function<void(T)>> onGet = std::nullopt){
		std::shared_ptr<service> output = std::make_shared<service>();
		output->serviceName = name;
		if(onGet){
			output->setServerGet<T>(onGet.value());
		}
		return(output);
	}
	static std::shared_ptr<service> createForwardService(std::string name);
	private:
	template<class T> void setServerGet(std::function<void(T)> onGet, identity<T>){
		getFunc = [onGet](std::shared_ptr<message> vpoint){//onGet MUST be passed by value, not reference as it will be taken out of context.
			onGet(vpoint->getData<T>());
		};
	}
	void setServerGet(std::function<void(std::shared_ptr<message>)> onGet, identity<std::function<void(std::shared_ptr<message>)>>);


};
