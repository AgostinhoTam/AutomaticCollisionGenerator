#include "Manager\debuggerImGuiManager.h"
#include "Main\main.h"
#include "Renderer\renderer.h"
#include "Manager\sceneManager.h"
#include "Scene\scene.h"
#include "Manager\gameObjectManager.h"
#include "GameObject\gameobject.h"
#include "GameObject\Character\character.h"

GameObject* DebuggerImGuiManager::m_TargetObject;
std::vector<GameObject*> DebuggerImGuiManager::m_GameObjectList;

void DebuggerImGuiManager::SetGameObject(GameObject* object)
{

	
}
void DebuggerImGuiManager::LoadCurrentSceneGameObjectList(Scene* scene)
{

	
}
void DebuggerImGuiManager::Init()
{
	//	ImGui�Z�b�g�A�b�v
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplWin32_Init(GetWindow());
	ImGui_ImplDX11_Init(Renderer::GetDevice(), Renderer::GetDeviceContext());
	ImGui::StyleColorsDark();

}

void DebuggerImGuiManager::Render(const std::vector<GameObject*>(&ObjectList)[static_cast<int>(GAMEOBJECT_TYPE::MAX_TYPE)])
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("GameObject List");
	for (int type = 0; type < static_cast<int>(GAMEOBJECT_TYPE::MAX_TYPE); ++type)
	{
		for (GameObject* gameObject : ObjectList[type])
		{
			if (!gameObject)continue;
			if (ImGui::Selectable(gameObject->GetName().c_str(), m_TargetObject == gameObject))
			{
				m_TargetObject = gameObject;
			}
		}
	}

	ImGui::End();
	// �I�����ꂽ�I�u�W�F�N�g�̏ڍׂ�\��
	if (m_TargetObject)
	{
		ImGui::Begin("Selected GameObject");
		const XMFLOAT3& position = m_TargetObject->GetPosition();
		const XMFLOAT3& rotation = m_TargetObject->GetRotation();
		ImGui::Text("Name: %s", m_TargetObject->GetName().c_str());
		ImGui::Text("Position: %.2f, %.2f, %.2f", position.x, position.y, position.z);
		ImGui::Text("Rotation: %.2f, %.2f, %.2f", rotation.x, rotation.y, rotation.z);

		Character* characterObject = dynamic_cast<Character*>(m_TargetObject);
		if (characterObject)
		{

			const XMFLOAT3& velocity = characterObject->GetVelocity();
			const XMFLOAT3& acceleration = characterObject->GetAccl();

			ImGui::Text("Velocity: %.2f, %.2f, %.2f", velocity.x, velocity.y, velocity.z);
			ImGui::Text("Acceleration: %.2f, %.2f, %.2f", acceleration.x, acceleration.y, acceleration.z);
		}

		ImGui::End();
	}

	//	DrawData����
	ImGui::Render();			//�@���̏���������AGetDrawData��Render�����
	//	Render�f�[�^
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void DebuggerImGuiManager::Uninit()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}
