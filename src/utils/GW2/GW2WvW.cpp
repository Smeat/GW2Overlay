#include "GW2WvW.h"

#include <algorithm>
#include <chrono>
#include <cstddef>
#include <cstdlib>
#include <ctime>
#include <glm/ext/vector_float3.hpp>
#include <glm/geometric.hpp>
#include <iterator>
#include <memory>
#include <string>
#include <thread>

#include <SDL2/SDL.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_ttf.h>

#include "GW2Api.h"
#include "GW2Map.h"

#define WVW_ENDPOINT "v2/wvw/matches?world=2204"
#define WVW_OBJECTIVE_ENDPOINT "v2/wvw/objectives/"

std::unordered_map<char, std::shared_ptr<Texture>> CharacterObject::m_textures;

CharacterObject::CharacterObject(const std::string& chars, std::shared_ptr<Renderer> renderer,
								 std::shared_ptr<Shader> shader) {
	for (const auto& c : chars) {
		this->m_last_char = c;
		std::shared_ptr<Texture> tex;
		auto f = CharacterObject::m_textures.find(c);
		if (f != CharacterObject::m_textures.end()) {
			tex = f->second;
		} else {
			TTF_Font* font = TTF_OpenFont("/usr/share/fonts/TTF/DejaVuSans.ttf", 24);
			if (!font) {
				printf("TTF_OpenFont: %s\n", TTF_GetError());
			}
			SDL_Color textColor = {255, 255, 255};
			char buf[2];
			buf[0] = c;
			buf[1] = '\0';
			SDL_Surface* surfaceMessage = TTF_RenderText_Blended(font, buf, textColor);
			SDL_SaveBMP(surfaceMessage, "/tmp/out.bmp");
			tex = renderer->load_texture(surfaceMessage);
			std::cout << "[FONT] Created " << buf << std::endl;
			SDL_FreeSurface(surfaceMessage);
		}
		this->m_textures.insert({c, tex});
		auto mesh = std::make_shared<Mesh>(Mesh::create_default_mesh());
		std::shared_ptr<TexturedMesh> my_mesh(new TexturedMesh(mesh, tex));
		std::cout << "[WvW-Character] Creating character" << std::endl;
		this->m_characters[c] = renderer->load_object(shader, {my_mesh});
		this->m_characters[c]->scale({0, 0, 0});
		this->set_character(c);
		std::cout << "[WvW-Character] end" << std::endl;
	}
}

void CharacterObject::disable() { this->m_characters[this->m_last_char]->scale({0, 0, 0}); }

void CharacterObject::set_character(char character) {
	if (this->m_characters.find(character) == this->m_characters.end()) return;
	for (const auto& c : this->m_characters) {
		c.second->scale({0, 0, 0});
	}
	this->m_characters[this->m_last_char]->scale({0, 0, 0});
	this->m_characters[character]->scale({10, 10, 10});
	this->m_last_char = character;
}

std::vector<std::shared_ptr<Object>> CharacterObject::get_objects() {
	std::vector<std::shared_ptr<Object>> ret;
	for (const auto& o : this->m_characters) {
		ret.push_back(o.second);
	}
	return ret;
}

GW2WvWObject::GW2WvWObject(std::shared_ptr<Renderer> renderer, std::shared_ptr<Shader> shader,
						   const std::vector<char>& icon_data) {
	std::cout << "[WvW-Object] Constructor" << std::endl;
	this->m_characters[0] = std::shared_ptr<CharacterObject>(new CharacterObject("0123456789", renderer, shader));
	this->m_characters[1] = std::shared_ptr<CharacterObject>(new CharacterObject("0123456", renderer, shader));
	this->m_characters[2] = std::shared_ptr<CharacterObject>(new CharacterObject(":", renderer, shader));
	this->m_characters[3] = std::shared_ptr<CharacterObject>(new CharacterObject("012345", renderer, shader));

	std::cout << "[WvW-Object] Creating surface with size " << icon_data.size() << std::endl;
	// TODO: use a map in GW2WvW or create a texture manager to avoid duplication
	SDL_Surface* surf = Texture::load_image((void*)icon_data.data(), icon_data.size());
	std::cout << "[WvW-Object] Creating texture with surf " << surf << std::endl;
	auto tex = renderer->load_texture(surf);
	SDL_FreeSurface(surf);
	std::cout << "[WvW-Object] Creating mesh" << std::endl;
	// FIXME: empty for now, since we only support one mesh anyway
	for (int i = 0; i < this->OBJECTIVE_COLORS.size(); ++i) {
		auto mesh = std::make_shared<Mesh>(Mesh::create_default_mesh(OBJECTIVE_COLORS[i]));
		std::shared_ptr<TexturedMesh> my_mesh(new TexturedMesh(mesh, tex));
		std::cout << "[WvW-Object] Creating object" << std::endl;
		this->m_object_symbols[i] = renderer->load_object(shader, {my_mesh});
	}
	this->set_team(GREY);
}

void GW2WvWObject::set_team(int team) { this->m_current_team = team; }

