#ifndef __GW2LINK_H__
#define __GW2LINK_H__

#include <stdint.h>
#include <string>

struct __attribute__((packed)) MumbleContext {
	unsigned char serverAddress[28];  // contains sockaddr_in or sockaddr_in6
	uint32_t mapId;
	uint32_t mapType;
	uint32_t shardId;
	uint32_t instance;
	uint32_t buildId;
	// Additional data beyond the 48 bytes Mumble uses for identification
	uint32_t uiState;  // Bitmask: Bit 1 = IsMapOpen, Bit 2 = IsCompassTopRight,
					   // Bit 3 = DoesCompassHaveRotationEnabled, Bit 4 = Game
					   // has focus, Bit 5 = Is in Competitive game mode, Bit 6
					   // = Textbox has focus, Bit 7 = Is in Combat
	uint16_t compassWidth;	 // pixels
	uint16_t compassHeight;	 // pixels
	float compassRotation;	 // radians
	float playerX;			 // continentCoords
	float playerY;			 // continentCoords
	float mapCenterX;		 // continentCoords
	float mapCenterY;		 // continentCoords
	float mapScale;
	uint32_t processId;
	uint8_t mountIndex;
};

struct __attribute__((packed)) LinkedMem {
	uint32_t uiVersion;
	uint32_t uiTick;
	float fAvatarPosition[3];
	float fAvatarFront[3];
	float fAvatarTop[3];
	uint16_t name[256];
	float fCameraPosition[3];
	float fCameraFront[3];
	float fCameraTop[3];
	uint16_t identity[256];
	uint32_t context_len;  // always 48
	unsigned char context[256];
	//	wchar_t description[2048]; // always empty

	std::string get_identity() const;
	const MumbleContext* get_context() const;
};

static_assert(sizeof(LinkedMem) == 1364, "");
static_assert(sizeof(MumbleContext) == 85, "");

class GW2Link {
 public:
	GW2Link();
	void update_gw2(bool block = false);
	const LinkedMem* get_gw2_data() const;

 private:
	int create_socket();
	int m_socket;
	LinkedMem m_gw2_data;
};

#endif
