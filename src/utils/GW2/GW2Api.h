#ifndef __GW2API_H__
#define __GW2API_H__

#include <bits/stdint-uintn.h>
#include <cstring>
#include <iostream>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

#include "../json/json.hpp"
using json = nlohmann::json;

const std::string GW2_HOST_API = "api.guildwars2.com";
const std::string GW2_URL_API = "https://" + GW2_HOST_API + "/";
const std::string GW2_HOST_RENDER = "render.guildwars2.com";
const std::string GW2_URL_RENDER = "https://" + GW2_HOST_RENDER + "/file/";

const std::vector<std::string> AUTHENTICATED_ENDPOINTS = {"v2/account", "v2/tokeninfo", "v2/characters"};

enum GW2Permission {
	ACCOUNT = (1 << 0),
	BUILDS = (1 << 1),
	CHARACTERS = (1 << 2),
	GUILDS = (1 << 3),
	INVENTORIES = (1 << 4),
	PROGRESSION = (1 << 5),
	PVP = (1 << 6),
	TRADINGPOST = (1 << 7),
	UNLOCKS = (1 << 8),
	WALLET = (1 << 9)
};

const std::unordered_map<std::string, GW2Permission> PERMISSION_MAP = {
	{"account", ACCOUNT}, {"builds", BUILDS},			{"characters", CHARACTERS},
	{"guilds", GUILDS},	  {"inventories", INVENTORIES}, {"progression", PROGRESSION},
	{"pvp", PVP},		  {"tradingpost", TRADINGPOST}, {"unlocks", UNLOCKS},
	{"wallet", WALLET}};

class GW2Api {
 public:
	GW2Api(const std::string& cache_folder = "./cache");

	std::vector<char> get_data(const std::string& url, const std::string& msg);
	std::vector<char> get_render(const std::string& signature, const std::string& file_id, const std::string& format,
								 bool cached = true);
	std::vector<char> get_render(const std::string& url, bool cached = true);
	std::string get_value(const std::string& endpoint, std::vector<std::string> parameter, bool cached = true);
	std::string get_value(const std::string& endpoint, bool cached = true) {
		return this->get_value(endpoint, {}, cached);
	}
	/// Returns "" if no cached value is found
	std::vector<char> get_cached(const std::string& endpoint);
	void save_to_cache(const std::string& endpoint, const std::vector<char>& value);

	void set_api_key(const std::string& key) {
		this->m_api_key = key;
		std::cout << "GW2 API key " << m_api_key << std::endl;
		this->check_permissions();
	}

	void check_permissions();
	bool has_permission(uint32_t perm);
	uint32_t get_permissions() const { return this->m_permissions; };

 private:
	std::string m_cache_folder;
	std::string m_api_key;
	uint32_t m_permissions;
};

#endif
