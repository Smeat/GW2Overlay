#include "GW2Api.h"

#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <memory>
#include <string>
#include <vector>

#include <netdb.h>
#include <openssl/err.h>
#include <openssl/ssl.h>
#include <openssl/tls1.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "../json/json.hpp"

#include "../Config.h"

using json = nlohmann::json;

std::shared_ptr<addrinfo> getaddrinfo(const char* __name, const char* __service, const struct addrinfo* __req) {
	addrinfo* info = nullptr;
	int ret = ::getaddrinfo(__name, __service, __req, &info);
	if (ret) {
		std::cout << "[Error] getaddrinfo returned " << ret << std::endl;
	}
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

std::vector<char> GW2Api::get_data(const std::string& host, const std::string& file) {
	int sock = -1;
	addrinfo hints{};
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	auto addr = getaddrinfo(host.c_str(), "443", &hints);

	if (!addr) return {};
	sock = ::socket(addr->ai_family, SOCK_STREAM, 0);
	int result = ::connect(sock, addr->ai_addr, addr->ai_addrlen);

	if (result != 0) return {};

	// connected
	// start ssl stuff (the API doesn't reply to plain http)
	const SSL_METHOD* method = TLS_client_method();
	SSL_CTX* ctx = SSL_CTX_new(method);
	SSL_CTX_set_min_proto_version(ctx, TLS1_2_VERSION);
	// SSL_CTX_set_max_proto_version(ctx, TLS1_2_VERSION);
	SSL* ssl = SSL_new(ctx);
	SSL_set_fd(ssl, sock);
	SSL_set_tlsext_host_name(ssl, host.c_str());
	int err = SSL_connect(ssl);
	if (err <= 0) {
		printf("Error creating SSL connection.  err=%x ssl_err %d\n", err, SSL_get_error(ssl, err));
		ERR_print_errors_fp(stderr);
		fflush(stdout);
	}
	printf("SSL connection using cipher %s and ssl version %s\n", SSL_get_cipher(ssl), SSL_get_version(ssl));
	std::string msg = "GET /" + file + " HTTP/1.1\nHost: " + host + "\r\n\r\n";

	std::cout << "Sending request: " << msg << std::endl << "To host : " << host << std::endl;
	std::vector<char> data;

	if (SSL_write(ssl, msg.c_str(), msg.size()) == msg.size()) {
		char buf;
		int bytes = 0;
		int total_bytes = 0;
		do {
			bytes = SSL_read(ssl, &buf, 1);
			data.push_back(buf);
		} while (bytes != 0 && (std::string(data.data(), data.size()).find("\r\n\r\n") == std::string::npos));
		data.push_back('\0');
		// header done!
		// read actual data
		std::string header = data.data();
		size_t content_pos = header.find("Content-Length: ");
		size_t num_begin = header.find(" ", content_pos);
		size_t num_end = header.find("\n", num_begin);
		std::string num_str(header.substr(num_begin + 1, num_end));
		data.clear();
		size_t msg_size = std::atoi(num_str.c_str());
		std::cout << "Reading " << msg_size << "bytes\n";
		do {
			SSL_read(ssl, &buf, 1);
			data.push_back(buf);
		} while (data.size() < msg_size);
		// TODO: timeout?
		std::cout << "Reading msg done with total bytes of " << data.size() << std::endl;
	} else {
		std::cout << "Failed to send request" << std::endl;
	}

	std::cout << "End request" << std::endl;

	SSL_free(ssl);
	SSL_CTX_free(ctx);
	::close(sock);

	return data;
}
std::vector<char> GW2Api::get_render(const std::string& signature, const std::string& file_id,
									 const std::string& format, bool cached) {
	std::string filename = signature + "/" + file_id + "." + format;
	std::string cached_path = "render/" + filename;
	std::vector<char> value;
	if (cached) {
		value = this->get_cached(cached_path);
		if (!value.empty()) return value;
	}
	value = this->get_data(GW2_HOST_RENDER, "file/" + filename);
	this->save_to_cache(cached_path, value);
	return value;
}

std::string GW2Api::get_value(const std::string& endpoint, bool cached) {
	std::string value;
	if (cached) {
		auto cached_data = this->get_cached(endpoint);
		if (cached_data.size()) {
			value = std::string(cached_data.data(), cached_data.size());
		}
		if (!value.empty()) return value;
	}
	std::cout << "Not found in cache...downloading" << std::endl;
	std::string msg = endpoint;
	if (std::find_if(AUTHENTICATED_ENDPOINTS.begin(), AUTHENTICATED_ENDPOINTS.end(), [&](const std::string& s) {
			return endpoint.find(s) != std::string::npos;
		}) != AUTHENTICATED_ENDPOINTS.end()) {
		msg += "?access_token=" + this->m_api_key;
	}
	std::cout << "Getting data with msg " << msg << std::endl;
	std::vector<char> data = this->get_data(GW2_HOST_API, msg);
	data.push_back('\0');
	try {
		auto j = json::parse(data.data());
		// valid answer
		value = data.data();
		this->save_to_cache(endpoint, data);
	} catch (...) {
		std::cout << "Failed to decode answer : " << data.data() << std::endl;
	}
	return value;
}

std::vector<char> GW2Api::get_cached(const std::string& endpoint) {
	std::filesystem::path target_file = std::filesystem::path(this->m_cache_folder) / endpoint;
	std::cout << "Trying to read cache from " << target_file << std::endl;
	std::ifstream input(target_file);
	if (input) {
		std::vector<char> val((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
		return val;
	}
	return {};
}
void GW2Api::save_to_cache(const std::string& endpoint, const std::vector<char>& value) {
	std::filesystem::path target_file = std::filesystem::path(this->m_cache_folder) / endpoint;
	std::filesystem::path target_dir = target_file.parent_path();
	std::filesystem::create_directories(target_dir);

	std::ofstream of(target_file);
	if (of) {
		of.write(value.data(), value.size());
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
