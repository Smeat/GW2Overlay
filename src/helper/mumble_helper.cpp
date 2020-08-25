#include <winsock2.h>

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <windows.h>

struct LinkedMem {
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
	uint32_t context_len;
	unsigned char context[256];
	uint16_t description[2048];
};

LinkedMem *lm = NULL;
int sock = -1;
struct sockaddr_in servaddr;

HANDLE initFileMapping() {
	HANDLE hMapObject = OpenFileMappingW(FILE_MAP_ALL_ACCESS, FALSE, L"MumbleLink");
	if (hMapObject == NULL) {
		printf("Couldn't open existing mapping...creating a new one\n");
		hMapObject = CreateFileMapping(INVALID_HANDLE_VALUE,  // use paging file
									   NULL,				  // default security
									   PAGE_READWRITE,		  // read/write access
									   0,					  // maximum object size (high-order DWORD)
									   sizeof(LinkedMem),	  // maximum object size (low-order DWORD)
									   "MumbleLink");		  // name of mapping object

		if (hMapObject == NULL) {
			printf("Could not create file mapping object (%d).\n", GetLastError());
			return;
		}
	}
	printf("File fd %d\n", hMapObject);
	return hMapObject;
}

void initView(HANDLE hMapObject) {
	lm = (LinkedMem *)MapViewOfFile(hMapObject, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(LinkedMem));
	if (lm == NULL) {
		CloseHandle(hMapObject);
		hMapObject = NULL;
		printf("Couldn't open view\n");
		return;
	}
	printf("Init done\n");
}

int init_socket() {
	WORD wVersionRequested;
	WSADATA wsaData;
	wVersionRequested = MAKEWORD(1, 1);
	if (WSAStartup(wVersionRequested, &wsaData) != 0) {
		return -1;
	}

	int fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (fd == -1) {
		printf("Failed to create socket\n");
		return fd;
	}
	memset(&servaddr, 0, sizeof(servaddr));
	// Filling server information
	servaddr.sin_family = AF_INET;	// IPv4
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	servaddr.sin_port = htons(7070);

	printf("Created socket\n");
	return fd;
}

int main() {
	initFileMapping() return;
	sock = init_socket();
	int last_tick = 0;
	while (true) {
		//	if (lm->uiTick > last_tick) {
		sendto(sock, (const char *)lm, sizeof(LinkedMem), 0, (SOCKADDR *)&servaddr, sizeof(servaddr));
		//	}
		Sleep(1 / 120.0);
	}
}
