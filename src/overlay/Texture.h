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
#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include <iostream>
#include <string>

#include <SDL2/SDL_image.h>
#include <SDL2/SDL_surface.h>

class Texture {
 public:
	virtual void set_active() = 0;
	SDL_Surface* load_image(const std::string& filename) {
		SDL_Surface* surf = IMG_Load(filename.c_str());
		// TODO: use SDL_DisplayFormat?
		if (!surf) {
			std::cerr << "Failed to load texture " << filename << std::endl;
			throw std::runtime_error("failed to load image file!");
		}
		return surf;
	}
};

#endif
