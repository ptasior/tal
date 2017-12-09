#include "model.h"
#include "shader.h"
#include "texture.h"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


void Model::init(const std::string path)
{
	mShader = Shader::getShader("model");

	mAttrVert = mShader->attrib("coord3d");
	mAttrTex = mShader->attrib("texcoord");
	mAttrNorm = mShader->attrib("vnorm");
}

void Model::setPosition(const glm::mat4 &position)
{
	mPosition = position;
	// mShader->setUniform("position", {glm::value_ptr(mPosition)});
}

void Model::paint()
{}

