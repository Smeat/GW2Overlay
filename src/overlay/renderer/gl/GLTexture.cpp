/*
 * Copyright (c) 2020 smeat.
 *
 * This file is part of GW2Overlay
 * (see https://github.com/Smeat/GW2Overlay).
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include "GLTexture.h"
#include <GL/glew.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <GL/gl.h>

#include <SDL2/SDL_surface.h>
#include <iostream>

GLTexture::GLTexture(SDL_Surface* surf) {
	glGenTextures(1, &this->m_id);
	glBindTexture(GL_TEXTURE_2D, this->m_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
					GL_REPEAT);	 // set texture wrapping to GL_REPEAT (default
								 // wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int mode = surf->format->BytesPerPixel == 4 ? GL_RGBA : GL_RGB;

	glTexImage2D(GL_TEXTURE_2D, 0, mode, surf->w, surf->h, 0, mode, GL_UNSIGNED_BYTE, surf->pixels);
	glGenerateMipmap(GL_TEXTURE_2D);
}

GLTexture::~GLTexture() {
	glBindTexture(GL_TEXTURE_2D, 0);
	glDeleteTextures(1, &this->m_id);
}

void GLTexture::set_active() { glBindTexture(GL_TEXTURE_2D, this->m_id); }
