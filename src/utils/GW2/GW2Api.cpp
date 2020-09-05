#include "GW2Api.h"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <memory>
#include <string>

#include <netdb.h>
#include <openssl/err.h>
#include <openssl/ssl.h>
#include <openssl/tls1.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "../json/json.hpp"

#include "../Config.h"

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

	std::string msg = "GET https://api.guildwars2.com/" + endpoint;
	if (std::find_if(AUTHENTICATED_ENDPOINTS.begin(), AUTHENTICATED_ENDPOINTS.end(), [&](const std::string& s) {
			return endpoint.find(s) != std::string::npos;
		}) != AUTHENTICATED_ENDPOINTS.end()) {
		msg += "?access_token=" + this->m_api_key + "\n";
	}

	msg += "\r\n";
	std::cout << msg << std::endl;

	if (SSL_write(ssl, msg.c_str(), msg.size()) == msg.size()) {
#define BUF_SIZE (1024 * 2)
		std::vector<char> data;
		char buf[BUF_SIZE];
		memset(buf, 0, BUF_SIZE);
		// TODO: read more data?
		int bytes = 0;
		int total_bytes = 0;
		do {
			bytes = SSL_read(ssl, buf, BUF_SIZE);
			std::copy(buf, buf + bytes, std::back_inserter(data));
			total_bytes += bytes;
			std::cout << "Read bytes " << bytes << " from " << endpoint << std::endl;
		} while (bytes != 0);
		data.push_back('\0');
		std::cout << "Reading done with total bytes of " << total_bytes << std::endl;
		try {
			auto j = json::parse(data.data());
			// valid answer
			value = data.data();
			this->save_to_cache(endpoint, value);
		} catch (...) {
			std::cout << "Failed to decode answer : " << data.data() << std::endl;
		}
	} else {
		std::cout << "Failed to send request" << std::endl;
	}

	std::cout << "End request" << std::endl;

	return value;
}

std::string GW2Api::get_cached(const std::string& endpoint) {
	std::filesystem::path target_file = std::filesystem::path(this->m_cache_folder) / endpoint;
	std::cout << "Trying to read cache from " << target_file << std::endl;
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
void GW2Api::check_permissions() {
	auto val = this->get_value("v2/tokeninfo", false);
	json j = json::parse(val);
	if (j.contains("permissions") && j["permissions"].is_array()) {
		this->m_permissions = 0;
		for (auto iter = j["permissions"].begin(); iter != j["permissions"].end(); ++iter) {
			auto find = PERMISSION_MAP.find(*iter);
			if (find != PERMISSION_MAP.end()) {
				this->m_permissions |= find->second;
			}
		}
	}
}
bool GW2Api::has_permission(uint32_t perm) { return this->m_permissions & (perm); }
