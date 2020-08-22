#include "GW2Api.h"

#include <filesystem>
#include <fstream>
#include <memory>
#include <string>

#include <netdb.h>
#include <openssl/err.h>
#include <openssl/ssl.h>
#include <openssl/tls1.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "json/json.hpp"

using json = nlohmann::json;

std::shared_ptr<addrinfo> getaddrinfo(const char* __name, const char* __service, const struct addrinfo* __req) {
	addrinfo* info = nullptr;
	::getaddrinfo(__name, __service, __req, &info);
	auto deleter = [](addrinfo* info) {
		if (info) ::freeaddrinfo(info);
	};
	return std::shared_ptr<addrinfo>(info, deleter);
}

GW2Api::GW2Api(const std::string& cache_folder) {
	SSL_library_init();
	SSLeay_add_ssl_algorithms();
	SSL_load_error_strings();
	this->m_cache_folder = cache_folder;
}

std::string GW2Api::get_value(const std::string& endpoint, bool cached) {
	std::string value;
	if (cached) {
		value = this->get_cached(endpoint);
		if (!value.empty()) return value;
	}

	int sock = -1;
	addrinfo hints{};
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	auto addr = getaddrinfo(GW2_HOST.c_str(), "443", &hints);

	sock = ::socket(addr->ai_family, SOCK_STREAM, 0);
	int result = ::connect(sock, addr->ai_addr, addr->ai_addrlen);

	if (result != 0) return "";

	// connected
	// start ssl stuff (the API doesn't reply to plain http)
	const SSL_METHOD* method = TLS_client_method();
	SSL_CTX* ctx = SSL_CTX_new(method);
	SSL_CTX_set_min_proto_version(ctx, TLS1_2_VERSION);
	SSL* ssl = SSL_new(ctx);
	SSL_set_fd(ssl, sock);
	int err = SSL_connect(ssl);
	if (err <= 0) {
		printf("Error creating SSL connection.  err=%x\n", err);
		fflush(stdout);
	}
	printf("SSL connection using %s\n", SSL_get_cipher(ssl));

	std::string msg = "GET https://api.guildwars2.com/" + endpoint + "\n\n";
	std::cout << msg << std::endl;

	if (SSL_write(ssl, msg.c_str(), msg.size()) == msg.size()) {
		char buf[2048];
		memset(buf, 0, 2048);
		// TODO: read more data?
		SSL_read(ssl, buf, 2048);
		try {
			auto j = json::parse(buf);
			// valid answer
			value = buf;
			this->save_to_cache(endpoint, value);
		} catch (...) {
		}
	}

	return value;
}

std::string GW2Api::get_cached(const std::string& endpoint) {
	std::filesystem::path target_file = std::filesystem::path(this->m_cache_folder) / endpoint;
	std::ifstream input(target_file);
	if (input) {
		std::string val((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
		return val;
	}
	return "";
}
void GW2Api::save_to_cache(const std::string& endpoint, const std::string& value) {
	std::filesystem::path target_file = std::filesystem::path(this->m_cache_folder) / endpoint;
	std::filesystem::path target_dir = target_file.parent_path();
	std::filesystem::create_directories(target_dir);

	std::ofstream of(target_file);
	if (of) {
		of << value.c_str();
	}
}
