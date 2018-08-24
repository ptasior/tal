#include "model_obj.h"
#include "log.h"
#include "shader.h"
#include "texture.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>



ModelObj::~ModelObj()
{
	if(!mShader) return;

	glDeleteBuffers(1, &vboVertices);

	for(auto a : mObjects)
		glDeleteBuffers(1, &a.first);
}

std::vector<GLfloat> ModelObj::readData(const char * pref, unsigned int lines, int values)
{
	std::vector<GLfloat> ret(lines * values);
	if(!lines) return ret;

	unsigned int cnt = 0;
	std::string buf;

	while(mFile.good())
	{
		mFile >> buf;
		if(buf != pref)
		{
			mFile.ignore(300, '\n');
			continue;
		}

		if(values == 3)
			mFile   >> ret[cnt*3]
					>> ret[cnt*3+1]
					>> ret[cnt*3+2];
		else
			mFile   >> ret[cnt*2]
					>> ret[cnt*2+1];

		if(++cnt == lines-1) break;
	}

	assert(ret.size() == lines * values);

	return ret;
}


unsigned int ModelObj::setupFaceTriplet(const std::vector<GLfloat> &vert,
										const std::vector<GLfloat> &tex,
										const std::vector<GLfloat> &norm,
										std::map<std::string, int> &idx,
										std::vector<GLfloat> &out_vec
										)
{
	unsigned int v, t, n;
	char c;

	mFile >> v;
	if(mTexCnt)
	{
		mFile >> c >> t;
		assert(c == '/');
	}
	if(mNormCnt)
	{
		if(!mTexCnt)
			mFile >> c;
		mFile >> c >> n;
		assert(c == '/');
	}

	std::string key = std::to_string(v)+"/"+std::to_string(t)+"/"+std::to_string(n);

	if(!idx.count(key))
	{
		assert(v-1 < mVertCnt*3);
		out_vec.push_back(vert[(v-1)*3]);
		out_vec.push_back(vert[(v-1)*3+1]);
		out_vec.push_back(vert[(v-1)*3+2]);
		if(mTexCnt)
		{
			assert(t-1 < mTexCnt*2);
			out_vec.push_back(tex[(t-1)*2]);
			out_vec.push_back(tex[(t-1)*2+1]);
		}

		if(mNormCnt)
		{
			assert(n-1 < mNormCnt*3);
			out_vec.push_back(norm[(n-1)*3]);
			out_vec.push_back(norm[(n-1)*3+1]);
			out_vec.push_back(norm[(n-1)*3+2]);
		}

		idx[key] = out_vec.size()/8-1;
	}

	return idx[key];
}

void ModelObj::init(const std::string path)
{
	Model::init(path);
	mDirectory = path.substr(0, path.find_last_of("/"));

	Log() << "ModelObj: Initalising " << path;
	assert(!mObjects.size()); // If not empty, make sure glDeleteBuffers is called

	mFile.open(path);
	if(!mFile.good())
		Log(Log::DIE) << "ModelObj: Cannot open model";

	readSize();
	mFile.clear();
	mFile.seekg(0, std::ios::beg);


	std::vector<GLfloat> raw_vert = readData("v", mVertCnt, 3);
	std::vector<GLfloat> raw_tex = readData("vt", mTexCnt, 2);
	std::vector<GLfloat> raw_norm = readData("vn", mNormCnt, 3);

	std::vector<std::vector<GLushort> > faces;
	std::vector<std::string> materials;
	if(!mTextures.size()) // If no textures used, add one object
		faces.push_back(std::vector<GLushort>());

	std::map<std::string, int> idx;
	std::vector<GLfloat> out_vec;

	unsigned int cnt = 0;
	int pos;
	std::string buf;

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
			pos = setupFaceTriplet(raw_vert, raw_tex, raw_norm, idx, out_vec);
			faces.back().push_back(pos);
		}

		if(++cnt == mFaceCnt-1) break;
	}

	glGenBuffers(1, &vboVertices);
	glBindBuffer(GL_ARRAY_BUFFER, vboVertices);
	glBufferData(GL_ARRAY_BUFFER, out_vec.size()*sizeof(GLfloat), out_vec.data(), GL_STATIC_DRAW);

	for(unsigned int i = 0; i < faces.size(); i++)
	{
		GLuint ibo;
		glGenBuffers(1, &ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, faces[i].size()*sizeof(GLushort), faces[i].data(), GL_STATIC_DRAW);

		mObjects[ibo] = Texture::getTexture((mDirectory+"/"+mTextures[materials[i]]).c_str(), mShader.get());
	}

	mFile.close();
	mTextures.clear(); // Not needed any more, clean
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
		else if(buf == "o") ;
		else if(buf == "g") ;
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
		Log(Log::DIE) << "ModelObj: Cannot open mtl file: " << mDirectory + "/" + path;

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
			for(unsigned int i = 0; i < tmp.length(); i++)
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
	mShader->use();
	mShader->setUniform("position", {glm::value_ptr(mPosition)});

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


	for(auto a : mObjects)
	{
		a.second->apply();

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, a.first);
		int size;
		glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
		glDrawElements(GL_TRIANGLES, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);
	}

	glDisableVertexAttribArray(mAttrTex);
	glDisableVertexAttribArray(mAttrVert);
	glDisableVertexAttribArray(mAttrNorm);
}

