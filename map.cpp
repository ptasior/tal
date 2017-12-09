#include "map.h"
#include "log.h"
#include "shader.h"
#include "texture.h"
#include <SDL2/SDL_image.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>


unsigned char pixel(SDL_Surface *surface, int x, int y)
{
	Uint32 *pixels = (Uint32 *)surface->pixels;
	Uint32 rgba = pixels[(y * surface->w) + x];
	return (rgba >> 8) & 0xff; // Only b channel
}

void Map::init(const std::string path, const std::string texture)
{
	// TODO Optimize it. But then, it's run only once at start, so probably later...

	Log() << "Map: loading " << path;
	float stepW;
	float stepH;
	float stepU;

	mShader = Shader::getShader("map");

	mAttrVert = mShader->attrib("coord3d");
	mAttrTex = mShader->attrib("texcoord");
	mAttrNorm = mShader->attrib("vnorm");

	SDL_Surface* img = IMG_Load(path.c_str());
	if(!img)
		Log(Log::DIE) << "Map: Could not load file: " << path;

	std::vector<GLfloat> vert(img->w * img->h * 3);
	std::vector<GLfloat> tex(img->w * img->h * 2);
	std::vector<GLfloat> norm(img->w * img->h * 3);

	stepW = (right-left)/img->w;
	stepH = (bottom-top)/img->h;
	stepU = up-down;

	// Vertices
	unsigned int cnt = 0;
	for(int h = 0; h < img->h; h++)
		for(int w = 0; w < img->w; w++)
		{
			vert[cnt+0] = h*stepH+left;
			vert[cnt+1] = (1.0*pixel(img, w, h)/255*stepU+down);
			vert[cnt+2] = w*stepW+top;
			cnt += 3;
		}

	// Textures
	cnt = 0;
	for(int h = 0; h < img->h; h++)
		for(int w = 0; w < img->w; w++)
		{
			tex[cnt+0] = 10.0*h/(img->h-1);
			tex[cnt+1] = 10.0*w/(img->w-1);
			cnt += 2;
		}

	// Normals for the first triangle of the rect
	for(int h = 0; h < img->h-1; h++)
		for(int w = 0; w < img->w-1; w++)
		{
			unsigned int p = h*img->w+w;
			float b[3] = {vert[(p+img->w)*3]-vert[p*3], vert[(p+img->w)*3+1]-vert[p*3+1], vert[(p+img->w)*3+2]-vert[p*3+2]};
			float a[3] = {vert[(p+1)*3]-vert[p*3], vert[(p+1)*3+1]-vert[p*3+1], vert[(p+1)*3+2]-vert[p*3+2]};

			float v1 = (a[1]*b[2] - a[2]*b[1]);
			float v2 = (a[2]*b[0] - a[0]*b[2]);
			float v3 = (a[0]*b[1] - a[1]*b[0]);

			norm[p*3+0] += v1;
			norm[p*3+1] += v2;
			norm[p*3+2] += v3;

			norm[(p+1)*3+0] += v1;
			norm[(p+1)*3+1] += v2;
			norm[(p+1)*3+2] += v3;

			norm[(p+img->w)*3+0] += v1;
			norm[(p+img->w)*3+1] += v2;
			norm[(p+img->w)*3+2] += v3;
		}

	// Normals for the second triangle of the rect
	for(int h = 1; h < img->h; h++)
		for(int w = 1; w < img->w; w++)
		{
			unsigned int p = h*img->w+w;
			float b[3] = {vert[p*3]-vert[(p-img->w)*3], vert[p*3+1]-vert[(p-img->w)*3+1], vert[p*3+2]-vert[(p-img->w)*3+2]};
			float a[3] = {vert[p*3]-vert[(p-1)*3],      vert[p*3+1]-vert[(p-1)*3+1],      vert[p*3+2]-vert[(p-1)*3+2]};

			float v1 = (a[1]*b[2] - a[2]*b[1]);
			float v2 = (a[2]*b[0] - a[0]*b[2]);
			float v3 = (a[0]*b[1] - a[1]*b[0]);

			norm[p*3+0] += v1;
			norm[p*3+1] += v2;
			norm[p*3+2] += v3;

			norm[(p-1)*3+0] += v1;
			norm[(p-1)*3+1] += v2;
			norm[(p-1)*3+2] += v3;

			norm[(p-img->w)*3+0] += v1;
			norm[(p-img->w)*3+1] += v2;
			norm[(p-img->w)*3+2] += v3;
		}

	// Normalize all vectors
	cnt = 0;
	for(int h = 0; h < img->h; h++)
		for(int w = 0; w < img->w; w++)
		{
			float len = sqrt(norm[cnt*3+0]*norm[cnt*3+0] + norm[cnt*3+1]*norm[cnt*3+1] + norm[cnt*3+2]*norm[cnt*3+2]);

			norm[cnt*3+0] /= len;
			norm[cnt*3+1] /= len;
			norm[cnt*3+2] /= len;

			cnt++;
		}

	std::vector<GLushort> faces((img->w-1) * (img->h-1)*6);

	unsigned int line;
	unsigned int imgw = (img->w-1);
	for(int h = 0; h < img->h-1; h++)
		for(int w = 0; w < img->w-1; w++)
		{
			line = h*imgw;

			faces[(line + w)*6  ] = h*img->w + w;
			faces[(line + w)*6+1] = h*img->w + w+1;
			faces[(line + w)*6+2] = (h+1)*img->w + w;

			faces[(line + w)*6+3] = h*img->w + w+1;
			faces[(line + w)*6+4] = (h+1)*img->w + w+1;
			faces[(line + w)*6+5] = (h+1)*img->w + w;
		}


	glGenBuffers(1, &vboVert);
	glBindBuffer(GL_ARRAY_BUFFER, vboVert);
	glBufferData(GL_ARRAY_BUFFER, vert.size()*sizeof(GLfloat), vert.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &vboTex);
	glBindBuffer(GL_ARRAY_BUFFER, vboTex);
	glBufferData(GL_ARRAY_BUFFER, tex.size()*sizeof(GLfloat), tex.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &vboNorm);
	glBindBuffer(GL_ARRAY_BUFFER, vboNorm);
	glBufferData(GL_ARRAY_BUFFER, norm.size()*sizeof(GLfloat), norm.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &iboElements);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboElements);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, faces.size()*sizeof(GLushort), faces.data(), GL_STATIC_DRAW);

	SDL_FreeSurface(img);
	mTexture = Texture::getTexture(texture.c_str());
}

