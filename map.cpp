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

void Map::init(const std::string path)
{
	// TODO Optimize it. But then, it's run only once at start, so probably later...

	Log() << "Map: loading " << path;
	float stepW;
	float stepH;
	float stepU;

	mShader = Shader::getShader("model");

	mAttrVert = mShader->mkAttrib("coord3d");
	mAttrTex = mShader->mkAttrib("texcoord");
	mAttrNorm = mShader->mkAttrib("vnorm");
	mUniformPosition = mShader->mkUniform("position");

	SDL_Surface* img = IMG_Load(path.c_str());
	if(!img)
		Log(Log::DIE) << "Map: Could not load file: " << path;

	std::vector<GLfloat> vert(img->w * img->h * 3);
	std::vector<GLfloat> tex(img->w * img->h * 2);
	std::vector<GLfloat> norm(img->w * img->h * 3 *2); // *2 for backward triangles

	stepW = (right-left)/img->w;
	stepH = (bottom-top)/img->h;
	stepU = up-down;

	int cnt = 0;
	for(int h = 0; h < img->h; h++)
		for(int w = 0; w < img->w; w++)
		{
			vert[cnt+0] = h*stepH+left;
			vert[cnt+1] = (1.0*pixel(img, w, h)/255*stepU+down);
			vert[cnt+2] = w*stepW+top;
			cnt += 3;
		}


	cnt = 0;
	for(int h = 0; h < img->h; h++)
		for(int w = 0; w < img->w; w++)
		{
			tex[cnt+0] = 1.0*h/(img->h-1);
			tex[cnt+1] = 1.0*w/(img->w-1);
			cnt += 2;
		}

	cnt = 0;
	for(int h = 0; h < img->h-1; h++)
		for(int w = 0; w < img->w-1; w++)
		{
			int p = h*img->w+w;
			float b[3] = {vert[(p+img->w)*3]-vert[p*3], vert[(p+img->w)*3+1]-vert[p*3+1], vert[(p+img->w)*3+2]-vert[p*3+2]};
			float a[3] = {vert[(p+1)*3]-vert[p*3], vert[(p+1)*3+1]-vert[p*3+1], vert[(p+1)*3+2]-vert[p*3+2]};

			norm[cnt*3+0] = (a[1]*b[2] - a[2]*b[1]);
			norm[cnt*3+1] = (a[2]*b[0] - a[0]*b[2]);
			norm[cnt*3+2] = (a[0]*b[1] - a[1]*b[0]);
			
			float len = sqrt(norm[cnt*3+0]*norm[cnt*3+0] + norm[cnt*3+1]*norm[cnt*3+1] + norm[cnt*3+2]*norm[cnt*3+2]);

			norm[cnt*3+0] /= len;
			norm[cnt*3+1] /= len;
			norm[cnt*3+2] /= len;

			assert(norm.size() > cnt*3+2);

			cnt++;
		}

	int norm2 = cnt;
	cnt = 0;

	for(int h = 1; h < img->h; h++)
		for(int w = 1; w < img->w; w++)
		{
			int p = h*img->w+w;
			float b[3] = {vert[p*3]-vert[(p-img->w)*3], vert[p*3+1]-vert[(p-img->w)*3+1], vert[p*3+2]-vert[(p-img->w)*3+2]};
			float a[3] = {vert[p*3]-vert[(p-1)*3],      vert[p*3+1]-vert[(p-1)*3+1],      vert[p*3+2]-vert[(p-1)*3+2]};

			norm[(cnt+norm2)*3+0] = (a[1]*b[2] - a[2]*b[1]);
			norm[(cnt+norm2)*3+1] = (a[2]*b[0] - a[0]*b[2]);
			norm[(cnt+norm2)*3+2] = (a[0]*b[1] - a[1]*b[0]);

			float len = sqrt(norm[(cnt+norm2)*3+0]*norm[(cnt+norm2)*3+0] + norm[(cnt+norm2)*3+1]*norm[(cnt+norm2)*3+1] + norm[(cnt+norm2)*3+2]*norm[(cnt+norm2)*3+2]);

			norm[(cnt+norm2)*3+0] /= len;
			norm[(cnt+norm2)*3+1] /= len;
			norm[(cnt+norm2)*3+2] /= len;

			cnt++;
		}

	std::map<std::tuple<int, int, int>, int> idx;
	std::vector<GLfloat> out_vec;
	std::vector<GLushort> faces((img->w-1) * (img->h-1)*6);

	int line;
	int imgw = (img->w-1);
	for(int h = 0; h < img->h-1; h++)
		for(int w = 0; w < img->w-1; w++)
		{
			line = h*imgw;
			faces[(line + w)*6  ] = setupFaceTriplet(vert, tex, norm,
								h*img->w + w, h*img->w + w, h*img->w + w-h,
								idx, out_vec);
			faces[(line + w)*6+1] = setupFaceTriplet(vert, tex, norm,
								h*img->w + w+1, h*img->w + w+1, h*img->w + w-h,
								idx, out_vec);
			faces[(line + w)*6+2] = setupFaceTriplet(vert, tex, norm,
								(h+1)*img->w + w, (h+1)*img->w + w, h*img->w + w-h,
								idx, out_vec);

			faces[(line + w)*6+3] = setupFaceTriplet(vert, tex, norm,
								h*img->w + w+1, h*img->w + w+1, h*img->w + w-h + norm2,
								idx, out_vec);
			faces[(line + w)*6+4] = setupFaceTriplet(vert, tex, norm,
								(h+1)*img->w + w+1, (h+1)*img->w + w+1, h*img->w + w-h + norm2,
								idx, out_vec);
			faces[(line + w)*6+5] = setupFaceTriplet(vert, tex, norm,
								(h+1)*img->w + w, (h+1)*img->w + w, h*img->w + w-h + norm2,
								idx, out_vec);

			assert((line + w)*6+5 < faces.size());
			assert((h+1)*img->w + w+1 <= vert.size()/3);
		}

	assert(faces.size() >= out_vec.size()/8);


	glGenBuffers(1, &vboVertices);
	glBindBuffer(GL_ARRAY_BUFFER, vboVertices);
	glBufferData(GL_ARRAY_BUFFER, out_vec.size()*sizeof(GLfloat), out_vec.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &iboElements);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboElements);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, faces.size()*sizeof(GLushort), faces.data(), GL_STATIC_DRAW);

	SDL_FreeSurface(img);
	mTexture = Texture::getTexture("assets/tex.png");
}

