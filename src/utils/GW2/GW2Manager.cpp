#include "GW2Manager.h"
#include <algorithm>
#include <csignal>
#include <string>
#include <vector>

#include "../Config.h"

bool GW2Manager::is_gw2_running() {
	this->m_gw2_pid = ProcessUtils::find_wine_process("GW2-64.exe");
	return this->m_gw2_pid > 0;
}
bool GW2Manager::is_helper_running() {
	// TODO: check if crashed?
	return this->m_helper_pid > 0 && ProcessUtils::is_child_running(this->m_helper_pid);
}

void GW2Manager::start_helper() {
	if (this->is_gw2_running()) {
		// kill previous process
		if (this->is_helper_running()) {
			kill(this->m_helper_pid, SIGKILL);
		}
		auto env = ProcessUtils::get_environment(this->m_gw2_pid);

		std::vector<std::string> envs_to_remove = {"WINESERVERSOCKET", "WINELOADERNOEXEC", "WINEPRELOADRESERVE",
												   "LD_PRELOAD"};

		// TODO: use erase_if from c++20
		auto rem_it = std::remove_if(env.begin(), env.end(), [&envs_to_remove](const auto& val) {
			for (const auto& to_remove : envs_to_remove) {
				if (val.find(to_remove) != std::string::npos) {
					return true;
				}
			}
			return false;
		});
		env.erase(rem_it, env.end());
		std::string wine = ProcessUtils::get_env_val(env, "WINE");
		std::string prefix = ProcessUtils::get_env_val(env, "WINEPREFIX");
		std::cout << "Wine: " << wine << " prefix " << prefix << std::endl;
		std::string helper_path = ConfigManager::getInstance().get_config("SETTINGS")["HELPER_SCRIPT"].get_item();

		if (!helper_path.empty()) {
			this->m_helper_pid =
				ProcessUtils::start_process(wine,
											{prefix + "/drive_c/Program Files/Python38/python.exe",
											 "/home/kevin/Dokumente/Projekte/Github/GW2Overlay/src/helper/mumble.py"},
											env);
			std::cout << "Started helper with pid " << this->m_helper_pid << std::endl;
		}
	}
}
