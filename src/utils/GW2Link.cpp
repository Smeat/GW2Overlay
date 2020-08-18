#include "GW2Link.h"

#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sstream>

GW2Link::GW2Link() {
	this->m_socket = this->create_socket();
	printf("Init GW2Link with Linkedmem size %lu, context size %lu total %lu\n",
		   sizeof(LinkedMem), sizeof(MumbleContext),
		   sizeof(LinkedMem) + sizeof(MumbleContext));
}

void GW2Link::update_gw2(bool block) {
	struct sockaddr_in si_other;
	int len = 0, slen = sizeof(si_other);
	LinkedMem data;
	int flags = 0;
	if (block) {
		int timeout = 200;
		struct timeval tv;
		tv.tv_sec = timeout / 1000;
		tv.tv_usec = (timeout % 1000) * 1000;
		fd_set rfds;
		FD_ZERO(&rfds);
		FD_SET(this->m_socket, &rfds);
		::select(FD_SETSIZE, &rfds, nullptr, nullptr, &tv);
	} else {
		flags |= MSG_DONTWAIT;
	}
	if ((len = ::recvfrom(this->m_socket, &data, sizeof(data), MSG_DONTWAIT,
						  (struct sockaddr *)&si_other, (socklen_t *)&slen)) >
		0) {
		// clang-format off
		/*
		printf("uiVersion %d, uiTick %d\n", data.uiVersion, data.uiTick);
		printf("fAvatarPosition (%f %f %f) fAvatarFront (%f %f %f) fAvatarTop (% f % f % f)\n", data.fAvatarPosition[0], data.fAvatarPosition[1],
			   data.fAvatarPosition[2],
			   data.fAvatarFront[0], data.fAvatarFront[1], data.fAvatarFront[2],
			   data.fAvatarTop[0], data.fAvatarTop[1], data.fAvatarTop[2]);
		printf("fCameraPosition (%f %f %f) fCameraFront (%f %f %f) fCameraTop (% f % f % f)\n", data.fCameraPosition[0], data.fCameraPosition[1],
			   data.fCameraPosition[2],
			   data.fCameraFront[0], data.fCameraFront[1], data.fCameraFront[2],
			   data.fCameraTop[0], data.fCameraTop[1], data.fCameraTop[2]);
		printf("Contextlen %d\n", data.context_len);
		printf("Packet len %d should be %lu, diff % d ctx %lu\n ", len, sizeof(LinkedMem), int(sizeof(LinkedMem) - len), sizeof(MumbleContext));
		*/
		// clang-format on

		this->m_gw2_data = data;
	}
}

const LinkedMem *GW2Link::get_gw2_data() const { return &this->m_gw2_data; }

int GW2Link::create_socket() {
	int fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (fd == -1) {
		printf("Failed to create socket\n");
		return fd;
	}
	struct sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(servaddr));
	// Filling server information
	servaddr.sin_family = AF_INET;	// IPv4
	servaddr.sin_addr.s_addr = INADDR_ANY;
	servaddr.sin_port = htons(7070);

	// TODO: proper getaddr
	if (::bind(fd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) ==
		-1) {
		printf("Failed to bind socket\n");
		::close(fd);
		fd = -1;
	}

	printf("Created socket\n");
	return fd;
}

std::string LinkedMem::get_identity() const {
	char c = this->identity[0];
	int i = 0;
	std::stringstream ss;
	while (c != 0 && i < 256) {
		i += 1;
		ss << c;
		c = this->identity[i];
	}
	return ss.str();
}

const MumbleContext *LinkedMem::get_context() const {
	return (MumbleContext *)this->context;
}
