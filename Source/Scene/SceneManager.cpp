#include "SceneManager.h"

void SceneManager::Update(float elapsedTime)
{
	if (nexScene)
	{
		Clear();
		curScene = nexScene;
		nexScene = nullptr;
		if (!curScene->IsReady()) curScene->Initialize();
	}
	if (curScene) curScene->Update(elapsedTime);
}

void SceneManager::Clear()
{
	if (curScene)
	{
		curScene->Finalize();
		delete curScene;
		curScene = nullptr;
	}
}
