#include "PerformanceStats.h"
#include <string>
#include <unordered_map>

void PerformanceStats::set_time(const std::string& type, uint64_t time_us) { this->m_times[type] = time_us; }

uint64_t PerformanceStats::get_time(const std::string& type) const {
	auto it = this->m_times.find(type);
	if (it != this->m_times.end()) {
		return it->second;
	}
	return 0;
}

std::unordered_map<std::string, uint64_t> PerformanceStats::get_times() const { return this->m_times; }
