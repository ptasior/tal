#pragma once

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Awful Lua wrapper for glm
class Matrix
{
public:
	glm::mat4& val()
	{
		return mVal;
	}

	void rotate(glm::vec3 rot)
	{
		if(rot.x) mVal = glm::rotate(mVal, glm::radians(rot.x), glm::vec3(1.0, 0, 0));
		if(rot.y) mVal = glm::rotate(mVal, glm::radians(rot.y), glm::vec3(0, 1.0, 0));
		if(rot.z) mVal = glm::rotate(mVal, glm::radians(rot.z), glm::vec3(0, 0, 1.0));
	}

	void translate(glm::vec3 pos)
	{
		mVal = glm::translate(mVal, pos);
	}

	void scale(float sc)
	{
		mVal = glm::scale(mVal, glm::vec3(sc, sc, sc));
	}

	void scaleVec(glm::vec3 sc)
	{
		mVal = glm::scale(mVal, glm::vec3(0.2, 0.2, 0.2));
	}

private:
	glm::mat4 mVal;
};
