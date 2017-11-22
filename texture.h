#pragma once
#include <SDL2/SDL_image.h>

class Texture
{
public:
	// TODO Add destructor

	void init(const char *path)
	{
		// SDL_Surface* res_texture = IMG_Load("assets/tex.png");
		SDL_Surface* res_texture = IMG_Load(path);
		if(!res_texture)
			Log(Log::DIE) << "IMG_Load: " << SDL_GetError();

		//SDL_Surface* res_texture = IMG_Load("assets/tex.png");
		// TTF_Font* font = TTF_OpenFont("assets/Hack.ttf", 90 );
		// if(!font)
		// 	Log(Log::DIE) << "Cannot initaialise specific font " << TTF_GetError();
		// SDL_Color textColor = { 255, 255, 255, 255 }; // white

		// SDL_Surface *res_texture  = TTF_RenderText_Blended(font, "Qqrq", textColor);
		// if(!res_texture)
		// 	Log(Log::DIE) << "IMG_Load: " << SDL_GetError();

		// SDL_Surface *rt  = TTF_RenderText_Blended(font, "Qqrq", textColor);
		// SDL_Surface *res_texture = SDL_CreateRGBSurface(SDL_SWSURFACE, 512, 512,
		// 	32,
		// 	0xff000000,
		// 	0x00ff0000,
		// 	0x0000ff00,
		// 	0x000000ff
		// );
		//
		// SDL_Rect rect;
		// rect.x = 0;
		// rect.y = 0;
		// rect.w = 512;
		// rect.h = 512;
		//
		// SDL_BlitSurface(rt, NULL, res_texture, &rect);
		// SDL_FreeSurface(rt);

		glGenTextures(1, &texture_id);
		glBindTexture(GL_TEXTURE_2D, texture_id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, // target
			0, // level, 0 = base, no minimap,
			GL_RGBA, // internalformat
			res_texture->w, // width
			res_texture->h, // height
			0, // border, always 0 in OpenGL ES
			GL_RGBA, // format
			GL_UNSIGNED_BYTE, // type
			res_texture->pixels);
		SDL_FreeSurface(res_texture);
	}

	void apply()
	{
		glActiveTexture(GL_TEXTURE0);
		glUniform1i(uniform_mytexture, /*GL_TEXTURE*/0);
		glBindTexture(GL_TEXTURE_2D, texture_id);
	}

private:
	GLuint texture_id;
	GLint uniform_mytexture;
};