unsigned int Map::setupFaceTriplet(const std::vector<GLfloat> &vert,
										const std::vector<GLfloat> &tex,
										const std::vector<GLfloat> &norm,
										int v,
										int t,
										int n,
										std::map<std::tuple<int, int, int>, int> &idx,
										std::vector<GLfloat> &out_vec
									)
{
	auto key = std::make_tuple(v,n,t);
	if(idx.count(key)) return idx[key];

	assert(v*3+2 < vert.size());
	assert(t*2+1 < tex.size());
	assert(n*3+2 < norm.size());

	out_vec.push_back(vert[(v)*3]);
	out_vec.push_back(vert[(v)*3+1]);
	out_vec.push_back(vert[(v)*3+2]);
	out_vec.push_back(tex[(t)*2]);
	out_vec.push_back(tex[(t)*2+1]);
	out_vec.push_back(norm[(n)*3]);
	out_vec.push_back(norm[(n)*3+1]);
	out_vec.push_back(norm[(n)*3+2]);

	idx[key] = out_vec.size()/8-1;
	return idx[key];
}

void Map::setPosition(const glm::mat4 &position)
{
	mPosition = position;
}

void Map::paint()
{
	mShader->use();
	glUniformMatrix4fv(mUniformPosition, 1, GL_FALSE, glm::value_ptr(mPosition));

	glEnableVertexAttribArray(mAttrVert);
	glBindBuffer(GL_ARRAY_BUFFER, vboVertices);

	glVertexAttribPointer(
		mAttrVert, // attribute
		3,                 // number of elements per vertex, here (x,y,z)
		GL_FLOAT,          // the type of each element
		GL_FALSE,          // take our values as-is
		8*sizeof(GLfloat),                 // no extra data between each position
		0                  // offset of first element
	);

	glEnableVertexAttribArray(mAttrTex);
	// glBindBuffer(GL_ARRAY_BUFFER, vboVertices);
	glVertexAttribPointer(
		mAttrTex, // attribute
		2,                  // number of elements per vertex, here (x,y)
		GL_FLOAT,           // the type of each element
		GL_FALSE,           // take our values as-is
		8*sizeof(GLfloat),                  // no extra data between each position
		(void *)(3*sizeof(GLfloat))                   // offset of first element
	);

	glEnableVertexAttribArray(mAttrNorm);
	// glBindBuffer(GL_ARRAY_BUFFER, vboVertices);
	glVertexAttribPointer(
		mAttrNorm, // attribute
		3,                  // number of elements per vertex, here (x,y)
		GL_FLOAT,           // the type of each element
		GL_FALSE,           // take our values as-is
		8*sizeof(GLfloat),                  // no extra data between each position
		(void *)(5*sizeof(GLfloat))                   // offset of first element
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

