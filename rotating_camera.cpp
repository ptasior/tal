#include "rotating_camera.h"
#include "log.h"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void RotatingCamera::init()
{
	Log() << "RotatingCamera: init";
	setModel(glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, -4.0)));

	setView(glm::lookAt(glm::vec3(0.0, 2.0, 0.0),
				glm::vec3(0.0, 0.0, -4.0),
				glm::vec3(0.0, 1.0, 0.0))
			);
}

void RotatingCamera::apply()
{
	float angle = SDL_GetTicks() / 1000.0 * 45;  // 45° per second
	glm::vec3 axis_y(0, 1, 0);
	glm::mat4 anim = glm::rotate(glm::mat4(1.0f), glm::radians(angle), axis_y);

	setPreRot(anim);

	Camera::apply();
}
 
