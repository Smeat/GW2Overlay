#ifndef __GW2WVW_H__
#define __GW2WVW_H__

#include <array>
#include <glm/ext/vector_float3.hpp>
#include <memory>
#include <unordered_map>
#include <vector>

#include "../json/json.hpp"

using json = nlohmann::json;

#include "../../overlay/GW2Object.h"
#include "../../overlay/renderer/Renderer.h"

#include "../../overlay/Object.h"
#include "../../overlay/Texture.h"

class CharacterObject {
 public:
	CharacterObject(const std::string& characters, std::shared_ptr<Renderer> rend);

	void set_character(char character);
	void translate(glm::vec3 pos);

 private:
	// just have n objects for every digit. avoids having to create dynamic textures
	std::unordered_map<char, std::shared_ptr<Object>> m_characters;
	static std::unordered_map<char, std::shared_ptr<Texture>> m_textures;
	char m_last_char = 0;
};

class GW2WvWObject : public GW2Object {
 public:
	GW2WvWObject(std::shared_ptr<Renderer> renderer, std::shared_ptr<Shader> shader,
				 const std::vector<char>& icon_data);
	virtual ~GW2WvWObject() = default;
	void set_timer_enabled(bool enable);
	void set_team(int team);
	void set_time(int minute, int second_10, int second_1);
	void translate(const glm::vec3& pos);

	enum teamColor { GREY = 0, RED, GREEN, BLUE };

	void update(const glm::vec3& pos, uint64_t button_mask) override{};
	std::vector<std::shared_ptr<Object>> get_objects() override;

 private:
	const std::vector<glm::vec3> OBJECTIVE_COLORS = {
		{1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}};
	// r, g, b, grey
	std::array<std::shared_ptr<Object>, 4> m_object_symbols;
	// m:ss
	std::array<std::shared_ptr<CharacterObject>, 4> m_characters;
};

class GW2WvW {
 public:
	GW2WvW(std::shared_ptr<Renderer> renderer, std::shared_ptr<Shader> shader);
	void set_world_id(int id);
	std::vector<std::shared_ptr<GW2Object>> set_map_id(int id);
	// endpoint: https://api.guildwars2.com/v2/wvw/matches?world=2204
	void update();

 private:
	int m_world_id = 0;
	std::unordered_map<std::string, std::shared_ptr<GW2WvWObject>> m_objects;
	json m_last_data;
	std::shared_ptr<Renderer> m_renderer;
	std::shared_ptr<Shader> m_shader;
};

#endif
