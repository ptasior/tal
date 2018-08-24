#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Awful Lua wrapper for glm
class Matrix
{
public:
	glm::mat4& val();

	void rotate(glm::vec3 rot);
	void translate(glm::vec3 pos);
	void scale(double sc);
	void scaleVec(glm::vec3 sc);

private:
	glm::mat4 mVal;
};

