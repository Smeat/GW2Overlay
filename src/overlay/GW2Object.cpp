#include "GW2Object.h"
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

GW2TrailObject::GW2TrailObject(std::shared_ptr<Trail> trail, std::shared_ptr<Renderer> renderer,
							   std::shared_ptr<Texture> tex) {
	trail->load_trail_data();
	std::vector<Vertex> vertices;
	std::vector<uint16_t> indices;
	int current_index = 0;
	float width = 5;
	float x_pos = 20;
	TrailData* prev_data = nullptr;
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
	for (auto iter = trail->m_trailData.begin(); iter != trail->m_trailData.end(); ++iter) {
		if (prev_data) {
			vertices.push_back(
				Vertex(glm::vec3(normalize(prev_data->x - width), normalize(prev_data->y), normalize(prev_data->z)),
					   glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 0.0f)));
			vertices.push_back(
				Vertex(glm::vec3(normalize(prev_data->x + width), normalize(prev_data->y), normalize(prev_data->z)),
					   glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(1.0f, 0.0f)));
			vertices.push_back(Vertex(glm::vec3(normalize(iter->x + width), normalize(iter->y), normalize(iter->z)),
									  glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(1.0f, 1.0f)));
			vertices.push_back(Vertex(glm::vec3(normalize(iter->x - width), normalize(iter->y), normalize(iter->z)),
									  glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 1.0f)));
			indices.push_back(current_index);
			indices.push_back(current_index + 1);
			indices.push_back(current_index + 2);
			indices.push_back(current_index + 2);
			indices.push_back(current_index + 3);
			indices.push_back(current_index);
			current_index += 4;
		}
		prev_data = &(*iter);
	}
	// print vertex list
	std::cout << "Vertex list" << std::endl;
	for (auto iter = vertices.begin(); iter != vertices.end(); ++iter) {
		std::cout << "v " << iter->pos.x << " " << iter->pos.y << " " << iter->pos.z << std::endl;
	}
	for (int i = 0; i < indices.size(); i += 3) {
		std::cout << "f " << indices[i] + 1 << " " << indices[i + 1] + 1 << " " << indices[i + 2] + 1 << std::endl;
	}
	auto cube_mesh = renderer->load_mesh(vertices, indices);
	std::shared_ptr<TexturedMesh> my_mesh(new TexturedMesh(cube_mesh, tex));
	this->set_meshes({my_mesh});
	glm::vec3 pos = trail->get_pos();
	pos.y += trail->get_height_offset();
	this->translate(pos);
	this->scale({trail->get_icon_size() * 1.0f, trail->get_icon_size() * 1.0f, 1.0f});
}
