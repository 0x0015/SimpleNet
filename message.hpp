#pragma once
#include <iostream>
#include <vector>
#include <memory>

template<typename T> struct identity{
	typedef T type;
};

class message{
public:
	uint32_t serviceId;
	uint16_t senderId;
	uint64_t millis;
	std::vector<char> data;
	template<class T> void setData(T& newData){
		return(setData(newData, identity<T>()));
	}
private:
	template<class T> void setData(T& newData, identity<T>){
		data.clear();
		int tSize = sizeof(newData);
		data.resize(tSize);
		*((T*)data.data()) = newData;
	}
private:
	void setData(std::string newData, identity<std::string>);
	void setData(std::vector<char>, identity<std::vector<char>>);
public:
	void setData(const char* newData, unsigned int len);
	template<class T> T getData(){
		return(getData(identity<T>()));
	}
private:
	template<class T> T getData(identity<T>){
		return(*((T*)data.data()));
	}
	std::vector<char> getData(identity<std::vector<char>>);
	std::string getData(identity<std::string>);
	std::pair<void*,unsigned int> getData(identity<std::pair<void*,unsigned int>>);
public:
	std::vector<char> toBuffer();
	message();
	message(bool updateTime);
	static std::shared_ptr<message> fromBuffer(std::vector<char> buffer);
};
