#ifndef __GW2BUILDS_H_
#define __GW2BUILDS_H_

#include <bits/stdint-uintn.h>
#include <stdint.h>
#include <cstddef>
#include <cstring>
#include <map>
#include <string>
#include <unordered_map>

#include "../base64.h"
#include "GW2Api.h"
#include "GW2Manager.h"

#include "../json/json.hpp"
using json = nlohmann::json;

static const std::unordered_map<int, int> PALETTE_MAP = {
	{42355, 5984}, {45460, 5971}, {41714, 5963}, {41100, 5959}, {45449, 5958}, {40183, 5941}, {44948, 5934},
	{42935, 5924}, {41615, 5921}, {41372, 5920}, {46148, 5909}, {43638, 5906}, {45380, 5904}, {45142, 5889},
	{45789, 5882}, {40498, 5865}, {43739, 5861}, {41205, 5860}, {44926, 5851}, {44080, 5827}, {42851, 5810},
	{41158, 5804}, {45333, 5789}, {45046, 5770}, {43148, 5758}, {44612, 5755}, {40915, 5754}, {44266, 5754},
	{42917, 5752}, {43745, 5750}, {40274, 5746}, {41919, 5738}, {42842, 5719}, {40507, 5717}, {45508, 5693},
	{44646, 5685}, {45970, 5684}, {41218, 5679}, {45717, 5678}, {43123, 5671}, {46335, 5663}, {43357, 5656},
	{43064, 5639}, {44239, 5632}, {45746, 5621}, {45088, 5617}, {42009, 5616}, {40200, 5614}, {41065, 5600},
	{30369, 4905}, {31248, 4903}, {30772, 4879}, {30101, 4878}, {31407, 4873}, {30525, 4868}, {30105, 4867},
	{29786, 4862}, {30871, 4858}, {30800, 4857}, {30189, 4850}, {29414, 4849}, {30305, 4848}, {29516, 4846},
	{29519, 4845}, {29666, 4843}, {31746, 4838}, {30074, 4828}, {30357, 4825}, {30258, 4823}, {31322, 4821},
	{30814, 4815}, {29739, 4812}, {29535, 4807}, {29941, 4804}, {30662, 4803}, {30343, 4802}, {30488, 4801},
	{30025, 4796}, {31582, 4792}, {30661, 4790}, {30273, 4789}, {31677, 4788}, {30359, 4787}, {30868, 4784},
	{29921, 4782}, {30238, 4776}, {30670, 4774}, {29948, 4773}, {29844, 4773}, {29613, 4769}, {29968, 4761},
	{30400, 4756}, {29526, 4755}, {30364, 4746}, {29965, 4745}, {29856, 4743}, {30553, 4740}, {30815, 4739},
	{30568, 4727}, {30047, 4726}, {30432, 4724}, {30461, 4721}, {28231, 4651}, {27025, 4651}, {29082, 4651},
	{27505, 4651}, {27014, 4651}, {26679, 4651}, {56662, 4651}, {40485, 4651}, {29209, 4614}, {26821, 4614},
	{29310, 4614}, {27322, 4614}, {28379, 4614}, {28516, 4614}, {56841, 4614}, {42949, 4614}, {26937, 4572},
	{29148, 4572}, {28219, 4572}, {27220, 4572}, {27372, 4572}, {56661, 4572}, {45686, 4572}, {27107, 4564},
	{27715, 4564}, {29197, 4564}, {27917, 4564}, {31100, 4564}, {26644, 4564}, {26557, 4564}, {56752, 4564},
	{41220, 4564}, {28406, 4554}, {31294, 4554}, {27356, 4554}, {29114, 4554}, {28287, 4554}, {27760, 4554},
	{27975, 4554}, {56773, 4554}, {45773, 4554}, {12340, 3939}, {21659, 3882}, {30881, 3882}, {21815, 3881},
	{21762, 3880}, {21656, 3879}, {21664, 3878}, {21773, 3877}, {21778, 3876}, {21750, 3875}, {12319, 2333},
	{14575, 482},  {12344, 456},  {5734, 446},	 {10685, 445},	{10311, 444},  {10377, 444},  {13117, 443},
	{9246, 441},   {29578, 438},  {14528, 429},	 {12491, 428},	{12500, 427},  {12633, 421},  {14502, 418},
	{13085, 415},  {10245, 410},  {10606, 409},	 {12569, 407},	{12550, 406},  {5933, 405},	  {5927, 403},
	{10267, 399},  {5912, 398},	  {5910, 397},	 {29522, 397},	{5860, 396},   {5904, 394},	  {5868, 393},
	{10341, 390},  {10237, 389},  {10244, 388},	 {10234, 387},	{10247, 386},  {10232, 385},  {10236, 384},
	{10302, 383},  {14483, 380},  {10550, 378},	 {9245, 376},	{10620, 375},  {10612, 374},  {10622, 373},
	{10609, 372},  {10673, 372},  {10545, 371},	 {10671, 371},	{12324, 369},  {10589, 368},  {10611, 367},
	{10176, 366},  {10213, 365},  {10608, 364},	 {10202, 363},	{10203, 362},  {10211, 361},  {10207, 360},
	{10204, 359},  {10201, 358},  {10200, 357},	 {10197, 356},	{5968, 353},   {5865, 352},	  {29591, 352},
	{5862, 351},   {5861, 350},	  {12367, 349},	 {13066, 347},	{13096, 346},  {13065, 345},  {13064, 344},
	{13062, 343},  {13060, 342},  {13093, 341},	 {13057, 340},	{13056, 339},  {12363, 338},  {12362, 337},
	{5539, 336},   {5639, 335},	  {5638, 334},	 {22572, 334},	{5635, 333},   {9187, 332},	  {9253, 331},
	{9182, 330},   {9175, 329},	  {9168, 328},	 {9247, 327},	{9242, 326},   {9150, 326},	  {12361, 324},
	{5624, 322},   {10607, 320},  {10672, 320},	 {13055, 318},	{14479, 317},  {9158, 312},	  {9154, 311},
	{9153, 310},   {9152, 309},	  {13044, 308},	 {13046, 307},	{9163, 306},   {9243, 306},	  {9151, 305},
	{9244, 305},   {10602, 304},  {13037, 303},	 {10543, 302},	{9084, 301},   {5857, 296},	  {6161, 294},
	{30337, 294},  {5838, 292},	  {5837, 291},	 {5836, 290},	{5573, 285},   {5572, 284},	  {13038, 283},
	{10187, 282},  {13035, 281},  {10185, 280},	 {5569, 279},	{9128, 278},   {5834, 276},	  {5825, 275},
	{30828, 275},  {5832, 274},	  {20451, 274},	 {10177, 271},	{13132, 270},  {13028, 269},  {13027, 268},
	{13026, 267},  {13021, 266},  {5812, 263},	 {5567, 261},	{9248, 260},   {9102, 259},	  {5821, 257},
	{9125, 256},   {9251, 255},	  {9093, 254},	 {9241, 254},	{10583, 250},  {5641, 246},	  {5703, 246},
	{10562, 245},  {14419, 238},  {12516, 237},	 {5554, 235},	{15795, 235},  {5546, 230},	  {10541, 228},
	{12360, 210},  {5542, 203},	  {5540, 202},	 {12502, 194},	{12501, 193},  {12580, 192},  {12537, 191},
	{12499, 190},  {12498, 189},  {12497, 188},	 {2631, 187},	{12496, 186},  {12495, 185},  {12494, 184},
	{12632, 183},  {12493, 182},  {12492, 181},	 {34309, 180},	{14413, 179},  {14412, 178},  {14408, 176},
	{14410, 175},  {14409, 174},  {14516, 173},	 {14407, 172},	{14406, 171},  {14405, 170},  {14404, 169},
	{14403, 168},  {14401, 167},  {14402, 166},	 {5818, 163},	{10548, 162},  {12489, 161},  {14355, 156},
	{10547, 155},  {12542, 154},  {12385, 152},	 {5516, 151},	{5534, 150},   {10646, 149},  {10549, 146},
	{5571, 145},   {5536, 144},	  {5570, 143},	 {5535, 142},	{10689, 139},  {9085, 138},	  {13020, 137},
	{5811, 136},   {29991, 136},  {5805, 134},	 {6020, 134},	{13050, 133},  {5802, 132},	  {10546, 129},
	{10674, 129},  {10544, 128},  {9083, 127},	 {31914, 121},	{12483, 120},  {10533, 118},  {5507, 117},
	{5503, 116},   {5506, 115},	  {5502, 114},	 {14392, 113},	{14389, 112},  {14388, 110},  {14372, 106},
	{14368, 105},  {13002, 88},	  {13082, 40},	 {5666, 38},	{12457, 37},   {12339, 33},	  {12403, 31},
	{12456, 30},   {12401, 29},	  {12453, 28},	 {12476, 27},	{12450, 25},   {12447, 23},	  {12440, 21},
	{12391, 20},   {10527, 18},	  {10670, 18},	 {12325, 17},	{12323, 14},   {12387, 13},	  {12320, 12},
	{14354, 10},   {12318, 9},	  {12338, 8},	 {12373, 7},	{12337, 4},	   {12417, 2},	  {12343, 1}};

