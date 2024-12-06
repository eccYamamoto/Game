#pragma once

#include "./Scene/Scene.h"

class SceneGame : public Scene
{
public:
	SceneGame() = default;
	~SceneGame() = default;
	void Initialize() override;
	void Finalize() override;
	void Update(float elapsedTime) override;
	void Render() override;
	void DrawGUI() override;
};
