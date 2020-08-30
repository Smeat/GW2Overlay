#ifndef __GW2API_H__
#define __GW2API_H__

#include <iostream>
#include <string>
#include <vector>

const std::string GW2_HOST = "api.guildwars2.com";

const std::vector<std::string> AUTHENTICATED_ENDPOINTS = {"v2/account"};

class GW2Api {
 public:
	GW2Api(const std::string& cache_folder);

	std::string get_value(const std::string& endpoint, bool cached = true);
	/// Returns "" if no cached value is found
	std::string get_cached(const std::string& endpoint);
	void save_to_cache(const std::string& endpoint, const std::string& value);

	void set_api_key(const std::string& key) {
		this->m_api_key = key;
		std::cout << "GW2 API key " << m_api_key << std::endl;
	}

 private:
	std::string m_cache_folder;
	std::string m_api_key;
};

#endif
