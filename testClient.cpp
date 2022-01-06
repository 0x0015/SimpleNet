#include <iostream>
#include "client.hpp"
#include "service.hpp"

void print(std::string s){
	std::cout<<s<<std::endl;
}

int main(){
	client c;
	c.autoUpdate = true;
	servicePtr serve = service::createService<std::string>("talk", print);
	c.addService(serve);
	c.connect("localhost", 9999);
	while(true){
		std::string message;
		std::getline(std::cin, message);
		serve->send<std::string>(message);
	}
	c.disconnect();

}