void Map::setPosition(const glm::mat4 &position)
{
	mPosition = position;
}

void Map::paint()
{
	mShader->use();
	// glUniformMatrix4fv(mUniformPosition, 1, GL_FALSE, glm::value_ptr(mPosition));
	mShader->setUniform("position", {glm::value_ptr(mPosition)});

	glEnableVertexAttribArray(mAttrVert);
	glBindBuffer(GL_ARRAY_BUFFER, vboVert);

	glVertexAttribPointer(
		mAttrVert, // attribute
		3,                 // number of elements per vertex, here (x,y,z)
		GL_FLOAT,          // the type of each element
		GL_FALSE,          // take our values as-is
		0,                 // no extra data between each position
		0                  // offset of first element
	);

	glEnableVertexAttribArray(mAttrTex);
	glBindBuffer(GL_ARRAY_BUFFER, vboTex);
	glVertexAttribPointer(
		mAttrTex, // attribute
		2,                  // number of elements per vertex, here (x,y)
		GL_FLOAT,           // the type of each element
		GL_FALSE,           // take our values as-is
		0,                 // no extra data between each position
		0                  // offset of first element
	);

	glEnableVertexAttribArray(mAttrNorm);
	glBindBuffer(GL_ARRAY_BUFFER, vboNorm);
	glVertexAttribPointer(
		mAttrNorm, // attribute
		3,                  // number of elements per vertex, here (x,y)
		GL_FLOAT,           // the type of each element
		GL_FALSE,           // take our values as-is
		0,                 // no extra data between each position
		0                  // offset of first element
	);

	mTexture->apply();

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboElements);
	int size;
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
	glDrawElements(GL_TRIANGLES, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

	glDisableVertexAttribArray(mAttrNorm);
	glDisableVertexAttribArray(mAttrTex);
	glDisableVertexAttribArray(mAttrVert);
}

void Map::setRect(double l, double t, double r, double b, double d, double u)
{
	left = l;
	top = t;
	right = r;
	bottom = b;
	down = d;
	up = u;
}