void GW2WvWObject::update(const glm::vec3& pos, uint64_t button_mask) {
	float offset =
		std::max(50 - glm::distance(pos, *this->m_object_symbols[this->m_current_team]->get_position()), 0.0f);
	for (const auto& o : this->m_object_symbols) {
		o->scale({0, 0, 0});
		o->set_offset({0, offset, 0});
	}
	int spacing = 0;
	for (const auto& c : this->m_characters) {
		c->set_offset({spacing, offset + 10, 0});
		spacing += 10;
	}
	this->m_object_symbols[this->m_current_team]->scale({10, 10, 10});
}

void GW2WvWObject::set_time(int seconds) {
	std::cout << "[WvW Object] Setting time to " << seconds << std::endl;
	int minute = seconds / 60;
	int seconds_remain = seconds % 60;
	int second_10 = seconds_remain / 10;
	int second_1 = seconds_remain % 10;
	this->m_characters[3]->set_character('0' + minute);
	this->m_characters[2]->set_character(':');
	this->m_characters[1]->set_character('0' + second_10);
	this->m_characters[0]->set_character('0' + second_1);
}

void GW2WvWObject::disable_timer() {
	for (const auto& c : this->m_characters) {
		c->disable();
	}
}

GW2WvW::GW2WvW(std::shared_ptr<Renderer> renderer, std::shared_ptr<Shader> shader) {
	// Caching is okay, since we are only interested in the map ids for now
	auto data = GW2ApiManager::getInstance().get_api()->get_value(WVW_ENDPOINT);
	write_lock lock(this->m_last_data_mutex);
	this->m_last_data = json::parse(data);
	// std::cout << "got data: " << this->m_last_data << std::endl;
	this->m_renderer = renderer;
	this->m_shader = shader;
}

void GW2WvW::set_world_id(int id) { this->m_world_id = id; }

std::vector<std::shared_ptr<GW2Object>> GW2WvW::set_map_id(int id) {
	this->m_current_map_id = id;
	std::cout << "[WvW] Setting map id" << std::endl;
	read_lock lock(this->m_last_data_mutex);
	this->m_objects.clear();
	for (auto iter = this->m_last_data["maps"].begin(); iter != this->m_last_data["maps"].end(); ++iter) {
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
			this->start_update_thread();
			break;
		} else {
			std::cout << "Not a WvW map. Stopping thread and clearing objects" << std::endl;
			this->m_run_update = false;
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
	for (const auto& o : this->m_object_symbols) {
		ret.push_back(o);
	}
	// TODO: return character objects
	for (const auto& c : this->m_characters) {
		auto char_objs = c->get_objects();
		std::copy(char_objs.begin(), char_objs.end(), std::back_inserter(ret));
	}

	return ret;
}

void GW2WvW::start_update_thread() {
	auto thread_func = [this]() {
		std::cout << "[THREAD] Starting WvW Thread" << std::endl;
		while (this->m_run_update) {
			try {
				auto data = GW2ApiManager::getInstance().get_api()->get_value(WVW_ENDPOINT, false);
				write_lock lock(this->m_last_data_mutex);
				this->m_last_data = json::parse(data);

				for (auto iter = this->m_last_data["maps"].begin(); iter != this->m_last_data["maps"].end(); ++iter) {
					int val = iter->operator[]("id");
					if (val == this->m_current_map_id)
						for (auto obj = iter->operator[]("objectives").begin();
							 obj != iter->operator[]("objectives").end(); ++obj) {
							// get all the textures for the given object
							std::string obj_id = obj->operator[]("id");
							std::string owner = obj->operator[]("owner");
							auto objective = this->m_objects.find(obj_id);
							if (objective != this->m_objects.end()) {
								auto team = TEAM_NAME_MAP.find(owner);
								if (team != TEAM_NAME_MAP.end()) {
									objective->second->set_team(team->second);
								}
								std::string flip_str = obj->operator[]("last_flipped");
								tm flip_tm;
								strptime(flip_str.c_str(), "%Y-%m-%dT%H:%M:%SZ", &flip_tm);
								time_t flip_time = mktime(&flip_tm);
								time_t now_local = ::time(NULL);
								tm now_tm = *gmtime(&now_local);
								now_tm.tm_isdst = -1;
								time_t now_utc = mktime(&now_tm);
								double time_diff = ::difftime(now_utc, flip_time);
								if (time_diff < (300)) {
									int time_till_cap = 300 - time_diff;
									objective->second->set_time(time_till_cap);
								} else {
									objective->second->disable_timer();
								}
							}
						}
				}
			} catch (...) {
			}
			// TODO: use a condition variable instead
			std::this_thread::sleep_for(std::chrono::milliseconds(this->m_update_rate_ms));
		}
		std::cout << "[THREAD] Stopping WvW Thread" << std::endl;
	};
	this->m_run_update = false;
	std::cout << "Checking if we need to join an old thread" << std::endl;
	if (this->m_update_thread.joinable()) {
		std::cout << "Joining old thread..." << std::endl;
		this->m_update_thread.join();
	}
	this->m_run_update = true;
	std::cout << "Starting thread!" << std::endl;
	this->m_update_thread = std::thread(thread_func);
	std::cout << "Start thread end!" << std::endl;
}
