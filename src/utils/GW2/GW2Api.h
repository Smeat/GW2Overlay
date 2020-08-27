#ifndef __GW2API_H__
#define __GW2API_H__

#include <iostream>
#include <string>
#include <vector>

const std::string GW2_HOST = "api.guildwars2.com";

const std::vector<std::string> AUTHENTICATED_ENDPOINTS = {"v2/account"};

struct GW2Achievement {
	int m_id = 0;
	std::vector<int> m_bits;
	int m_current = 0;
	int m_max = 0;
	bool m_done = false;
	int m_repeated = 0;
	bool m_unlocked = false;
};

class GW2Api {
 public:
	GW2Api(const std::string& cache_folder);

	std::string get_value(const std::string& endpoint, bool cached = true);
	/// Returns "" if no cached value is found
	std::string get_cached(const std::string& endpoint);
	void save_to_cache(const std::string& endpoint, const std::string& value);

 private:
	std::string m_cache_folder;
	std::string m_api_key;
};

#endif
