#pragma once
#include "model.h"
#include <fstream>
#include <map>


class ModelObj : public Model
{
public:
	void load(const std::string& path);
	void paint();

private:
	void readSize();
	void readMtl(std::string &path);
	// std::vector<GLfloat> readData(const char * pref, unsigned int lines, int values);

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

