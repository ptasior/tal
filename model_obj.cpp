#include "model_obj.h"
#include "log.h"
#include "shader.h"
#include "texture.h"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>


void ModelObj::load()
{
	Log() << "Initalising ModelObj";
	mFile.open("assets/3d/test.obj");
	if(mFile.fail()) Log(Log::DIE) << "Canot open model";

	readSize();
	mFile.clear();
	mFile.seekg(0, std::ios::beg);

	char buf[3];

	std::vector<GLfloat> raw_vert(mVertCnt *3);
	int cnt = 0;
	while(mFile.good())
	{
		mFile >> buf;
		if(buf[0] != 'v' || buf[1] != '\0')
		{
			mFile.ignore(100, '\n');
			continue;
		}

		mFile   >> raw_vert[cnt*3]
				>> raw_vert[cnt*3+1]
				>> raw_vert[cnt*3+2];

		if(++cnt == mVertCnt) break;
	}

	for(int i = 0; i < mVertCnt; i++)
		Log() << "v " << raw_vert[i*3] << " "
				<< raw_vert[i*3+1] << " "
				<< raw_vert[i*3+2];


	std::vector<GLfloat> raw_tex(mTexCnt *2);
	cnt = 0;
	while(mFile.good())
	{
		mFile >> buf;
		if(buf[0] != 'v' || buf[1] != 't')
		{
			mFile.ignore(100, '\n');
			continue;
		}

		mFile   >> raw_tex[cnt*2]
				>> raw_tex[cnt*2+1];

		if(++cnt == mTexCnt) break;
	}

	for(int i = 0; i < mTexCnt; i++)
		Log() << "vt " << raw_tex[i*2] << " "
				<< raw_tex[i*2+1];


	std::vector<GLfloat> raw_norm(mNormCnt *3);
	cnt = 0;
	while(mFile.good())
	{
		mFile >> buf;
		if(buf[0] != 'v' || buf[1] != 'n')
		{
			mFile.ignore(100, '\n');
			continue;
		}

		mFile   >> raw_norm[cnt*3]
				>> raw_norm[cnt*3+1]
				>> raw_norm[cnt*3+2];

		if(++cnt == mNormCnt) break;
	}

	for(int i = 0; i < mNormCnt; i++)
		Log() << "vn " << raw_norm[i*3] << " "
				<< raw_norm[i*3+1] << " "
				<< raw_norm[i*3+2];



	std::vector<GLushort> faces(mFaceCnt *3);
	std::map<std::string, int> idx;
	std::vector<GLfloat> out_vec;

	cnt = 0;
	char c;
	int v, t, n;
	std::string key;
	while(mFile.good())
	{
		mFile >> buf;
		if(buf[0] != 'f' || buf[1] != '\0')
		{
			mFile.ignore(100, '\n');
			continue;
		}

		for(int i = 0; i < 3; i++)
		{
			mFile >> v >> c >> t >> c >> n;

			key = std::to_string(v)+"/"+std::to_string(t)+"/"+std::to_string(n);

			Log() << cnt << " - face " << key;
			if(!idx.count(key))
			{
				out_vec.push_back(raw_vert[(v-1)*3]);
				out_vec.push_back(raw_vert[(v-1)*3+1]);
				out_vec.push_back(raw_vert[(v-1)*3+2]);
				out_vec.push_back(raw_tex[(t-1)*2]);
				out_vec.push_back(raw_tex[(t-1)*2+1]);
				out_vec.push_back(raw_norm[(n-1)*3]);
				out_vec.push_back(raw_norm[(n-1)*3+1]);
				out_vec.push_back(raw_norm[(n-1)*3+2]);

				int pos = out_vec.size()/8-1;
				Log() << "adding " << pos << " cnt = " << cnt << " vec size= " << (int)out_vec.size();
				idx[key] = pos;
				faces[cnt] = pos;
				// faces[cnt] = idx[key] = out_vec.size()/8;
			}
			else
			{
				Log() << "using " << idx[key];
				faces[cnt] = idx[key];
			}
			cnt++;
		}

		if(cnt == mFaceCnt*3) break;
	}

	for(int i = 0; i < mFaceCnt*3; i++)
		Log() << "face " << i << " = " << faces[i];

	glGenBuffers(1, &vbo_cube_vertices);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_cube_vertices);
	glBufferData(GL_ARRAY_BUFFER, out_vec.size()*sizeof(GLfloat), &out_vec[0], GL_STATIC_DRAW);

	glGenBuffers(1, &ibo_cube_elements);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_cube_elements);
	// glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3*sizeof(GLushort), &faces[0], GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, faces.size()*sizeof(GLushort), &faces[0], GL_STATIC_DRAW);

	mTexture = Texture::getTexture("assets/cloth.png");
}

void ModelObj::readSize()
{
	char buf[100];

	while(mFile.good())
	{
		mFile.getline(buf, 100);
		if(buf[0] == 'v' && buf[1] == ' ') mVertCnt++;
		if(buf[0] == 'v' && buf[1] == 't') mTexCnt++;
		if(buf[0] == 'v' && buf[1] == 'n') mNormCnt++;
		if(buf[0] == 'f' && buf[1] == ' ') mFaceCnt++;
	}

	Log() << "Model Obj; vert: " << mVertCnt
			<< " tex: " << mTexCnt
			<< " norm: " << mNormCnt
			<< " face: " << mFaceCnt;
}

void ModelObj::paint()
{
	// mShader->use();
	//
	glUniformMatrix4fv(uniform_position, 1, GL_FALSE, glm::value_ptr(mPosition));

	mTexture->apply();

	glEnableVertexAttribArray(attribute_coord3d);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_cube_vertices);

	glVertexAttribPointer(
		attribute_coord3d, // attribute
		3,                 // number of elements per vertex, here (x,y,z)
		GL_FLOAT,          // the type of each element
		GL_FALSE,          // take our values as-is
		8*sizeof(GLfloat),                 // no extra data between each position
		0                  // offset of first element
	);

	glEnableVertexAttribArray(attribute_texcoord);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_cube_vertices);
	glVertexAttribPointer(
		attribute_texcoord, // attribute
		2,                  // number of elements per vertex, here (x,y)
		GL_FLOAT,           // the type of each element
		GL_FALSE,           // take our values as-is
		8*sizeof(GLfloat),                  // no extra data between each position
		(void *)(3*sizeof(GLfloat))                   // offset of first element
	);


	// Draw a triangle from the 3 vertices
	// glDrawArrays(GL_TRIANGLES, 0, 3);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_cube_elements);
	int size;
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
	glDrawElements(GL_TRIANGLES, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

	glDisableVertexAttribArray(attribute_texcoord);
	glDisableVertexAttribArray(attribute_coord3d);
}
