#pragma once

#include "./Scene/Scene.h"

class SceneTitle : public Scene
{
public:
	SceneTitle() = default;
	~SceneTitle() override = default;
	void Initialize() override;
	void Finalize() override;
	void Update(float elapsedTime) override;
	void DrawGUI() override;
	void Render() override;
};

