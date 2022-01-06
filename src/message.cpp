#include "message.hpp"
#include <cstring>
#include <stddef.h>
#include <sys/time.h>

void message::setData(const char* newData, unsigned int l){
	unsigned int len = l;
	if(len == 0){
		len = strlen(newData);
	}
	data.clear();
	data.resize(len);
	std::memcpy(((char*)data.data()), newData, l);
}

void message::setData(std::vector<char> newData, identity<std::vector<char>>){
	data = newData;//that was easy
}

void message::setData(std::string newData, identity<std::string>){
	unsigned int strlen = newData.length();
	setData(newData.c_str(), strlen);
}

message::message(){

}

std::vector<char> message::getData(identity<std::vector<char>>){
	return(data);
}

std::string message::getData(identity<std::string>){
	std::string output;
	for(int i=0;i<data.size();i++){//maybe not the fastest, but fairly safe.
		output += data[i];
	}
	return(output);
}

std::pair<void*,unsigned int> message::getData(identity<std::pair<void*, unsigned int>>){
	return(std::pair<void*, unsigned int>((void*)data.data(), data.size()));
}

unsigned int getMillis(){
	struct timeval current_time;
	gettimeofday(&current_time, NULL);
	return((current_time.tv_usec + (current_time.tv_sec * 1000000))/1000);
}

message::message(bool updateTime){
	if(updateTime){
		millis = getMillis();
	}
}

std::vector<char> message::toBuffer(){
	//std::cout<<"To buffer.  millis: "<<millis<<", serviceId: "<<serviceId<<", senderId: "<<senderId<<std::endl;
	unsigned int outputSize = sizeof(serviceId) + sizeof(millis) + sizeof(senderId) + data.size();

	std::vector<char> output;

	output.resize(outputSize);
	*((uint64_t*)output.data()) = millis;
	*((uint32_t*)(output.data() + sizeof(millis))) = serviceId;
	*((uint16_t*)(output.data()+sizeof(millis)+sizeof(serviceId))) = senderId;
	char* cbegin = output.data() + sizeof(millis) + sizeof(serviceId) + sizeof(senderId);
	std::memcpy(cbegin, data.data(), output.size()-sizeof(millis)-sizeof(serviceId)-sizeof(senderId));
	return(output);
}

std::shared_ptr<message> message::fromBuffer(std::vector<char> buffer){
	std::shared_ptr<message> output = std::make_shared<message>(false);

	unsigned int dataSize = buffer.size() - sizeof(millis) - sizeof(serviceId) - sizeof(senderId);
	output->data.resize(dataSize);
	output->millis = *((uint64_t*)buffer.data());
	output->serviceId = *((uint32_t*)(buffer.data()+sizeof(output->millis)));
	output->senderId = *((uint16_t*)(buffer.data()+sizeof(output->millis)+sizeof(output->serviceId)));
	char* cbegin = buffer.data() + sizeof(output->millis) + sizeof(output->serviceId) + sizeof(output->senderId);
	std::memcpy(output->data.data(), cbegin, dataSize);
	
	//std::cout<<"From buffer.  millis: "<<output->millis<<", serviceId: "<<output->serviceId<<", senderId: "<<output->senderId<<std::endl;

	return(output);
}
