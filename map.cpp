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
	float left = -2;
	float top = -2;
	float right = 2;
	float bottom = 2;
	float down = 0;
	float up = 2;
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

	// std::vector<GLfloat> vert(img->w * img->h * 3);
	// std::vector<GLfloat> tex(img->w * img->h * 2);
	// std::vector<GLfloat> norm(img->w * img->h * 3);
    //
	// stepW = (right-left)/img->w;
	// stepH = (bottom-top)/img->h;
	// stepU = up-down;
    //
	// int cnt = 0;
	// for(int h = 0; h < img->h; h++)
	// 	for(int w = 0; w < img->w; w++)
	// 	{
	// 		vert[cnt+0] = h*stepH+left;
	// 		vert[cnt+1] = 0;//(1.0*pixel(img, w, h)/255*stepU+down);
	// 		vert[cnt+2] = w*stepW+top;
    //
	// 		Log()
	// 				<< " x = " << vert[cnt+0]
	// 				<< " y = " << vert[cnt+1]
	// 				<< " z = " << vert[cnt+2]
	// 				;
	// 		cnt += 3;
	// 	}
    //
	// SDL_FreeSurface(img);
    //
	// cnt = 0;
	// for(int h = 0; h < img->h; h++)
	// 	for(int w = 0; w < img->w; w++)
	// 	{
	// 		tex[cnt+0] = 1.0*h/img->h;
	// 		tex[cnt+1] = 1.0*w/img->w;
    //
	// 		Log()
	// 				<< " u = " << tex[cnt+0]
	// 				<< " v = " << tex[cnt+1]
	// 				;
	// 		cnt += 2;
	// 	}
    //
	// // TODO Fix the last lines
	// cnt = 0;
	// for(int h = 0; h < img->h-2; h++)
	// 	for(int w = 0; w < img->w-2; w++)
	// 	{
	// 		// TODO If wrong direction, change - to +
    //
	// 		float a[3] = {vert[cnt]-vert[cnt+3], vert[cnt+1]-vert[cnt+1+3], vert[cnt+2]-vert[cnt+2+3]};
	// 		float b[3] = {vert[cnt]-vert[cnt+img->w], vert[cnt+1]-vert[cnt+1+img->w], vert[cnt+2]-vert[cnt+2+img->w]};
	// 		norm[cnt+0] = (a[1]*b[2] - a[2]*b[1]);
	// 		norm[cnt+1] = (a[2]*b[0] - a[0]*b[2]);
	// 		norm[cnt+2] = (a[0]*b[1] - a[1]*b[0]);
    //
	// 		cnt += 3;
	// 	}
    //
    //
	// std::vector<GLuint> faces((img->w-1) * (img->h-1)*6);/#<{(|3);
	// for(int h = 0; h < img->h-1; h++)
	// 	for(int w = 0; w < img->w-1; w++)
	// 	{
	// 		faces[(h*img->w + w)*6  ] = h*img->w + w;
	// 		faces[(h*img->w + w)*6+1] = h*img->w + w+1;
	// 		faces[(h*img->w + w)*6+2] = (h+1)*img->w + w;
    //
	// 		faces[(h*img->w + w)*6+3] = h*img->w + w+1;
	// 		faces[(h*img->w + w)*6+4] = (h+1)*img->w + w+1;
	// 		faces[(h*img->w + w)*6+5] = (h+1)*img->w + w;
    //
	// 		assert((h*img->w + w)*6+5 < faces.size());
	// 		assert((h+1)*img->w + w+1 <= vert.size()/3);
	// 	}
    //
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


	std::vector<GLfloat> vert = 
	{
		-1.0, -1.0,  0.0,
		 1.0, -1.0,  0.0,
		 1.0,  1.0,  0.0,
		-1.0,  1.0,  0.0
	};

	std::vector<GLfloat> norm = 
	{
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
	};

	std::vector<GLfloat> tex = 
	{
		0.0, 0.0,
		1.0, 0.0,
		1.0, 1.0,
		0.0, 1.0,
	};

	std::vector<GLuint> faces = 
	{
		0,  1,  2,
		2,  3,  0
	};


	glGenBuffers(1, &vboVert);
	glBindBuffer(GL_ARRAY_BUFFER, vboVert);
	glBufferData(GL_ARRAY_BUFFER, vert.size(), vert.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &vboTex);
	glBindBuffer(GL_ARRAY_BUFFER, vboTex);
	glBufferData(GL_ARRAY_BUFFER, tex.size(), tex.data(), GL_STATIC_DRAW);

	// glGenBuffers(1, &vboVNorm);
	// glBindBuffer(GL_ARRAY_BUFFER, vboVNorm);
	// glBufferData(GL_ARRAY_BUFFER, norm.size(), norm.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &iboElements);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboElements);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, faces.size(), faces.data(), GL_STATIC_DRAW);


	mTexture = Texture::getTexture("assets/tex.png");
}

void Map::setPosition(const glm::mat4 &position)
{
	mPosition = position;
}

void Map::paint()
{
	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
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

