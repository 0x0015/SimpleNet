#include "server.hpp"
#include "service.hpp"

void print(std::string s){
	std::cout<<s<<std::endl;
}

int main(){
	server s;
	s.autoUpdate = true;
	servicePtr serv = service::createService<std::string>("talk", print);
	s.addService(serv);
	s.start(9999);
	while(true){
		std::string m;
		std::getline(std::cin, m);
		std::vector<unsigned int> toWhom;
		serv->send<std::string>(m, toWhom);
	}
	s.stop();
}

