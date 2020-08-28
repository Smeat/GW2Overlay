#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

typedef std::function<void(const std::string&)> config_changed_cb;

const std::unordered_map<std::string, std::string> DEFAULT_CONFIG = {
	{"USE_KEY", "f"}, {"ICON_POS_X", "1580"}, {"ICON_POS_Y", "0"}, {"ICON_SCALE", "1.0"}};

class Config {
 public:
	Config() = default;
	void load_config(const std::string& file);
	void save_config(const std::string& file);

	void set_config_item(const std::string& item, const std::string& value);
	std::string get_config_item(const std::string& item);

	void add_callback(const std::string& item, config_changed_cb cb);
	void remove_callback(const std::string& item, config_changed_cb cb);

 private:
	std::unordered_map<std::string, std::string> m_config_items = DEFAULT_CONFIG;
	std::unordered_map<std::string, std::vector<config_changed_cb>> m_notify_callbacks;

	void notify_all();
	void notify(const std::string& item);
};

#endif
