#ifndef __PERFORMANCE_STATS_H_
#define __PERFORMANCE_STATS_H_

#include <stdint.h>

#include <string>
#include <unordered_map>

class PerformanceStats {
 public:
	static PerformanceStats& getInstance() {
		static PerformanceStats instance;
		return instance;
	}

	void set_time(const std::string& type, uint64_t time_us);
	uint64_t get_time(const std::string& type) const;
	std::unordered_map<std::string, uint64_t> get_times() const;

 private:
	PerformanceStats(){};
	std::unordered_map<std::string, uint64_t> m_times;
};

#endif	// __PERFORMANCE_STATS_H_

