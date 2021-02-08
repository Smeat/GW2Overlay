#include "GW2Object.h"
#include <algorithm>
#include <cmath>
#include <vector>

#include "Mesh.h"
#include "Texture.h"
#include "renderer/Renderer.h"

GW2POIObject::GW2POIObject(std::shared_ptr<POI> poi) {
	this->m_poi = poi;
	glm::vec3 pos = poi->get_pos();
	pos.y += poi->get_height_offset();
	this->translate(pos);
	this->scale({poi->get_icon_size() * 1.0f, poi->get_icon_size() * 1.0f, 1.0f});
}

void GW2POIObject::update(const glm::vec3& pos, uint64_t button_mask) {
	// Triggered action
	float distance = glm::distance(this->get_world_position(), pos);
	if (distance <= this->m_poi->get_trigger_range()) {
		bool remove = this->m_poi->get_auto_trigger();
		// TODO: set flag in config for daily reset etc
		remove |= ((this->m_poi->get_behavior() == poiBehavior::REAPPEAR_ON_DAILY_RESET) ||
				   (this->m_poi->get_behavior() == poiBehavior::ONLY_VISIBLE_BEFORE_ACTIVATION)) &&
				  button_mask;
		// TODO: use actual button mask
		if (remove) {
			// XXX: just scale it down, since we'd need to rebuild the command queue otherwise
			this->scale({0, 0, 0});
			this->m_inactive = true;
			this->m_disabled = true;
		}
	}
	if (!this->m_disabled) {
		if ((this->m_poi->get_fade_far() > 0 && distance > this->m_poi->get_fade_far()) ||
			(this->m_poi->get_fade_near() > 0 && distance < this->m_poi->get_fade_near() && false)) {
			if (!this->m_inactive) {
				std::cout << "Disabling object " << this->m_poi->get_type() << " with distance " << distance << " far "
						  << this->m_poi->get_fade_far() << " near " << this->m_poi->get_fade_near() << std::endl;
				this->scale({0, 0, 0});
				this->m_inactive = true;
			}
		} else if (this->m_inactive) {
			std::cout << "Enabling object " << this->m_poi->get_type() << " with distance " << distance << " far "
					  << this->m_poi->get_fade_far() << " near " << this->m_poi->get_fade_near() << std::endl;
			this->scale({this->m_poi->get_icon_size() * 1.0f, this->m_poi->get_icon_size() * 1.0f, 1.0f});
			this->m_inactive = false;
		}
	}
}

void get_perpendicular_points(glm::vec3 p1, glm::vec3 p2, float distance, glm::vec3* out1, glm::vec3* out2) {
	float a = p1.z - p2.z;
	float b = p1.x - p2.x;

	float norm = std::sqrt(a * a + b * b);
	a = a / norm;
	b /= norm;

	out1->x = p2.x - a * distance;
	out1->z = p2.z + b * distance;
	out1->y = p2.y;

	out2->x = p2.x + a * distance;
	out2->z = p2.z - b * distance;
	out2->y = p2.y;
}

