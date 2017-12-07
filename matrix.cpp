#include "matrix.h"

glm::mat4& Matrix::val()
{
	return mVal;
}

void Matrix::rotate(glm::vec3 rot)
{
	if(rot.x) mVal = glm::rotate(mVal, glm::radians(rot.x), glm::vec3(1.0, 0, 0));
	if(rot.y) mVal = glm::rotate(mVal, glm::radians(rot.y), glm::vec3(0, 1.0, 0));
	if(rot.z) mVal = glm::rotate(mVal, glm::radians(rot.z), glm::vec3(0, 0, 1.0));
}

void Matrix::translate(glm::vec3 pos)
{
	mVal = glm::translate(mVal, pos);
}

void Matrix::scale(float sc)
{
	mVal = glm::scale(mVal, glm::vec3(sc, sc, sc));
}

void Matrix::scaleVec(glm::vec3 sc)
{
	mVal = glm::scale(mVal, glm::vec3(0.2, 0.2, 0.2));
}

