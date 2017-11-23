#include "texture.h"
#include "log.h"
#include <SDL2/SDL_image.h>
#ifdef __EMSCRIPTEN__
	#include <SDL_ttf.h>
#else
	#include <SDL2/SDL_ttf.h>
#endif


std::mutex Texture::mMutex;
std::map<std::string, std::shared_ptr<Texture>> Texture::mList;

SDL_Surface* flip(SDL_Surface *surface, int flags);
Uint32 get_pixel32(SDL_Surface *surface, int x, int y);
void put_pixel32(SDL_Surface *surface, int x, int y, Uint32 pixel);

const int SIZE = 64;

Texture::Texture()
{
}

Texture::~Texture()
{
	glDeleteTextures(1, &texture_id);
}

void Texture::init(const char *path)
{
	Log() << "Loading texture " << path;
	mName = path;
	SDL_Surface* res_texture;
	if(mName.substr(0, 7) != "letter-")
	{
		res_texture = IMG_Load(path);
		if(!res_texture)
			Log(Log::DIE) << "IMG_Load: " << SDL_GetError();
	}
	else
	{
		// TODO mutex
		static TTF_Font* font = TTF_OpenFont("assets/Hack.ttf", SIZE);

		if(!font)
			Log(Log::DIE) << "Cannot initaialise specific font " << TTF_GetError();
		SDL_Color textColor = {255, 255, 255, 255}; // white

		SDL_Surface *rt  = TTF_RenderText_Blended(font, mName.substr(7).c_str(), textColor);
		if(!rt)
			Log(Log::DIE) << "Error in TTF_RenderText_Blended: " << SDL_GetError();

		// rt = flip(rt, SDL_FLIP_VERTICAL);

		res_texture = SDL_CreateRGBSurface(SDL_SWSURFACE, SIZE, SIZE,
				32, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff
			);
		SDL_Rect rect = {0, 0, SIZE, SIZE};

		SDL_BlitSurface(rt, NULL, res_texture, &rect);
		SDL_FreeSurface(rt);

		// TTF_CloseFont(font);
	}

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
			res_texture->pixels
		);
	SDL_FreeSurface(res_texture);
}

void Texture::apply()
{
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(uniform_mytexture, /*GL_TEXTURE*/0);
	glBindTexture(GL_TEXTURE_2D, texture_id);
}

std::shared_ptr<Texture> Texture::getTexture(const char* path)
{
	std::lock_guard<std::mutex> lock(mMutex);
	if(!mList.count(path))
	{
		mList[path].reset(new Texture);
		mList[path]->init(path);
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

SDL_Surface* flip(SDL_Surface *surface, int flags)
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