GW2TrailObject::GW2TrailObject(std::shared_ptr<Trail> trail, std::shared_ptr<Renderer> renderer,
							   std::shared_ptr<Texture> tex) {
	trail->load_trail_data();
	std::vector<std::vector<Vertex>> mesh_verticies;
	std::vector<std::vector<uint16_t>> mesh_indices;
	int current_index = 0;
	float width = 2;
	float min_pos = FLT_MAX;
	float max_pos = FLT_MIN;
	for (auto iter = trail->m_trailData.begin(); iter != trail->m_trailData.end(); ++iter) {
		min_pos = std::min(min_pos, iter->x);
		min_pos = std::min(min_pos, iter->y);
		min_pos = std::min(min_pos, iter->z);

		max_pos = std::max(max_pos, iter->x);
		max_pos = std::max(max_pos, iter->y);
		max_pos = std::max(max_pos, iter->z);
	}
	max_pos += width;
	min_pos -= width;
	auto normalize = [min_pos, max_pos](float val) {
		float result = 2.0f * ((val - min_pos) / (max_pos - min_pos)) - 1.0f;
		std::cout << "Normalized " << val << " with min " << min_pos << " max " << max_pos << " to " << result
				  << std::endl;
		return result;
	};
	std::vector<Vertex> vertices;
	std::vector<uint16_t> indices;
	TrailData* prev_data = nullptr;
	glm::vec3 prev_p1;
	glm::vec3 prev_p2;
	for (auto iter = trail->m_trailData.begin(); iter != trail->m_trailData.end(); ++iter) {
		if (iter->x == 0 && iter->y == 0 && iter->z == 0) {
			if (vertices.size() > 0 && indices.size() > 0) {
				mesh_verticies.push_back(vertices);
				mesh_indices.push_back(indices);
				vertices.clear();
				indices.clear();
				current_index = 0;
				prev_data = nullptr;
			}
			continue;
		}
		if (prev_data) {
			// TODO: currently the previous points are a duplicate due to the texture coordinates
			vertices.push_back(Vertex(prev_p1, glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 0.0f)));
			vertices.push_back(Vertex(prev_p2, glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(1.0f, 0.0f)));
			float distance = glm::distance(glm::vec3(prev_data->x, prev_data->y, prev_data->z),
										   glm::vec3(iter->x, iter->y, iter->z));
			float length = 7.0f;
			int frac = std::max(1.0f, distance / length);
			get_perpendicular_points({prev_data->x, prev_data->y, prev_data->z}, {iter->x, iter->y, iter->z}, width,
									 &prev_p1, &prev_p2);
			vertices.push_back(Vertex(prev_p2, glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(1.0f, frac)));
			vertices.push_back(Vertex(prev_p1, glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f, frac)));
			indices.push_back(current_index);
			indices.push_back(current_index + 1);
			indices.push_back(current_index + 2);
			indices.push_back(current_index + 2);
			indices.push_back(current_index + 3);
			indices.push_back(current_index);
			current_index += 4;
		} else {
			prev_p1 = glm::vec3(iter->x - width, iter->y, iter->z);
			prev_p2 = glm::vec3(iter->x + width, iter->y, iter->z);
		}
		prev_data = &(*iter);
	}
	// print vertex list

	if (trail->get_type() ==
		"tw_guides.tw_lws5.tw_lws5_bjoramarches.tw_lws5_bjoramarches_eternalicenodes.tw_lws5_bjoramarches_"
		"eternalicenodes_toggletrail") {
		std::ofstream output("/tmp/test.obj");
		std::cout << "Vertex list" << std::endl;
		for (auto iter = vertices.begin(); iter != vertices.end(); ++iter) {
			output << "v " << iter->pos.x << " " << iter->pos.y << " " << iter->pos.z << std::endl;
		}
		for (int i = 0; i < indices.size(); i += 3) {
			output << "f " << indices[i] + 1 << " " << indices[i + 1] + 1 << " " << indices[i + 2] + 1 << std::endl;
		}
	}
	// add the last mesh
	if (vertices.size() > 0 && indices.size() > 0) {
		mesh_verticies.push_back(vertices);
		mesh_indices.push_back(indices);
	}

	std::vector<std::shared_ptr<TexturedMesh>> meshes;
	for (int i = 0; i < mesh_verticies.size(); ++i) {
		auto cube_mesh = renderer->load_mesh(mesh_verticies[i], mesh_indices[i]);
		std::shared_ptr<TexturedMesh> my_mesh(new TexturedMesh(cube_mesh, tex));
		meshes.push_back(my_mesh);
	}
	this->set_meshes(meshes);
	glm::vec3 pos = trail->get_pos();
	pos.y += trail->get_height_offset();
	this->translate(pos);
	this->scale({trail->get_icon_size() * 1.0f, trail->get_icon_size() * 1.0f, 1.0f});
}
