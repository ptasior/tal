#pragma once
#include "model.h"
#include <fstream>
#include <vector>
#include <map>


class ModelObj : public Model
{
public:
	void load(const std::string& path);
	void paint();

private:
	void readSize();
	void readMtl(std::string &path);
	std::vector<GLfloat> readData(const char * pref, unsigned int lines, int values);
	unsigned int setupFaceTriplet(const std::vector<GLfloat> &vert,
									const std::vector<GLfloat> &tex,
									const std::vector<GLfloat> &norm,
									std::map<std::string, int> &idx,
									std::vector<GLfloat> &out_vec
									);

	int mVertCnt = 0;
	int mTexCnt = 0;
	int mNormCnt = 0;
	int mFaceCnt = 0;

	GLuint vboVertices;

	std::ifstream mFile;
	std::string mDirectory;
	std::map<std::string, std::string> mTextures;

	std::map<GLuint, std::shared_ptr<Texture>> mObjects;
};

