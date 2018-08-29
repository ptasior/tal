#include "renderer.h"
#include "camera.h"
#include "camera_rotating.h"
#include "shader.h"
#include "texture.h"
#include "gui.h"
#include "scene.h"


void Renderer::windowResized(int width, int height)
{
	Log() << "Renderer: Window resized";

	if(mCamera)
		mCamera->setSceneSize(width, height);

	if(mGui)
		mGui->setSceneSize(width, height);
}

void Renderer::paint() //const
{
	// glEnable(GL_CULL_FACE);
	if(mScene)
		mScene->paint();

	// glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);

	if(mGui)
		mGui->paint();

	glEnable(GL_DEPTH_TEST);
}

void Renderer::init()
{
	Log() << "Renderer: Initialising";

	//
	// mScene = std::make_shared<Scene>();
	// // mScene->init();
	// mScene->setCamera(mCamera);

	mGui = std::make_shared<Gui>();
	mGui->init();
}

Camera* Renderer::camera()
{
	return mCamera.get();
}

Scene* Renderer::scene()
{
	return mScene.get();
}

Gui* Renderer::gui()
{
	return mGui.get();
}

std::shared_ptr<Shader> Renderer::shader(const char *name)
{
	if(!mShaders.count(name)) // Not yet initialised
	{
		mShaders[name].reset(new Shader);
		mShaders[name]->init(name);
		if(mCamera)
			mCamera->setupShaderMVP(mShaders[name].get());
	}

	return mShaders[name];
}

std::shared_ptr<Texture> Renderer::texture(const char* path, Shader *s, const char* name, int id)
{
	// TODO Is it ok that texture may be reused with another shader???
	if(!mTextures.count(path))
	{
		mTextures[path].reset(new Texture);
		mTextures[path]->setName(name?name:"mytexture", id);
		mTextures[path]->init(path, s);
	}

	return mTextures[path];
}

void Renderer::initCamera()
{
	mCamera = std::make_shared<RotatingCamera>(); // TODO Configurable
	// mCamera = std::make_shared<FpsCamera>();
	mCamera->init();

	for(auto s : mShaders)
		if(s.second->hasUniform("mvp"))
			mCamera->setupShaderMVP(s.second.get());
}

