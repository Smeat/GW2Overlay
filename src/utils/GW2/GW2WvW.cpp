#include "GW2WvW.h"

#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <glm/ext/vector_float3.hpp>
#include <iterator>
#include <memory>
#include <string>

#include <SDL2/SDL.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_ttf.h>

#include "GW2Api.h"
#include "GW2Map.h"

#define WVW_ENDPOINT "v2/wvw/matches?world=2204"
#define WVW_OBJECTIVE_ENDPOINT "v2/wvw/objectives/"

std::unordered_map<char, std::shared_ptr<Texture>> CharacterObject::m_textures;

CharacterObject::CharacterObject(const std::string& chars, std::shared_ptr<Renderer> renderer) {
	for (const auto& c : chars) {
		this->m_last_char = c;
		std::shared_ptr<Texture> tex;
		auto f = CharacterObject::m_textures.find(c);
		if (f != CharacterObject::m_textures.end()) {
			tex = f->second;
		} else {
			TTF_Font* font = TTF_OpenFont("Sans.ttf", 24);
			SDL_Color textColor = {255, 255, 255};
			char buf[2];
			buf[0] = c;
			buf[1] = '\0';
			SDL_Surface* surfaceMessage = TTF_RenderText_Solid(font, buf, textColor);
			tex = renderer->load_texture(surfaceMessage);
			SDL_FreeSurface(surfaceMessage);
			this->m_textures.insert({c, tex});
		}
	}
}

void CharacterObject::set_character(char character) {
	if (this->m_characters.find(character) == this->m_characters.end()) return;
	this->m_characters[this->m_last_char]->scale({0, 0, 0});
	this->m_characters[character]->scale({1, 1, 1});
	this->m_last_char = character;
}

void CharacterObject::translate(glm::vec3 pos) {
	for (auto iter = this->m_characters.begin(); iter != this->m_characters.end(); ++iter) {
		iter->second->translate(pos);
	}
}

GW2WvWObject::GW2WvWObject(std::shared_ptr<Renderer> renderer, std::shared_ptr<Shader> shader,
						   const std::vector<char>& icon_data) {
	std::cout << "[WvW-Object] Constructor" << std::endl;
	/*this->m_characters[0] = std::shared_ptr<CharacterObject>(new CharacterObject("0123456789", renderer));
	this->m_characters[1] = std::shared_ptr<CharacterObject>(new CharacterObject("0123456", renderer));
	this->m_characters[2] = std::shared_ptr<CharacterObject>(new CharacterObject(":", renderer));
	this->m_characters[3] = std::shared_ptr<CharacterObject>(new CharacterObject("012345", renderer));
	*/

	std::cout << "[WvW-Object] Creating surface with size " << icon_data.size() << std::endl;
	// TODO: use a map in GW2WvW or create a texture manager to avoid duplication
	SDL_Surface* surf = Texture::load_image((void*)icon_data.data(), icon_data.size());
	std::cout << "[WvW-Object] Creating texture with surf " << surf << std::endl;
	auto tex = renderer->load_texture(surf);
	SDL_FreeSurface(surf);
	std::cout << "[WvW-Object] Creating mesh" << std::endl;
	// FIXME: empty for now, since we only support one mesh anyway
	auto cube_mesh = renderer->load_mesh({}, {});
	std::shared_ptr<TexturedMesh> my_mesh(new TexturedMesh(cube_mesh, tex));
	std::cout << "[WvW-Object] Creating object" << std::endl;
	this->m_object_symbol = renderer->load_object(shader, {my_mesh});
	this->m_object_symbol->scale({10, 10, 10});
}

void GW2WvWObject::translate(const glm::vec3& pos) { this->m_object_symbol->translate(pos); }

GW2WvW::GW2WvW(std::shared_ptr<Renderer> renderer, std::shared_ptr<Shader> shader) {
	// Caching is okay, since we are only interested in the map ids for now
	auto data = GW2ApiManager::getInstance().get_api()->get_value(WVW_ENDPOINT);
	this->m_last_data = json::parse(data);
	// std::cout << "got data: " << this->m_last_data << std::endl;
	this->m_renderer = renderer;
	this->m_shader = shader;
}

