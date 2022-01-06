#include "service.hpp"

#define A 54059 /* a prime */
#define B 76963 /* another prime */
#define C 86969 /* yet another prime */
#define FIRSTH 37 /* also prime */
uint32_t hash_str(const char* s, int len)//a static hash is required between the client and server
{
   uint32_t h = FIRSTH;
   int i=0;
   while (*s && i<len) {
     h = (h * A) ^ (s[0] * B);
     s++;
     i++;
   }
   return h; // or return h % C;
}

uint32_t hash_str(std::string s){
	return(hash_str(s.c_str(), s.length()));
}


void service::setServerGet(std::function<void(std::shared_ptr<message>)> onGet, identity<std::function<void(std::shared_ptr<message>)>>){
	getFunc = onGet;
}

static std::shared_ptr<service> createForwardService(std::string name){
	std::shared_ptr<service> output = std::make_shared<service>();
	output->serviceName = name;
	return(output);
}

service::service(){

}

