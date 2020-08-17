#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include <string>

class Texture {
	public:
		Texture(const std::string& path);
		void set_active();
	
	private:
		unsigned int m_id;

};

#endif
