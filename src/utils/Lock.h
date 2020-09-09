#ifndef __LOCK_H__
#define __LOCK_H__

#include <shared_mutex>
using mutex_type = std::shared_timed_mutex;
using read_lock = std::shared_lock<mutex_type>;
using write_lock = std::unique_lock<mutex_type>;

#endif
