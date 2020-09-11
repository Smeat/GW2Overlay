#ifndef __GW2WVW_H__
#define __GW2WVW_H__

#include <array>
#include <glm/ext/vector_float3.hpp>
#include <memory>
#include <thread>
#include <unordered_map>
#include <vector>

#include "../json/json.hpp"

using json = nlohmann::json;

#include "../../overlay/GW2Object.h"
#include "../../overlay/renderer/Renderer.h"

#include "../../overlay/Object.h"
#include "../../overlay/Texture.h"

#include "../Lock.h"

class CharacterObject : public GW2Object {
 public:
	CharacterObject(const std::string& characters, std::shared_ptr<Renderer> rend, std::shared_ptr<Shader> shader);

	void set_character(char character);
	void disable();
	virtual std::vector<std::shared_ptr<Object>> get_objects() override;
	virtual void update(const glm::vec3& player_pos, uint64_t button_mask) override{};

 private:
	// just have n objects for every digit. avoids having to create dynamic textures
	std::unordered_map<char, std::shared_ptr<Object>> m_characters;
	static std::unordered_map<char, std::shared_ptr<Texture>> m_textures;
	char m_last_char = 0;
};

enum teamColor { GREY = 0, RED, GREEN, BLUE };

class GW2WvWObject : public GW2Object {
 public:
	GW2WvWObject(std::shared_ptr<Renderer> renderer, std::shared_ptr<Shader> shader,
				 const std::vector<char>& icon_data);
	virtual ~GW2WvWObject() = default;
	void disable_timer();
	void set_team(int team);
	void set_time(int seconds);

	virtual void update(const glm::vec3& pos, uint64_t button_mask) override;
	std::vector<std::shared_ptr<Object>> get_objects() override;

 private:
	const std::vector<glm::vec3> OBJECTIVE_COLORS = {
		{1.0f, 1.0f, 1.0f}, {3.0f, 0.3f, 0.3f}, {0.15f, 1.8f, 0.5f}, {0.2f, 2.0f, 7.0f}};
	// r, g, b, grey
	std::array<std::shared_ptr<Object>, 4> m_object_symbols;
	// m:ss
	std::array<std::shared_ptr<CharacterObject>, 4> m_characters;

	int m_current_team = 0;
};
const std::unordered_map<std::string, int> TEAM_NAME_MAP = {
	{"Red", RED}, {"Green", GREEN}, {"Blue", BLUE}, {"Neutral", GREY}};

class GW2WvW {
 public:
	GW2WvW(std::shared_ptr<Renderer> renderer, std::shared_ptr<Shader> shader);
	void set_world_id(int id);
	std::vector<std::shared_ptr<GW2Object>> set_map_id(int id);
	// endpoint: https://api.guildwars2.com/v2/wvw/matches?world=2204
	void update();

 private:
	int m_world_id = 0;
	int m_current_map_id = 0;
	std::unordered_map<std::string, std::shared_ptr<GW2WvWObject>> m_objects;
	json m_last_data;
	mutex_type m_last_data_mutex;
	std::shared_ptr<Renderer> m_renderer;
	std::shared_ptr<Shader> m_shader;
	std::thread m_update_thread;
	int m_update_rate_ms = 10000;

	bool m_run_update = false;

	void start_update_thread();
};

#endif
