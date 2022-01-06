#include "cs_base.hpp"
#include "service.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>

cs_base::cs_base(){
	/*
	if(!SDL_Init(0)){
		std::cout<<"Failed to initialize SDL with error: "<<SDL_GetError()<<std::endl;
	}
	if(!SDLNet_Init()){
		std::cout<<"Failed to initialize SDL_net with error: "<<SDLNet_GetError()<<std::endl;
	}
	*///doesn't seem neccesary(works fine without it, and both give errors)
	time_t t;
	srand((unsigned)time(&t));
	ID = rand();
}

void cs_base::addService(std::shared_ptr<service> s, std::string name){
	if(ID == 0){//could make this virtual, but this is quicker because the server will always have an ID of 0
		s->isServer = true;
		s->_server = (server*)this;
	}else{
		s->isServer = false;
		s->_client = (client*)this;
	}

	std::string name_Real = name;
	if(name_Real == ""){
		name_Real = s->serviceName;
	}
	services[name_Real] = s;
}

void cs_base::update(){
	
}
