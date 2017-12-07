#include "log.h"

template<class T>
void Scene::addModel(T* m)
{
	Log() << "Scene::addModel";
	mLuaModels.push_back(m);
}
