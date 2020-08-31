#ifndef __GW2MANAGER_H__
#define __GW2MANAGER_H__

#include <sched.h>
#include "GW2Link.h"

#include "../ProcessUtils.h"

class GW2Manager {
 public:
	static GW2Manager& getInstance() {
		static GW2Manager instance;
		return instance;
	}
	bool is_gw2_running();
	bool is_helper_running();

	void start_helper();

 private:
	pid_t m_gw2_pid = 0;
	pid_t m_helper_pid = 0;
};

#endif
