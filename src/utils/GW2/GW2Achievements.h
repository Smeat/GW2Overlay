#ifndef __GW2ACHIEVEMENTS_H__
#define __GW2ACHIEVEMENTS_H__

#include <string>
#include <unordered_map>
#include <vector>

struct GW2Achievement {
	int m_id = 0;
	std::vector<int> m_bits;
	int m_current = 0;
	int m_max = 0;
	bool m_done = false;
	int m_repeated = 0;
	bool m_unlocked = false;
};

class GW2Achievements {
 public:
	GW2Achievements(const std::string& data);
	bool is_done(int id);
	bool has_bit(int id, int bit);

 private:
	std::unordered_map<int, GW2Achievement> m_achievements;
};

#endif