static const std::unordered_map<std::string, int> PROFESSION_MAP = {
	{"Guardian", 1}, {"Warrior", 2},	  {"Engineer", 3}, {"Ranger", 3},	 {"Ranger", 4},
	{"Thief", 5},	 {"Elementalist", 6}, {"Mesmer", 7},   {"Necromant", 8}, {"Revenant", 9}};

#pragma pack(push, 1)
typedef struct specialization_s {
	uint8_t specialization;
	uint8_t trait_adept : 2;
	uint8_t trait_master : 2;
	uint8_t trait_grandmaster : 2;
	uint8_t trait_padding : 2;
} specialization_t;

struct GW2BuildChatLink {
	struct {
		uint8_t magic_number = 0x0D;
		uint8_t profession;
		specialization_t specialization[3];
		// 8
		uint16_t land_heal;
		uint16_t water_heal;
		uint16_t land_utility1;
		uint16_t water_utility1;
		uint16_t land_utility2;
		uint16_t water_utility2;
		uint16_t land_utility3;
		uint16_t water_utility3;
		uint16_t land_elite;
		uint16_t water_elite;
		// 28
		union {
			struct {
				uint8_t land_pet1;
				uint8_t land_pet2;
				uint8_t water_pet1;
				uint8_t water_pet2;
				uint8_t padding[12];
			} ranger;
			struct {
				uint8_t land_active_legend;
				uint8_t land_inactive_legend;
				uint8_t water_active_legend;
				uint8_t water_inactive_legend;
				uint16_t land_inactive_legend_utility1;
				uint16_t land_inactive_legend_utility2;
				uint16_t land_inactive_legend_utility3;
				uint16_t water_inactive_legend_utility1;
				uint16_t water_inactive_legend_utility2;
				uint16_t water_inactive_legend_utility3;
			} revenant;
		};
	} chat_data;
	std::string name;

