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

void Map::init(const std::string& path)
{
	Log() << "Map: loading " << path;
	float left = -0.5;
	float top = -0.5;
	float right = 0.5;
	float bottom = 0.5;
	float down = 0;
	float up = 0.5;
	float stepW;
	float stepH;
	float stepU;

	mShader = Shader::getShader("model");

	attribute_coord3d = mShader->mkAttrib("coord3d");
	attribute_texcoord = mShader->mkAttrib("texcoord");
	attribute_vnorm = mShader->mkAttrib("vnorm");
	uniform_position = mShader->mkUniform("position");

	SDL_Surface* img = IMG_Load(path.c_str());
	if(!img)
		Log(Log::DIE) << "Map: Could not load file: " << path;

	std::vector<GLfloat> vert(img->w * img->h * 3);
	std::vector<GLfloat> tex(img->w * img->h * 2);
	std::vector<GLfloat> norm(img->w * img->h * 3);

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

			Log()
					<< " x = " << vert[cnt+0]
					<< " y = " << vert[cnt+1]
					<< " z = " << vert[cnt+2]
					;
			cnt += 3;
		}

	SDL_FreeSurface(img);

	cnt = 0;
	for(int h = 0; h < img->h; h++)
		for(int w = 0; w < img->w; w++)
		{
			tex[cnt+0] = 1.0*h/(img->h-1);
			tex[cnt+1] = 1.0*w/(img->w-1);

			Log()
					<< " u = " << tex[cnt+0]
					<< " v = " << tex[cnt+1]
					;
			cnt += 2;
		}

	// TODO Fix the last lines
	cnt = 0;
	for(int h = 0; h < img->h-1; h++)
		for(int w = 0; w < img->w-1; w++)
		{
			// TODO If wrong direction, change - to +

			float a[3] = {vert[cnt]-vert[cnt+3], vert[cnt+1]-vert[cnt+1+3], vert[cnt+2]-vert[cnt+2+3]};
			float b[3] = {vert[cnt]-vert[cnt+img->w], vert[cnt+1]-vert[cnt+1+img->w], vert[cnt+2]-vert[cnt+2+img->w]};
			// Log() << "a = " << a[0] << " " << a[1] << " " << a[2] << " ";
			// Log() << "b = " << b[0] << " " << b[1] << " " << b[2] << " ";

			norm[cnt+0] = (a[1]*b[2] - a[2]*b[1]);
			norm[cnt+1] = (a[2]*b[0] - a[0]*b[2]);
			norm[cnt+2] = (a[0]*b[1] - a[1]*b[0]);
			
			float len = sqrt(norm[cnt+0]*norm[cnt+0] + norm[cnt+1]*norm[cnt+1] + norm[cnt+2]*norm[cnt+2]);

			norm[cnt+0] /= len;
			norm[cnt+1] /= len;
			norm[cnt+2] /= len;

			// Log() << "norm = " << norm[cnt+0] << " " << norm[cnt+1] << " " << norm[cnt+2] << " " << "len = " << len;

			cnt += 3;
		}

	for(int h = 0; h < img->h-1; h++)
	{
		for(int w = 0; w < img->w-1; w++)
		{
				Log() << "norm = " << (float)norm[(h*img->w+w)*3+0] << " "
					<< (float)norm[(h*img->w+w)*3+1] << " "
					<< (float)norm[(h*img->w+w)*3+2] << " "
					;
		}
	}



	std::vector<GLushort> faces((img->w-1) * (img->h-1)*6*3);
	for(int h = 0; h < img->h-1; h++)
		for(int w = 0; w < img->w-1; w++)
		{
			faces[(h*img->w + w)*6  ] = h*img->w + w;
			faces[(h*img->w + w)*6+1] = h*img->w + w+1;
			faces[(h*img->w + w)*6+2] = (h+1)*img->w + w;

			faces[(h*img->w + w)*6+3] = h*img->w + w+1;
			faces[(h*img->w + w)*6+4] = (h+1)*img->w + w+1;
			faces[(h*img->w + w)*6+5] = (h+1)*img->w + w;

			assert((h*img->w + w)*6+5 < faces.size());
			assert((h+1)*img->w + w+1 <= vert.size()/3);
		}

	// for(int h = 0; h < img->h-1; h++)
	// {
	// 	for(int w = 0; w < img->w-1; w++)
	// 	{
	// 		for(int i = 0; i < 6; i++)
	// 			Log() << (h*img->w+w)*6+i << " = " << (int)faces[(h*img->w+w)*6+i] << " ";
    //
	// 		Log() << "---";
	// 	}
	// 	Log() << "+++";
	// }
	// Log() << "faces size = " << (int)faces.size();


	glGenBuffers(1, &vboVert);
	glBindBuffer(GL_ARRAY_BUFFER, vboVert);
	glBufferData(GL_ARRAY_BUFFER, vert.size()*sizeof(GLfloat), vert.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &vboTex);
	glBindBuffer(GL_ARRAY_BUFFER, vboTex);
	glBufferData(GL_ARRAY_BUFFER, tex.size()*sizeof(GLfloat), tex.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &vboVNorm);
	glBindBuffer(GL_ARRAY_BUFFER, vboVNorm);
	glBufferData(GL_ARRAY_BUFFER, norm.size()*sizeof(GLfloat), norm.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &iboElements);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboElements);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, faces.size()*sizeof(GLushort), faces.data(), GL_STATIC_DRAW);

	mTexture = Texture::getTexture("assets/tex.png");
}

// unsigned int setupFaceTriplet(const std::vector<GLfloat> &vert,
// 										const std::vector<GLfloat> &tex,
// 										const std::vector<GLfloat> &norm,
// 										int v,
// 										int t,
// 										int n,
// 										std::map<std::tuple<int, int, int>, int> &idx,
// 										std::vector<GLfloat> &out_vec
// 										)
// {
//
// }

void Map::setPosition(const glm::mat4 &position)
{
	mPosition = position;
}

void Map::paint()
{
	glUniformMatrix4fv(uniform_position, 1, GL_FALSE, glm::value_ptr(mPosition));

	mTexture->apply();

	glEnableVertexAttribArray(attribute_coord3d);
	glBindBuffer(GL_ARRAY_BUFFER, vboVert);

	glVertexAttribPointer(
		attribute_coord3d, // attribute
		3,                 // number of elements per vertex, here (x,y,z)
		GL_FLOAT,          // the type of each element
		GL_FALSE,          // take our values as-is
		0,                 // no extra data between each position
		0                  // offset of first element
	);

	glEnableVertexAttribArray(attribute_texcoord);
	glBindBuffer(GL_ARRAY_BUFFER, vboTex);
	glVertexAttribPointer(
		attribute_texcoord, // attribute
		2,                  // number of elements per vertex, here (x,y)
		GL_FLOAT,           // the type of each element
		GL_FALSE,           // take our values as-is
		0,                  // no extra data between each position
		0                   // offset of first element
	);

	glEnableVertexAttribArray(attribute_vnorm);
	glBindBuffer(GL_ARRAY_BUFFER, vboVNorm);
	glVertexAttribPointer(
		attribute_vnorm, // attribute
		3,                  // number of elements per vertex, here (x,y)
		GL_FLOAT,           // the type of each element
		GL_FALSE,           // take our values as-is
		0,                  // no extra data between each position
		0                   // offset of first element
	);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboElements);
	int size;
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
	glDrawElements(GL_TRIANGLES, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

	glDisableVertexAttribArray(attribute_vnorm);
	glDisableVertexAttribArray(attribute_texcoord);
	glDisableVertexAttribArray(attribute_coord3d);
}

