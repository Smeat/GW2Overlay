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

	void load_map_id() {
		std::ifstream input_stream(this->m_trail_filename, std::ifstream::binary);
		size_t total_len = 0;
		if (input_stream) {
			this->m_trailData.clear();
			input_stream.seekg(0, input_stream.end);
			total_len = input_stream.tellg();
			input_stream.seekg(0, input_stream.beg);
			if (total_len < 8) return;
			input_stream.seekg(4);	// skip header
			uint32_t map_id = 0;
			input_stream.read(reinterpret_cast<char*>(&map_id), sizeof(uint32_t));
			this->set_map_id(map_id);
		}
	}

	void load_trail_data() {
		std::cout << "[Trail] Loading data for " << this->m_trail_filename << std::endl;
		std::ifstream input_stream(this->m_trail_filename, std::ifstream::binary);
		size_t total_len = 0;
		if (input_stream) {
			this->m_trailData.clear();
			input_stream.seekg(0, input_stream.end);
			total_len = input_stream.tellg();
			input_stream.seekg(0, input_stream.beg);
			if (total_len < 8) return;
			input_stream.seekg(4);	// skip header
			uint32_t map_id = 0;
			input_stream.read(reinterpret_cast<char*>(&map_id), sizeof(uint32_t));
			this->set_map_id(map_id);

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
	}

	std::vector<std::shared_ptr<POI>> generate_pois() {
		std::vector<std::shared_ptr<POI>> pois;
		TrailData* last_data = nullptr;
		for (auto iter = this->m_trailData.begin(); iter != this->m_trailData.end(); ++iter) {
			auto poi = POI::create_poi(this->m_this.lock());
			poi->set_x(iter->x);
			poi->set_y(iter->y);
			poi->set_z(iter->z);
			poi->set_is_poi(true);
			this->add_child(poi);
			pois.push_back(poi);
			std::cout << "Created new trail poi with icon " << poi->get_icon_file() << " at x " << poi->get_pos().x
					  << " y " << poi->get_pos().y << " z " << poi->get_pos().z << std::endl;

			if (false && last_data) {
				auto interpolated_poi = POI::create_poi(this->m_this.lock());
				interpolated_poi->set_x((last_data->x + iter->x) / 2.0);
				interpolated_poi->set_y((last_data->y + iter->y) / 2.0);
				interpolated_poi->set_z((last_data->z + iter->z) / 2.0);
				pois.push_back(interpolated_poi);
			}

			last_data = &(*iter);
		}
		return pois;
	}
};

#endif	// __TRAIL_H_
