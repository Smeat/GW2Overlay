#include "Config.h"

#include <algorithm>
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>

Config::Config(const std::string& name, const std::string& file) {
	this->m_config_file = file;
	this->m_name = name;
	std::cout << "String constructor with name " << name << std::endl;
}

Config::~Config() { std::cout << "Destroying " << m_name << std::endl; }

void Config::save_config(std::string filename) {
	if (filename.empty()) filename = this->m_config_file;
	json j;
	std::cout << "Saving config to " << filename << std::endl;
	std::function<void(json&, const Config&)> get_all_children;
	get_all_children = [&](json& curr_j, const Config& entry) {
		std::cout << "Processing " << entry.m_name << " with value size " << entry.m_values.size() << " and children "
				  << entry.m_children.size() << std::endl;
		// add values
		if (entry.m_values.size() > 1) {
			curr_j[entry.m_name] = entry.m_values;
		} else if (entry.m_values.size() == 1) {
			curr_j[entry.m_name] = entry.m_values[0];
			std::cout << "Adding val to json " << curr_j[entry.m_name] << std::endl;
		}

		// add children
		for (const auto& child : entry.m_children) {
			get_all_children(curr_j[entry.m_name], *child.second);
		}
	};
	get_all_children(j, *this);

	std::ofstream of(filename);
	if (of) {
		of << j.dump();
	}
	this->m_config_file = filename;
}

json Config::to_json() {}

void Config::from_json(const json& j, bool clear) {
	if (clear) {
		this->m_values.clear();
		this->m_children.clear();
	}
	std::cout << "#####Parsing: " << j.dump() << std::endl;
	for (auto iter = j.begin(); iter != j.end(); ++iter) {
		if (iter->is_object()) {
			json element = iter->get<json::object_t>();
			std::shared_ptr<Config> child(new Config(iter.key()));
			child->from_json(element);
			std::cout << "Created new child from " << element << " name " << child->m_name << std::endl;
			this->m_children[child->m_name] = child;
			//	this->m_children.insert({child->m_name, child});
			std::cout << "Adding " << iter.key() << ":" << iter.value() << " to " << m_name << " total children "
					  << m_children.size() << std::endl;
		} else if (iter->is_string()) {
			std::cout << "string key " << iter.key() << " val " << iter.value() << std::endl;
			this->set_item(iter.key(), iter.value());
		} else if (iter->is_array()) {
			std::cout << "Array!" << std::endl;
			this->set_items(iter.key(), iter.value());
		}
	}
}

void Config::load_config(std::string filename, bool clear) {
	if (filename.empty()) filename = this->m_config_file;
	std::ifstream input(filename);
	if (input) {
		json j;
		input >> j;
		this->from_json(j, clear);
	}
	this->m_config_file = filename;
}

void Config::set_item(const std::string& item, const std::string& value) {
	bool changed = false;
	auto& child = this->get_entry(item);
	if (child.get_item() != value) {
		changed = true;
	}
	child.m_values.clear();
	child.m_values.push_back(value);
	// TODO: notify root to save new config
	this->save_config();
}
void Config::set_items(const std::string& item, const std::vector<std::string>& values) {
	bool changed = false;
	auto& child = this->get_entry(item);
	child.m_values = values;
}

std::string Config::get_item() {
	std::cout << "Getting item with size " << m_values.size() << " from " << m_name << std::endl;
	if (m_values.size() == 1) {
		return m_values[0];
	}
	return "";
}

void Config::add_callback(const std::string& item, config_changed_cb cb) {
	this->m_notify_callbacks[item].push_back(cb);
}

void Config::notify() {
	/*
	auto cbs = this->m_notify_callbacks.find(item);
	if (cbs != this->m_notify_callbacks.end()) {
		for (const auto& cb : cbs->second) {
			cb(item);
		}
	}
	*/
}

void Config::notify_all() {
	/*
	for (const auto& item : this->m_notify_callbacks) {
		this->notify(item.first);
	}
	*/
}

ConfigManager::ConfigManager() {
	this->m_root_config.reset(new Config("root"));
	for (auto iter = DEFAULT_CONFIGS.begin(); iter != DEFAULT_CONFIGS.end(); ++iter) {
		std::shared_ptr<Config> conf(new Config);
		conf->load_config(*iter, false);
		// TODO: allow multiple categeories per file. With the current implementation the config would get overwritten
		// We hope this loop only has a single iteration ;)
		for (const auto& c : conf->m_children) {
			c.second->m_config_file = *iter;
		}
		this->m_root_config->m_children.insert(conf->m_children.begin(), conf->m_children.end());
	}
}

Config& ConfigManager::get_config(const std::string& conf) {
	return this->m_root_config->get_entry(conf);
	/*
	auto res = this->m_root_config->m_children.find(conf);
	if (res != this->m_root_config->m_children.end()) {
		return res->second;
	} else {
		std::cout << "Couldn't find " << conf << std::endl << "Currently available: \n";
		for (auto iter = this->m_root_config->m_children.begin(); iter != this->m_root_config->m_children.end(); ++iter)
	{ std::cout << iter->first << std::endl;
		}
		std::shared_ptr<Config> new_conf(new Config(conf, ""));
		this->m_configs.insert({conf, new_conf});
		return new_conf;
	}
	*/
}
