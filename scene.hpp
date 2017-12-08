#include "log.h"

template<class T>
void Scene::addModel(T* m)
{
	mLuaModels.push_back(m);
}
