#include "cs_base.hpp"
#include "service.hpp"

cs_base::cs_base(){
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
