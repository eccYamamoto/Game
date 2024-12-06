#include "SceneTitle.h"

#include "./System/Input.h"
#include "SceneManager.h"
#include "SceneGame.h"
#include "../imgui-docking/imgui.h"

void SceneTitle::Initialize()
{

}

void SceneTitle::Finalize()
{

}

void SceneTitle::Update(float elapsedTime)
{
	Mouse& mouse = Input::Instance().GetMouse();

	if (mouse.GetButtonDown() & mouse.BTN_MIDDLE)
	{
		SceneManager::Instance().ChangeScene(new SceneGame);
		return;
	}
}

void SceneTitle::Render()
{

}

void SceneTitle::DrawGUI()
{
	ImGui::Begin("Title");

	ImGui::End();
}
