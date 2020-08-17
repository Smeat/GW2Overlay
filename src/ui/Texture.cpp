#include "Texture.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <GL/glew.h>
#include <GL/gl.h>


#include <SDL2/SDL_surface.h>
#include <iostream>

Texture::Texture(const std::string& path){
	SDL_Surface* surf = IMG_Load(path.c_str());
	if(!surf) {
		std::cerr << "Failed to load texture " << path << std::endl;
		SDL_FreeSurface(surf);
		return;
	}
	glGenTextures(1, &this->m_id);
	glBindTexture(GL_TEXTURE_2D, this->m_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int mode = surf->format->BytesPerPixel == 4 ? GL_RGBA : GL_RGB;

	glTexImage2D(GL_TEXTURE_2D, 0, mode, surf->w, surf->h, 0, mode, GL_UNSIGNED_BYTE, surf->pixels);
	glGenerateMipmap(GL_TEXTURE_2D);
	SDL_FreeSurface(surf);
}

void Texture::set_active() {
	glBindTexture(GL_TEXTURE_2D, this->m_id);
}
