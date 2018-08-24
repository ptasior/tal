#include "texture.h"
#include "shader.h"
#include "log.h"
#include "game.h"
#include "config.h"
#include "global.h"
#if defined(JS) || defined(ANDROID)
	#include <SDL_ttf.h>
	#include <SDL_image.h>
#else
	#include <SDL2/SDL_image.h>
	#include <SDL2/SDL_ttf.h>
#endif
#include <assert.h>
#include <vector>


std::mutex Texture::mMutex;
std::map<std::string, std::shared_ptr<Texture>> Texture::mList;

Uint32 get_pixel32(SDL_Surface *surface, int x, int y);
void put_pixel32(SDL_Surface *surface, int x, int y, Uint32 pixel);

const int SIZE = 250;

Texture::Texture()
{
}

Texture::~Texture()
{
	glDeleteTextures(1, &mTextureId);
}

void Texture::init(const char *path, Shader *shader)
{
	Log() << "Texture: Loading " << path;
	mName = path;
	mShader = shader;
	SDL_Surface* res_texture;
	if(mName.substr(0, 7) == "letter-") // A letter
	{
		assert(false);
	}
	else if(mName.substr(0, 5) == "text-") // A letter
	{
		// TODO mutex
		auto fontName = Global::get<Config>()->get("font");

		auto data = Global::get<Game>()->openResource(fontName);
		auto rw = SDL_RWFromConstMem(data->data(), data->size());

		static TTF_Font* font = TTF_OpenFontRW(rw, 1, SIZE);
		static TTF_Font* fontOutline = TTF_OpenFont(fontName.c_str(), SIZE);

		if(!font)
			Log(Log::DIE) << "Texture: Cannot initaialise specific font " << TTF_GetError();
		// TTF_SetFontOutline(font, 1);
		// TTF_SetFontHinting(font, TTF_HINTING_MONO);
		TTF_SetFontHinting(font, TTF_HINTING_LIGHT);
		// TTF_SetFontHinting(font, TTF_HINTING_NONE);

		const int OUTLINE_SIZE = 3;

		TTF_SetFontKerning(font, 1);
		TTF_SetFontKerning(fontOutline, 1);
		TTF_SetFontOutline(fontOutline, OUTLINE_SIZE);


		SDL_Color white = {255, 255, 255, 255}; // white
		SDL_Color black = {255, 255, 255, 128}; // white
		// SDL_Color black = {0, 0, 0, 255};

		SDL_Surface *bg  = TTF_RenderText_Blended(fontOutline, mName.substr(5).c_str(), black);
		SDL_Surface *fg  = TTF_RenderText_Blended(font, mName.substr(5).c_str(), white);

		if(!fg | !bg)
			Log(Log::DIE) << "Texture: Error in TTF_RenderText_Blended: " << SDL_GetError();

		SDL_Rect rect = {OUTLINE_SIZE, OUTLINE_SIZE, fg->w, fg->h}; 

		SDL_SetSurfaceBlendMode(fg, SDL_BLENDMODE_BLEND);
		SDL_BlitSurface(fg, NULL, bg, &rect);
		SDL_FreeSurface(fg);


		res_texture = SDL_CreateRGBSurface(SDL_SWSURFACE, SIZE, SIZE,
				32, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff
			);

		SDL_Rect rectS = {0, 0, bg->w, bg->h};
		SDL_Rect rectD = {0, 0, SIZE, SIZE};
		SDL_BlitScaled(bg, &rectS, res_texture, &rectD);
		SDL_FreeSurface(bg);

	}
	else // Regular image
	{
		auto data = Global::get<Game>()->openResource(path);
		res_texture = IMG_Load_RW(SDL_RWFromConstMem(data->data(), data->size()), 1);

		if(!res_texture)
			Log(Log::DIE) << "Texture: IMG_Load: " << SDL_GetError();

		if(res_texture->format->BytesPerPixel != 4)
			Log(Log::DIE) << "Texture: Image does not have alpha channel: " << path;
	}

	res_texture = flip(res_texture, SDL_FLIP_VERTICAL);

	glGenTextures(1, &mTextureId);
	glBindTexture(GL_TEXTURE_2D, mTextureId);

	glTexImage2D(GL_TEXTURE_2D, // target
			0, // level, 0 = base, no minimap,
			GL_RGBA, // internalformat
			res_texture->w, // width
			res_texture->h, // height
			0, // border, always 0 in OpenGL ES
			GL_RGBA, // format
			GL_UNSIGNED_BYTE, // type
			res_texture->pixels
		);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);

	SDL_FreeSurface(res_texture);

	mShader->setUniform(mGlslName.c_str(), Shader::Value{.int_val=mGlslId});
}