	GW2BuildChatLink(std::string json_str) {
		memset(&this->chat_data, 0, sizeof(this->chat_data));
		this->chat_data.magic_number = 0x0d;
		size_t pos = 0;
		std::string search = "null";
		// replace all "null" with 0
		while ((pos = json_str.find(search, pos)) != std::string::npos) {
			json_str.replace(pos, search.length(), "0");
			pos += 1;
		}
		json j_obj = json::parse(json_str);
		this->name = j_obj["name"].get<std::string>();
		this->set_profession(j_obj["profession"].get<std::string>());
		for (int i = 0; i < 3; ++i) {
			auto spec = j_obj["specializations"][i];
			this->chat_data.specialization[i].specialization = spec["id"].get<uint8_t>();

			this->chat_data.specialization[i].trait_adept = get_trait_location(spec["traits"][0].get<uint16_t>());
			this->chat_data.specialization[i].trait_master = get_trait_location(spec["traits"][1].get<uint16_t>());
			this->chat_data.specialization[i].trait_grandmaster = get_trait_location(spec["traits"][2].get<uint16_t>());
		}
		auto skills = j_obj["skills"];
		this->chat_data.land_heal = get_skill(skills["heal"]);
		this->chat_data.land_utility1 = get_skill(skills["utilities"][0]);
		this->chat_data.land_utility2 = get_skill(skills["utilities"][1]);
		this->chat_data.land_utility3 = get_skill(skills["utilities"][2]);
		this->chat_data.land_elite = get_skill(skills["elite"]);
		skills = j_obj["aquatic_skills"];
		this->chat_data.water_heal = get_skill(skills["heal"]);
		this->chat_data.water_utility1 = get_skill(skills["utilities"][0]);
		this->chat_data.water_utility2 = get_skill(skills["utilities"][1]);
		this->chat_data.water_utility3 = get_skill(skills["utilities"][2]);
		this->chat_data.water_elite = get_skill(skills["elite"]);
	}

	uint8_t get_trait_location(uint16_t id) {
		if (id == 0) return 0;
		try {
			GW2Api* api = GW2Manager::getInstance().get_api();
			auto val = json::parse(api->get_value("v2/traits/" + std::to_string(id)));
			return val["order"].get<uint8_t>() + 1;
		} catch (...) {
			return 0;
		}
	}

	uint16_t get_skill(const json& obj) {
		try {
			auto it = PALETTE_MAP.find(obj.get<int>());
			if (it == PALETTE_MAP.end()) {
				return 0;
			} else {
				return it->second;
			}
		} catch (...) {
			return 0;
		}
	}

	void set_profession(std::string prof) {
		auto res = PROFESSION_MAP.find(prof);
		if (res != PROFESSION_MAP.end()) {
			this->chat_data.profession = res->second;
		}
	}

	std::string get_profession() {
		for (auto iter = PROFESSION_MAP.begin(); iter != PROFESSION_MAP.end(); ++iter) {
			if (this->chat_data.profession == iter->second) {
				return iter->first;
			}
		}
		return "";
	}

	std::string get_chat_string() {
		size_t size = Base64::EncodedLength(sizeof(this->chat_data)) + 1;
		char* b64_string = (char*)malloc(size);
		Base64::Encode((char*)&this->chat_data, sizeof(this->chat_data), b64_string, size);
		std::string chat_string = "[&" + std::string(b64_string) + "]";
		free(b64_string);
		return chat_string;
	}
};
#pragma pack(pop)

static_assert(sizeof(GW2BuildChatLink::chat_data) == 44, "GW2BuildChatLink size is wrong!");

#endif	// __GW2BUILDS_H_
