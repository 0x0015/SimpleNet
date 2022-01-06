#pragma once
#include <iostream>
#include <vector>
#include <memory>
#include <unordered_map>

class service;

class cs_base{
public:
	uint16_t ID;
	std::unordered_map<std::string, std::shared_ptr<service>> services;
	void addService(std::shared_ptr<service> s, std::string name = "");
	virtual void update();
	cs_base();
};
