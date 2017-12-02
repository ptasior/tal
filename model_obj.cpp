#include "model_obj.h"
#include "log.h"
#include "shader.h"
#include "texture.h"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>



// std::vector<GLfloat> ModelObj::readData(const char * pref, unsigned int lines, int values)
// {
// 	std::vector<GLfloat> ret(lines * values);
// 	int cnt = 0;
//
// 	while(mFile.good())
// 	{
// 		mFile >> buf;
// 		if(buf != pref)
// 		{
// 			mFile.ignore(300, '\n');
// 			continue;
// 		}
//
// 		if(values == 3)
// 			mFile   >> ret[cnt*3]
// 					>> ret[cnt*3+1]
// 					>> ret[cnt*3+2];
// 		else
// 			mFile   >> ret[cnt*2]
// 					>> ret[cnt*2+1];
//
// 		if(++cnt == lines) break;
// 	}
//
// 	return ret;
// }

void ModelObj::load(const std::string& path)
{
	mDirectory = path.substr(0, path.find_last_of("/"));

	Log() << "Initalising ModelObj " << path;

	mFile.open(path);
	if(!mFile.good())
		Log(Log::DIE) << "Cannot open model";

	readSize();
	mFile.clear();
	mFile.seekg(0, std::ios::beg);

	std::string buf;

	std::vector<GLfloat> raw_vert(mVertCnt *3);
	int cnt = 0;
	while(mFile.good())
	{
		mFile >> buf;
		if(buf != "v")
		{
			mFile.ignore(300, '\n');
			continue;
		}

		mFile   >> raw_vert[cnt*3]
				>> raw_vert[cnt*3+1]
				>> raw_vert[cnt*3+2];

		if(++cnt == mVertCnt) break;
	}

	std::vector<GLfloat> raw_tex(mTexCnt *2);
	cnt = 0;
	while(mFile.good())
	{
		mFile >> buf;
		if(buf != "vt")
		{
			mFile.ignore(300, '\n');
			continue;
		}

		mFile   >> raw_tex[cnt*2]
				>> raw_tex[cnt*2+1];

		if(++cnt == mTexCnt) break;
	}

	std::vector<GLfloat> raw_norm(mNormCnt *3);
	cnt = 0;
	while(mFile.good())
	{
		mFile >> buf;
		if(buf != "vn")
		{
			mFile.ignore(300, '\n');
			continue;
		}

		mFile   >> raw_norm[cnt*3]
				>> raw_norm[cnt*3+1]
				>> raw_norm[cnt*3+2];

		if(++cnt == mNormCnt) break;
	}



	std::vector<std::vector<GLushort> > faces;
	std::vector<std::string> materials;
	if(!mTextures.size()) // If no textures used, add one object
		faces.push_back(std::vector<GLushort>());

	std::map<std::string, int> idx;
	std::vector<GLfloat> out_vec;

	cnt = 0;
	char c;
	int v, t, n;
	std::string key;
	while(mFile.good())
	{
		mFile >> buf;
		if(buf == "usemtl")
		{
			faces.push_back(std::vector<GLushort>());
			mFile >> buf;
			materials.push_back(buf);
			continue;
		}

		if(buf != "f")
		{
			mFile.ignore(100, '\n');
			continue;
		}

		for(int i = 0; i < 3; i++)
		{
			mFile >> v >> c >> t >> c >> n;

			key = std::to_string(v)+"/"+std::to_string(t)+"/"+std::to_string(n);

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
				idx[key] = pos;
				faces.back().push_back(pos);
			}
			else
				faces.back().push_back(idx[key]);

			cnt++;
		}

		if(cnt == mFaceCnt*3) break;
	}

	glGenBuffers(1, &vboVertices);
	glBindBuffer(GL_ARRAY_BUFFER, vboVertices);
	glBufferData(GL_ARRAY_BUFFER, out_vec.size()*sizeof(GLfloat), &out_vec[0], GL_STATIC_DRAW);

	for(int i = 0; i < faces.size(); i++)
	{
		GLuint ibo;
		glGenBuffers(1, &ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, faces[i].size()*sizeof(GLushort), &faces[i][0], GL_STATIC_DRAW);

		mObjects[ibo] = Texture::getTexture((mDirectory+"/"+mTextures[materials[i]]).c_str());
	}

	mFile.close();
	mTextures.clear(); // TODO make local
}

