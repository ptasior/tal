#include <SDL2/SDL_image.h>
#ifdef __EMSCRIPTEN__
	#include <SDL_ttf.h>
#else
	#include <SDL2/SDL_ttf.h>
#endif
#include <iostream>
#include <map>


const int SIZE = 80;
const int GRID = 16;
const int OUTLINE_SIZE = 1;

std::map<std::string, TTF_Font*> fonts;
std::map<std::string, TTF_Font*> fontsOtl;

struct Overwrite
{
	int x;
	int y;
	short unsigned int txt;
	const char * font;
};

Overwrite overwrites[] = {
	{1, 0, 0x25b2, "Hack.ttf"},
	{2, 0, 0x25bc, "Hack.ttf"},
	{3, 0, 0x25c0, "Hack.ttf"},
	{4, 0, 0x25b6, "Hack.ttf"},
	{5, 0, 0x23f8, "Symbola.ttf"},
	{6, 0, 0x23f9, "Symbola.ttf"},
	{7, 0, 0x2b6e, "Symbola.ttf"},
	{8, 0, 0x2b6f, "Symbola.ttf"},
	{9, 0, 0x2328, "Symbola.ttf"},
	{10, 0, 0x2610, "Symbola.ttf"},
	{11, 0, 0x2611, "Symbola.ttf"},
	{12, 0, 0x2612, "Symbola.ttf"},
	{0, 1, 0x2680, "Symbola.ttf"},
	{1, 1, 0x2681, "Symbola.ttf"},
	{2, 1, 0x2682, "Symbola.ttf"},
	{3, 1, 0x2683, "Symbola.ttf"},
	{4, 1, 0x2684, "Symbola.ttf"},
	{5, 1, 0x2685, "Symbola.ttf"},
};

void initFont(const char* fontName)
{
	if(fonts.count(fontName))
		return;

	TTF_Font* font = TTF_OpenFont(fontName, SIZE);
	TTF_Font* fontOutline = TTF_OpenFont(fontName, SIZE);
	TTF_SetFontOutline(fontOutline, OUTLINE_SIZE); 

	if(!font || !fontOutline)
	{
		std::cerr << "Cannot initaialise specific font " << TTF_GetError() << std::endl;
		exit(-1);
	}

	fonts[fontName] = font;
	fontsOtl[fontName] = fontOutline;
}

void destroyFonts()
{
	for(auto i:fonts)
		TTF_CloseFont(i.second);

	for(auto i:fontsOtl)
		TTF_CloseFont(i.second);
}

void render(const char* fontName, int x, int y, short unsigned text, SDL_Surface* res_texture)
{
	initFont(fontName);

	// TTF_SetFontHinting(font, TTF_HINTING_MONO);
	// TTF_SetFontHinting(font, TTF_HINTING_LIGHT);
	// TTF_SetFontHinting(font, TTF_HINTING_NONE);

	SDL_Color white = {0xFF, 0xFF, 0xFF}; 
	SDL_Color black = {0x55, 0x55, 0x55}; 
	// SDL_Color black = {0x00, 0x00, 0x00}; 

	std::cout << "Rendering " << text << " x: " << x << " y: " << y << std::endl;

	// SDL_Surface *bg_surface = TTF_RenderText_Blended(fontsOtl[fontName], text, black); 
	// SDL_Surface *fg_surface = TTF_RenderText_Blended(fonts[fontName], text, white); 

	SDL_Surface *bg_surface = TTF_RenderGlyph_Blended(fontsOtl[fontName], text, black); 
	SDL_Surface *fg_surface = TTF_RenderGlyph_Blended(fonts[fontName], text, white); 

	if(!bg_surface || !fg_surface)
	{
		std::cerr << "Error in TTF_RenderText_Blended: " << SDL_GetError() << std::endl;
		return;
	}

	SDL_Rect rect = {OUTLINE_SIZE, OUTLINE_SIZE, fg_surface->w, fg_surface->h}; 

	SDL_SetSurfaceBlendMode(fg_surface, SDL_BLENDMODE_BLEND); 
	SDL_BlitSurface(fg_surface, NULL, bg_surface, &rect); 
	SDL_FreeSurface(fg_surface);

	SDL_Rect rectS = {0, 0, bg_surface->w, bg_surface->h};
	SDL_Rect rectD = {SIZE*x, SIZE*y, SIZE, SIZE};
	SDL_BlitScaled(bg_surface, &rectS, res_texture, &rectD);

	SDL_FreeSurface(bg_surface);
}

bool applyOverwrite(int x, int y, SDL_Surface* res_texture)
{
	for(auto o : overwrites)
		if(o.x == x && o.y == y)
		{
			render(o.font, x, y, o.txt, res_texture);
			return true;
		}

	return false;
}

int main(int, char**)
{
	SDL_Init(SDL_INIT_VIDEO);

	if(TTF_Init()==-1) {
		std::cerr << "TTF_Init: " <<  TTF_GetError();
		return -1;
	}

	SDL_Surface* res_texture = SDL_CreateRGBSurface(SDL_SWSURFACE, SIZE*GRID, SIZE*GRID,
				32, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff
			);

	unsigned char cnt = 0;

	for(int j = 0; j < GRID; j++)
		for(int i = 0; i < GRID; i++, cnt++)
		{
			if(applyOverwrite(i, j, res_texture)) continue;
			if(!cnt) continue;

			if(cnt < 128)
				render("SVBasicManual.ttf", i, j, cnt, res_texture);
			else
				render("SVBasicManual-Bold.ttf", i, j, cnt-128, res_texture);
		}

	IMG_SavePNG(res_texture, "font.png");
	std::cout << "Saved as ./font.png" << std::endl;

	destroyFonts();

	return 0;
}

