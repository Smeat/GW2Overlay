#ifndef __RENDERER_H__
#define __RENDERER_H__

#include <memory>
#include <vector>

#include "../Object.h"

class Renderer {
 public:
	virtual void init() = 0;
	virtual void update(std::vector<std::shared_ptr<Object>> objs) = 0;
	virtual void clear() = 0;
};

#endif	// __RENDERER_H__