void Texture::setClamp()
{
	glBindTexture(GL_TEXTURE_2D, mTextureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void Texture::setRepeat()
{
	glBindTexture(GL_TEXTURE_2D, mTextureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void Texture::apply()
{
	glActiveTexture(GL_TEXTURE0+mGlslId);

	mShader->setUniform(mGlslName.c_str(), Shader::Value{.int_val=mGlslId});
	glBindTexture(GL_TEXTURE_2D, mTextureId);
}

void Texture::unbind()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

std::shared_ptr<Texture> Texture::getTexture(const char* path, Shader *s, const char* name, int id)
{
	// TODO Is it ok that texture may be reused with another shader???
	std::lock_guard<std::mutex> lock(mMutex);
	if(!mList.count(path))
	{
		mList[path].reset(new Texture);
		mList[path]->setName(name, id);
		mList[path]->init(path, s);
	}

	return mList[path];
}

Uint32 get_pixel32(SDL_Surface *surface, int x, int y)
{
	//Convert the pixels to 32 bit
	Uint32 *pixels = (Uint32 *)surface->pixels;
	
	//Get the requested pixel
	return pixels[(y * surface->w) + x];
}

void put_pixel32(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
	//Convert the pixels to 32 bit
	Uint32 *pixels = (Uint32 *)surface->pixels;
	
	//Set the pixel
	pixels[(y * surface->w) + x] = pixel;
}

SDL_Surface* Texture::flip(SDL_Surface *surface, int flags)
// Frees input, allocates new output
{
	//Pointer to the soon to be flipped surface
	SDL_Surface *flipped = NULL;
	
	//If the image is color keyed

	flipped = SDL_CreateRGBSurface(SDL_SWSURFACE,
				surface->w, surface->h,
				surface->format->BitsPerPixel,
				surface->format->Rmask,
				surface->format->Gmask,
				surface->format->Bmask,
				surface->format->Amask
			);
	//If the surface must be locked
	if(SDL_MUSTLOCK(surface))
	{
		//Lock the surface
		SDL_LockSurface(surface);
	}
	//Go through columns
	for(int x = 0, rx = flipped->w - 1; x < flipped->w; x++, rx--)
	{
		//Go through rows
		for(int y = 0, ry = flipped->h - 1; y < flipped->h; y++, ry--)
		{
			//Get pixel
			Uint32 pixel = get_pixel32(surface, x, y);
			
			//Copy pixel
			if((flags & SDL_FLIP_VERTICAL) && (flags & SDL_FLIP_HORIZONTAL))
			{
				put_pixel32(flipped, rx, ry, pixel);
			}
			else if(flags & SDL_FLIP_HORIZONTAL)
			{
				put_pixel32(flipped, rx, y, pixel);
			}
			else if(flags & SDL_FLIP_VERTICAL)
			{
				put_pixel32(flipped, x, ry, pixel);
			}
		}
	}
	//Unlock surface
	if(SDL_MUSTLOCK(surface))
	{
		SDL_UnlockSurface(surface);
	}
	
	SDL_FreeSurface(surface);

	//Return flipped surface
	return flipped;
}

void Texture::setName(std::string name, int id)
{
	mGlslName = name;
	mGlslId = id;
}

std::string Texture::getName()
{
	return mName;
}

