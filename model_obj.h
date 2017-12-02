#pragma once
#include "model.h"
#include <fstream>


class ModelObj : public Model
{
public:
	void load();
	void paint();

private:
	void readSize();

	int mVertCnt = 0;
	int mTexCnt = 0;
	int mNormCnt = 0;
	int mFaceCnt = 0;

	std::ifstream mFile;
};

