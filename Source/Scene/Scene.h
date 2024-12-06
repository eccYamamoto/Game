#pragma once

class Scene
{
public:
	Scene() = default;
	virtual ~Scene() = default;
	virtual void Initialize() = 0;
	virtual void Finalize() = 0;
	virtual void Update(float elapsedTime) = 0;
	virtual void Render() = 0;
	virtual void DrawGUI() = 0;

	bool IsReady() const { return ready; }
	void SetReady() { ready = true; }
protected:
	bool ready{ false };
};
