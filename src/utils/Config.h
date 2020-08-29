#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

typedef std::function<void(const std::string&)> config_changed_cb;

const std::unordered_map<std::string, std::string> DEFAULT_CONFIG_SETTINGS = {
	{"USE_KEY", "f"}, {"ICON_POS_X", "1580"}, {"ICON_POS_Y", "0"}, {"ICON_SCALE", "1.0"}};

const std::unordered_map<std::string, std::string> DEFAULT_CONFIGS = {{"SETTINGS", "./settings.json"},
																	  {"BUILDS", "./builds.json"}};

class Config {
 public:
	Config() = default;
	Config(const std::string& file);
	~Config();
	void load_config(std::string file = "");
	void save_config(std::string file = "");

	void set_item(const std::string& item, const std::string& value);
	std::string get_item(const std::string& item);

	void add_callback(const std::string& item, config_changed_cb cb);
	void remove_callback(const std::string& item, config_changed_cb cb);

 private:
	std::unordered_map<std::string, std::string> m_config_items = DEFAULT_CONFIG_SETTINGS;
	std::unordered_map<std::string, std::vector<config_changed_cb>> m_notify_callbacks;

	void notify_all();
	void notify(const std::string& item);

	std::string m_config_file;
};

class ConfigManager {
 public:
	static ConfigManager& getInstance() {
		static ConfigManager instance;
		return instance;
	}

	std::shared_ptr<Config> get_config(const std::string& name);

 private:
	ConfigManager();
	ConfigManager(ConfigManager const&);
	void operator=(ConfigManager const&);

	std::unordered_map<std::string, std::shared_ptr<Config>> m_configs;
};

#endif
