#ifndef __PROCESSUTILS_H__
#define __PROCESSUTILS_H__

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

#include <fcntl.h>
#include <unistd.h>
#include <cstring>

#include <spawn.h>
#include <sys/types.h>
#include <sys/wait.h>

class ProcessUtils {
 public:
	static int find_wine_process(const std::string& name) {
		for (auto& p : std::filesystem::directory_iterator("/proc")) {
			bool is_process_dir =
				p.is_directory() && (p.path().filename().string().find_first_not_of("0123456789") == std::string::npos);

			if (is_process_dir) {
				auto cmdline_path = p.path() / "cmdline";
				std::ifstream input_stream(cmdline_path);
				if (input_stream) {
					std::string cmdline;
					std::getline(input_stream, cmdline);
					// std::replace(cmdline.begin(), cmdline.end(), '\000', ' ');
					int pos = cmdline.find_first_of('\000');
					if (pos != std::string::npos) {
						cmdline = cmdline.substr(0, pos);
					}
					if (cmdline.find(name) != std::string::npos) {
						return std::stoi(p.path().filename().string());
					}
				}
			}
		}
	}

	static pid_t start_process(const std::string& cmd, const std::vector<std::string>& args,
							   const std::vector<std::string>& env) {
		pid_t pid = 0;
		std::vector<char*> args_c;
		std::vector<char*> env_c;

		auto convert_to_cstr = [](const std::string& s) {
			char* cstr = new char[s.size() + 1];
			std::strcpy(cstr, s.c_str());
			return cstr;
		};

		args_c.push_back(convert_to_cstr(cmd));
		std::transform(args.begin(), args.end(), std::back_inserter(args_c), convert_to_cstr);
		std::transform(env.begin(), env.end(), std::back_inserter(env_c), convert_to_cstr);
		args_c.push_back(NULL);
		env_c.push_back(NULL);
		posix_spawn_file_actions_t action;
		posix_spawn_file_actions_init(&action);
		posix_spawn_file_actions_addopen(&action, STDOUT_FILENO, "/dev/null", O_WRONLY, 0);
		posix_spawn_file_actions_adddup2(&action, 1, 2);

		int status = posix_spawn(&pid, cmd.c_str(), &action, nullptr, args_c.data(), env_c.data());
		posix_spawn_file_actions_destroy(&action);
		std::cout << "Started process " << cmd << " with return val " << status << " and pid " << pid << std::endl;

		auto clean_vec = [](std::vector<char*>* vec) {
			for (auto iter = vec->begin(); iter != vec->end(); ++iter) {
				if (*iter) {
					delete[] * iter;
				}
			}
		};

		clean_vec(&args_c);
		clean_vec(&env_c);
		return pid;
	}

	static bool is_child_running(pid_t pid) {
		int ret = waitpid(pid, NULL, WNOHANG);
		return ret == 0;
	}

	static std::vector<std::string> get_environment(int pid) {
		std::vector<std::string> env;
		std::ifstream input("/proc/" + std::to_string(pid) + "/environ");
		if (input) {
			std::string data;
			while (std::getline(input, data, '\000')) {
				env.push_back(data);
			}
		}
		return env;
	}

	static std::string get_env_val(const std::vector<std::string>& list_of_envs, const std::string& env_to_find) {
		for (const auto& env : list_of_envs) {
			int pos = env.find_first_of("=");
			if (env.substr(0, pos) == env_to_find) {
				return env.substr(pos + 1);
			}
		}
		return "";
	}
};

#endif
