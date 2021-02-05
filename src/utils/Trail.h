#ifndef __TRAIL_H_
#define __TRAIL_H_

#include <fstream>
#include <ios>
#include <memory>
#include <string>
#include <vector>

#include "POI.h"

struct TrailData {
	float x;
	float y;
	float z;
};

class Trail : public POI {
 private:
	Trail() = default;
	Trail(std::shared_ptr<POI> base) : POI(*base) {}

 public:
	static std::shared_ptr<Trail> create_trail(std::shared_ptr<POI> parent) {
		std::shared_ptr<Trail> t;
		t.reset(new Trail(POI::create_poi()));
		t->m_this = t;
		return t;
	}
	float m_anim_speed = 1.0f;
	std::string m_trail_filename;
	std::vector<TrailData> m_trailData;

	void load_trail_data() {
		std::cout << "[Trail] Loading data for " << this->m_trail_filename << std::endl;
		std::ifstream input_stream(this->m_trail_filename, std::ifstream::binary);
		size_t total_len = 0;
		if (input_stream) {
			this->m_trailData.clear();
			input_stream.seekg(0, input_stream.end);
			total_len = input_stream.tellg();
			input_stream.seekg(0, input_stream.beg);
		}
		input_stream.seekg(4);	// skip header
		input_stream.read(reinterpret_cast<char*>(&this->m_map_id), sizeof(uint32_t));

		total_len -= 8;
		std::cout << "Total size " << total_len << " Values: " << total_len / (4.0f * 3.0f) << std::endl;
		while (input_stream.good()) {
			TrailData trail_data;
			input_stream.read(reinterpret_cast<char*>(&trail_data), 3 * sizeof(float));
			if (input_stream) {
				this->m_trailData.push_back(trail_data);
				total_len -= 3 * sizeof(float);
			}
		}
	}

	std::vector<std::shared_ptr<POI>> generate_pois(const std::shared_ptr<POI> parent) {
		std::vector<std::shared_ptr<POI>> pois;
		std::shared_ptr<POI> trail_root = POI::create_poi();
		trail_root->set_parent(parent);

		return pois;
	}
};

#endif	// __TRAIL_H_
