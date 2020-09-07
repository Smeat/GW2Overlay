#ifndef __GW2API_H__
#define __GW2API_H__

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

const std::string GW2_HOST_API = "api.guildwars2.com";
const std::string GW2_URL_API = "https://" + GW2_HOST_API + "/";
const std::string GW2_HOST_RENDER = "render.guildwars2.com";
const std::string GW2_URL_RENDER = "https://" + GW2_HOST_RENDER + "/file/";

const std::vector<std::string> AUTHENTICATED_ENDPOINTS = {"v2/account", "v2/tokeninfo"};

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
	std::string get_value(const std::string& endpoint, bool cached = true);
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

class GW2ApiManager {
 public:
	static GW2ApiManager& getInstance() {
		static GW2ApiManager instance;
		return instance;
	}

	GW2Api* get_api() { return &this->m_api; }

 private:
	GW2ApiManager(){};
	GW2Api m_api;
};

#endif
