#ifndef __GW2API_H__
#define __GW2API_H__

#include <iostream>

const std::string GW2_HOST = "api.guildwars2.com";

class GW2Api {
 public:
	GW2Api(const std::string& cache_folder);

	std::string get_value(const std::string& endpoint, bool cached = true);
	/// Returns "" if no cached value is found
	std::string get_cached(const std::string& endpoint);
	void save_to_cache(const std::string& endpoint, const std::string& value);

 private:
	std::string m_cache_folder;
};

#endif
