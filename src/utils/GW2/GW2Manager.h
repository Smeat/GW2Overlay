#ifndef __GW2MANAGER_H__
#define __GW2MANAGER_H__

#include <sched.h>
#include "GW2Link.h"

#include "../ProcessUtils.h"
#include "GW2Api.h"
#include "GW2Link.h"

class GW2Manager {
 public:
	static GW2Manager& getInstance() {
		static GW2Manager instance;
		return instance;
	}
	GW2Api* get_api() { return &this->m_api; }
	GW2Link* get_link() { return &this->m_link; }

	bool is_gw2_running();
	bool is_helper_running();

	void start_helper();

 private:
	GW2Manager(){};
	pid_t m_gw2_pid = 0;
	pid_t m_helper_pid = 0;
	GW2Api m_api;
	GW2Link m_link;
};

#endif
