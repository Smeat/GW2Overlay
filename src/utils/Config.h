#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "json/json.hpp"
using json = nlohmann::json;

typedef std::function<void(const std::string&)> config_changed_cb;

const std::unordered_map<std::string, std::string> DEFAULT_CONFIG_SETTINGS = {
	{"USE_KEY", "f"}, {"ICON_POS_X", "1580"}, {"ICON_POS_Y", "0"}, {"ICON_SCALE", "1.0"}};

const std::vector<std::string> DEFAULT_CONFIGS = {"./settings.json", "./builds.json"};

class Config {
 public:
	Config(const std::string& name, const std::string& file = "");
	Config() = default;
	~Config();
	void load_config(std::string file = "", bool clear = true);
	void save_config(std::string file = "");
	json to_json();

	void from_json(const json& j, bool clear = true);

	void set_item(const std::string& item, const std::string& value);
	void add_item(const std::string& item, const std::string& value);
	void set_items(const std::string& item, const std::vector<std::string>& values);
	const std::vector<std::string>& get_items() const { return this->m_values; };

	void add_callback(const std::string& item, config_changed_cb cb);
	void remove_callback(const std::string& item, config_changed_cb cb);

	Config& operator[](const std::string& val) { return this->get_entry(val); }

	Config& get_entry(const std::string& val) {
		auto result = this->m_children.find(val);
		if (result != this->m_children.end()) {
			return *result->second;
		} else {
			// not found, create new child
			/*std::cout << "Adding new entry " << val << " for " << m_name << " current children: " << std::endl;
			for (auto iter = m_children.begin(); iter != m_children.end(); ++iter) {
				std::cout << iter->first << std::endl;
			}
			*/
			auto conf = std::shared_ptr<Config>(new Config(val, ""));
			auto ins = this->m_children.insert({val, conf});
			Config& ret = *ins.first->second;
			return ret;
		}
	}
	std::string get_item();
	const std::unordered_map<std::string, std::shared_ptr<Config>>* get_children() const { return &this->m_children; }
	std::string get_name() { return this->m_name; }

 private:
	std::unordered_map<std::string, std::vector<config_changed_cb>> m_notify_callbacks;

	std::string m_name;
	std::vector<std::string> m_values;
	std::unordered_map<std::string, std::shared_ptr<Config>> m_children;

	void notify_all();
	void notify();

	std::string m_config_file;

	friend class ConfigManager;
};

class ConfigManager {
 public:
	static ConfigManager& getInstance() {
		static ConfigManager instance;
		return instance;
	}

	Config& get_config(const std::string& name);

 private:
	ConfigManager();
	ConfigManager(ConfigManager const&);
	void operator=(ConfigManager const&);

	std::shared_ptr<Config> m_root_config;
};

#endif