void GW2WvW::set_world_id(int id) { this->m_world_id = id; }

std::vector<std::shared_ptr<GW2Object>> GW2WvW::set_map_id(int id) {
	std::cout << "[WvW] Setting map id" << std::endl;
	for (auto iter = this->m_last_data["maps"].begin(); iter != this->m_last_data["maps"].end(); ++iter) {
		std::cout << "[WvW] iter: " << *iter << std::endl;
		int val = iter->operator[]("id");
		if (val == id) {
			std::cout << "On WvW map!" << std::endl;
			// create all objects with all textures and position
			for (auto obj = iter->operator[]("objectives").begin(); obj != iter->operator[]("objectives").end();
				 ++obj) {
				std::cout << "[WvW] Getting objective: " << *obj << std::endl;
				// get all the textures for the given object
				std::string obj_id = obj->operator[]("id");
				// get the position
				json objective_json =
					json::parse(GW2ApiManager::getInstance().get_api()->get_value(WVW_OBJECTIVE_ENDPOINT + obj_id));
				std::cout << "[WvW] Objective data: " << objective_json << std::endl;
				if (!objective_json.contains("coord") || !objective_json.contains("marker")) continue;
				glm::vec3 pos = {objective_json["coord"][0], objective_json["coord"][2], objective_json["coord"][1]};
				//	glm::vec3 pos = {objective_json["label_coord"][0], 0, objective_json["label_coord"][1]};
				std::string icon_link = objective_json["marker"];
				// get extension, id etc from link
				size_t format_pos = icon_link.find_last_of(".");
				size_t id_pos = icon_link.find_last_of("/");
				size_t sig_pos = icon_link.substr(0, id_pos).find_last_of("/");
				std::string format = icon_link.substr(format_pos + 1);
				std::string id = icon_link.substr(id_pos + 1, format_pos - id_pos - 1);
				std::string sig = icon_link.substr(sig_pos + 1, id_pos - sig_pos - 1);
				std::cout << "sig: " << sig << " id " << id << " format " << format << std::endl;
				auto icon = GW2ApiManager::getInstance().get_api()->get_render(sig, id, format);
				std::cout << "[WvW] Creating wvw object..." << std::endl;
				std::shared_ptr<GW2WvWObject> wvw_obj(new GW2WvWObject(this->m_renderer, this->m_shader, icon));
				std::cout << "[WvW] done" << std::endl;
				auto d =
					GW2ApiManager::getInstance().get_api()->get_value(std::string("v2/maps/") + std::to_string(val));
				GW2Map map;
				map.load_map(d);
				std::cout << "[WvW] translate before " << pos.x << "," << pos.y << "," << pos.z << std::endl;
				pos.x = map.continentToMapX(pos.x);
				pos.z = map.continentToMapY(pos.z);
				pos.y = (pos.y / -map.get_max_level()) + 40;
				wvw_obj->translate(pos);
				std::cout << "[WvW] translate done " << pos.x << "," << pos.y << "," << pos.z << std::endl;
				this->m_objects.insert({obj_id, wvw_obj});
				std::cout << "[WvW] insert done" << std::endl;
			}
			break;
		} else {
			this->m_objects.clear();
		}
	}
	std::vector<std::shared_ptr<GW2Object>> ret;
	std::transform(this->m_objects.begin(), this->m_objects.end(), std::back_inserter(ret),
				   [](auto& val) { return std::dynamic_pointer_cast<GW2Object>(val.second); });
	std::cout << "[WvW] Set id done with size " << ret.size() << std::endl;
	return ret;
}

std::vector<std::shared_ptr<Object>> GW2WvWObject::get_objects() {
	std::vector<std::shared_ptr<Object>> ret;
	ret.push_back(this->m_object_symbol);
	// TODO: return character objects

	return ret;
}
