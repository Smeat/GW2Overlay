#include "Config.h"

#include "json/json.hpp"

#include <fstream>
#include <iostream>

using json = nlohmann::json;

Config::Config(const std::string& file) { this->m_config_file = file; }

void Config::save_config(std::string filename) {
	if (filename.empty()) filename = this->m_config_file;
	json j;
	for (auto iter = this->m_config_items.begin(); iter != this->m_config_items.end(); ++iter) {
		j[iter->first] = iter->second;
	}

	std::ofstream of(filename);
	if (of) {
		of << j.dump();
	}
	this->m_config_file = filename;
}

void Config::load_config(std::string filename) {
	if (filename.empty()) filename = this->m_config_file;
	std::ifstream input(filename);
	if (input) {
		json j;
		input >> j;
		this->m_config_items.clear();
		for (auto iter = j.begin(); iter != j.end(); ++iter) {
			this->m_config_items[iter.key()] = iter.value();
		}
	}
	this->m_config_file = filename;
}

void Config::set_item(const std::string& item, const std::string& value) {
	bool changed = false;
	if (this->get_item(item) != value) {
		changed = true;
		std::cout << "Setting " << item << " to " << value << std::endl;
	}
	this->m_config_items[item] = value;

	if (changed) {
		this->notify(item);
		this->save_config();
	}
}

std::string Config::get_item(const std::string& item) {
	auto res = this->m_config_items.find(item);
	return (res != this->m_config_items.end()) ? res->second : "";
}

void Config::add_callback(const std::string& item, config_changed_cb cb) {
	this->m_notify_callbacks[item].push_back(cb);
}

void Config::notify(const std::string& item) {
	auto cbs = this->m_notify_callbacks.find(item);
	if (cbs != this->m_notify_callbacks.end()) {
		for (const auto& cb : cbs->second) {
			cb(item);
		}
	}
}

void Config::notify_all() {
	for (const auto& item : this->m_notify_callbacks) {
		this->notify(item.first);
	}
}
