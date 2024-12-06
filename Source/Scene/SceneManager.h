#pragma once

#include "./Template/Manager.h"
#include "Scene.h"

class SceneManager : public Manager<Scene>
{
public:
	static SceneManager& Instance()
	{
		static SceneManager instance;
		return instance;
	}

	void Update(float elapsedTime) override;
	void Render() override { if (curScene) curScene->Render(); }
	void DrawGUI() override { if (curScene) curScene->DrawGUI(); }
	void Clear() override;

	void ChangeScene(Scene* scene) { nexScene = scene; }
private:
	SceneManager() = default;
	~SceneManager() = default;
private:
	Scene* curScene{ nullptr };
	Scene* nexScene{ nullptr };
};
