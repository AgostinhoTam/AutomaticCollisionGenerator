#include "Manager\debuggerImGuiManager.h"
#include "Main\main.h"
#include "System\Renderer\renderer.h"
#include "System\Collision\collision.h"
#include "System\Renderer\animationModel.h"
#include "System\Collision\characterBoneCollision.h"
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
		//	��{Transform���
		ImGui::Text("Name: %s", m_TargetObject->GetName().c_str());
		if (ImGui::CollapsingHeader("Transform"))
		{
			const XMFLOAT3& position = m_TargetObject->GetPosition();
			const XMFLOAT3& rotation = m_TargetObject->GetRotation();
			ImGui::Text("Position: %.2f, %.2f, %.2f", position.x, position.y, position.z);
			ImGui::Text("Rotation: %.2f, %.2f, %.2f", rotation.x, rotation.y, rotation.z);
		}

		//�@�L�����N�^�[���I�����ꂽ���ɕ\���������
		Character* characterObject = dynamic_cast<Character*>(m_TargetObject);
		if (characterObject)
		{
			//	�����֘A
			if (ImGui::CollapsingHeader("Physics"))
			{
				const XMFLOAT3& velocity = characterObject->GetVelocity();
				const XMFLOAT3& acceleration = characterObject->GetAccl();

				ImGui::Text("Velocity: %.2f, %.2f, %.2f", velocity.x, velocity.y, velocity.z);
				ImGui::Text("Acceleration: %.2f, %.2f, %.2f", acceleration.x, acceleration.y, acceleration.z);
			}
			//	�A�j���[�V�����֘A
			if (ImGui::CollapsingHeader("Animation"))
			{
				AnimationModel* model = characterObject->GetAnimationModel();
				ImGui::Text("CurrentAnimationName:%s", model->GetCurrentAnimationName().c_str());
				ImGui::Text("CurrAnimationFrame: %d", model->GetCurrentAnimationFrame());
				ImGui::Text("IsTransitioning: %d", model->GetIsTransitioning());
				ImGui::Text("NextAnimationName:%s", model->GetNextAnimationName().c_str());
				ImGui::Text("NextAnimationFrame: %d", model->GetNextAnimationFrame());
				ImGui::Text("BlendRatio: %f", model->GetBlendRatio());
			}
			//	�����蔻��֘A
			if (ImGui::CollapsingHeader("Collision"))
			{
				ImGui::Text("Current Collision List");

				std::vector<std::string> collisionKeys;
				std::vector<const char*> collisionKeyPointers;
				std::vector<std::string> allBoneKeys;
				std::vector<const char*> allBoneKeyPointers;

				//	==============���݃R���W�������X�g�쐬=====================
				std::unordered_map<std::string, Collision*>& collisionList = characterObject->GetCollisionList();
				{
					static int item_current = 0;
					static std::string previousKey{};
					std::string selectedKey{};

					for (const auto& pair : collisionList)
					{
						collisionKeys.emplace_back(pair.first);
					}
					//	const char*�z��쐬�@
					for (const auto& key : collisionKeys)
					{
						collisionKeyPointers.emplace_back(key.c_str());
					}
					//	���݂̃R���W������S���\��
					if (!collisionKeyPointers.empty())
					{
						int previousItemCurrent = item_current;

						ImGui::ListBox(" ", &item_current, collisionKeyPointers.data(), static_cast<int>(collisionKeyPointers.size()),4);

						//	�I�𒆂̃R���W������ϐF
						selectedKey = collisionKeys[item_current];
						if (!previousKey.empty() && previousKey != selectedKey)
						{
							Collision* previousCollision = characterObject->GetSelectedCollision(previousKey);
							if (previousCollision)
							{
								previousCollision->SetIsSelected(false);
							}
						}
						Collision* selectedCollision = characterObject->GetSelectedCollision(selectedKey);
						if (selectedCollision)
						{
							selectedCollision->SetIsSelected(true);
						}
						previousKey = selectedKey;
					}
					else
					{
						ImGui::Text("No Collision");
					}
					//	�I�������R���W�������폜
					ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(1 / 7.0f, 0.6f, 0.6f));
					ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(1 / 7.0f, 0.7f, 0.7f));
					ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(1 / 7.0f, 0.8f, 0.8f));
					if (ImGui::Button("Delete Bone Collision"))
					{
						auto it = collisionList.find(selectedKey);
						if (it != collisionList.end())
						{
							delete it->second;
							collisionList.erase(it);
							selectedKey.clear();
							previousKey.clear();
							item_current = 0;
						}
					}
					ImGui::PopStyleColor(3);
				}

				//==================�쐬�\�{�[�����X�g===================
				{
					ImGui::Text("Create Bone Collision");
					static int head_bone = 0;
					static int tail_bone = 1;

					allBoneKeys = characterObject->GetBoneMap();
					for (const auto& key : allBoneKeys)
					{
						allBoneKeyPointers.emplace_back(key.c_str());
					}
					if (!allBoneKeyPointers.empty())
					{
						ImGui::Combo("HeadBone", &head_bone, allBoneKeyPointers.data(),  static_cast<int>(allBoneKeyPointers.size()), 4);
						ImGui::Combo("TailBone", &tail_bone, allBoneKeyPointers.data(),  static_cast<int>(allBoneKeyPointers.size()), 4);
					}

					ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(1 / 7.0f, 0.6f, 0.6f));
					ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(1 / 7.0f, 0.7f, 0.7f));
					ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(1 / 7.0f, 0.8f, 0.8f));
					if (ImGui::Button("Create New Bone Collision"))
					{
						characterObject->CreateSingleBoneCollision(allBoneKeys[head_bone], allBoneKeys[tail_bone], XMFLOAT3(0, 0, 0));
					}
					ImGui::PopStyleColor(3);
				}
			}
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