void ModelObj::readSize()
{
	std::string buf;

	while(mFile.good())
	{
		mFile >> buf;

			 if(buf == "#") ;
		else if(buf == "v") mVertCnt++;
		else if(buf == "vt") mTexCnt++;
		else if(buf == "vn") mNormCnt++;
		else if(buf == "f") mFaceCnt++;
		else if(buf == "f") mFaceCnt++;
		else if(buf == "vp")
			Log(Log::DIE) << "ModelObj: 'vp' free-form geometry not supported";
		else if(buf == "mtllib")
		{
			mFile >> buf;
			readMtl(buf);
		}
		else if(buf == "usemtl")
		{
			mFile >> buf;
			if(!mTextures.count(buf))
				Log(Log::DIE) << "ModelObj: unknown mtl: " << buf;
		}
		else if(buf == "o")
			Log() << "ModelObj: o";
		else if(buf == "g")
			Log() << "ModelObj: g";
		else
			Log(Log::DIE) << "ModelObj: " << buf << " unsupported";

		mFile.ignore(100, '\n');
	}
}

void ModelObj::readMtl(std::string &path)
{
	Log() << "ModelObj: reading mtl file: " << path;

	std::ifstream file(mDirectory + "/" + path);

	if(!file.good())
		Log(Log::DIE) << "Cannot open mtl file: " << mDirectory + "/" + path;

	std::string buf;
	std::string name;

	while(file.good())
	{
		file >> buf;

		if(buf == "newmtl")
		{
			if(!name.empty())
				Log(Log::DIE) << "ModelObj: readMtl: second newmtl without map_Kd";
			file >> name;
		}
		else if(buf == "map_Kd")
		{
			std::string tmp;
			file >> tmp;
			for(int i = 0; i < tmp.length(); i++)
				if(tmp[i] == '\\')
					tmp[i] = '/';
			mTextures[name] = tmp;

			name = "";
		}
		else if(buf == "map_Kp")
			Log(Log::DIE) << "ModelObj: readMtl: model uses map_Kp texture";
		else if(buf == "map_Ka")
			Log(Log::DIE) << "ModelObj: readMtl: model uses map_Ka texture";

		file.ignore(100, '\n');
	}
}

void ModelObj::paint()
{
	glUniformMatrix4fv(uniform_position, 1, GL_FALSE, glm::value_ptr(mPosition));


	glEnableVertexAttribArray(attribute_coord3d);
	glBindBuffer(GL_ARRAY_BUFFER, vboVertices);

	glVertexAttribPointer(
		attribute_coord3d, // attribute
		3,                 // number of elements per vertex, here (x,y,z)
		GL_FLOAT,          // the type of each element
		GL_FALSE,          // take our values as-is
		8*sizeof(GLfloat),                 // no extra data between each position
		0                  // offset of first element
	);

	glEnableVertexAttribArray(attribute_texcoord);
	glBindBuffer(GL_ARRAY_BUFFER, vboVertices);
	glVertexAttribPointer(
		attribute_texcoord, // attribute
		2,                  // number of elements per vertex, here (x,y)
		GL_FLOAT,           // the type of each element
		GL_FALSE,           // take our values as-is
		8*sizeof(GLfloat),                  // no extra data between each position
		(void *)(3*sizeof(GLfloat))                   // offset of first element
	);

	// glEnableVertexAttribArray(attribute_texcoord);
	// glBindBuffer(GL_ARRAY_BUFFER, vboVertices);
	// glVertexAttribPointer(
	// 	attribute_vnorm, // attribute
	// 	2,                  // number of elements per vertex, here (x,y)
	// 	GL_FLOAT,           // the type of each element
	// 	GL_FALSE,           // take our values as-is
	// 	8*sizeof(GLfloat),                  // no extra data between each position
	// 	(void *)(5*sizeof(GLfloat))                   // offset of first element
	// );


	for(auto a : mObjects)
	{
		a.second->apply();

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, a.first);
		int size;
		glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
		glDrawElements(GL_TRIANGLES, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);
	}

	glDisableVertexAttribArray(attribute_texcoord);
	glDisableVertexAttribArray(attribute_coord3d);
}
