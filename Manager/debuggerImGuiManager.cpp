#include <filesystem>
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
std::vector<std::string> DebuggerImGuiManager::m_FileList;

void DebuggerImGuiManager::SetGameObject(GameObject* object)
{


}
void DebuggerImGuiManager::LoadCurrentSceneGameObjectList(Scene* scene)
{


}
void DebuggerImGuiManager::Init()
{
	//	ImGuiセットアップ
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	ImGui_ImplWin32_Init(GetWindow());
	ImGui_ImplDX11_Init(Renderer::GetDevice(), Renderer::GetDeviceContext());
	ImGui::StyleColorsDark();

}

void DebuggerImGuiManager::Render(std::vector<GameObject*>(&ObjectList)[static_cast<int>(GAMEOBJECT_TYPE::MAX_TYPE)])
{

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("GameObject List");
	for (int type = 0; type < static_cast<int>(GAMEOBJECT_TYPE::MAX_TYPE); ++type)
	{
		for (GameObject*& gameObject : ObjectList[type])
		{
			if (!gameObject)continue;
			if (ImGui::Selectable(gameObject->GetName().c_str(), m_TargetObject == gameObject))
			{
				m_TargetObject = gameObject;
			}
		}
	}

	ImGui::End();
	// 選択されたオブジェクトの詳細を表示
	if (m_TargetObject)
	{
		ImGui::Begin("Selected GameObject");
		//	基本Transform情報
		ImGui::Text("Name: %s", m_TargetObject->GetName().c_str());
		if (ImGui::CollapsingHeader("Transform"))
		{
			const XMFLOAT3& position = m_TargetObject->GetPosition();
			const XMFLOAT3& rotation = m_TargetObject->GetRotation();
			const XMFLOAT3& scale = m_TargetObject->GetScale();
			ImGui::Text("Position: %.2f, %.2f, %.2f", position.x, position.y, position.z);
			ImGui::Text("Rotation: %.2f, %.2f, %.2f", rotation.x, rotation.y, rotation.z);
			ImGui::Text("Rotation: %.2f, %.2f, %.2f", scale.x, scale.y, scale.z);
		}

		//　キャラクターが選択された時に表示される情報
		Character* characterObject = dynamic_cast<Character*>(m_TargetObject);
		if (characterObject)
		{
			//	物理関連
			if (ImGui::CollapsingHeader("Physics"))
			{
				const XMFLOAT3& velocity = characterObject->GetVelocity();
				const XMFLOAT3& acceleration = characterObject->GetAccl();

				ImGui::Text("Velocity: %.2f, %.2f, %.2f", velocity.x, velocity.y, velocity.z);
				ImGui::Text("Acceleration: %.2f, %.2f, %.2f", acceleration.x, acceleration.y, acceleration.z);
			}
			//	アニメーション関連
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
			//	当たり判定関連
			if (ImGui::CollapsingHeader("Collision"))
			{
				ImGui::Text("Current Collision List");

				std::vector<std::string> collisionKeys;
				std::vector<const char*> collisionKeyPointers;
				std::vector<std::string> allBoneKeys;
				std::vector<const char*> allBoneKeyPointers;

				//	==============現在コリジョンリスト作成=====================
				std::unordered_map<std::string, Collision*>& collisionList = characterObject->GetCollisionList();
				{
					
					static int item_current = 0;
					static std::string previousKey{};
					std::string selectedKey{};

					for (const auto& pair : collisionList)
					{
						if (!pair.second)continue;
						collisionKeys.emplace_back(pair.first);
					}
					//	const char*配列作成　Imgui display
					for (const auto& key : collisionKeys)
					{
						collisionKeyPointers.emplace_back(key.c_str());
					}
					//	現在のコリジョンを全部表示
					if (!collisionKeyPointers.empty())
					{
						int previousItemCurrent = item_current;

						ImGui::ListBox(" ", &item_current, collisionKeyPointers.data(), static_cast<int>(collisionKeyPointers.size()),4);

						//	選択中のコリジョンを変色
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
						static float offset[3]{};
						static float radius[1]{};
						offset[0] = selectedCollision->GetOffset().x;
						offset[1] = selectedCollision->GetOffset().y;
						offset[2] = selectedCollision->GetOffset().z;
						
						if (ImGui::DragFloat3("Offset Slider", offset, 0.01f,-10, 10))
						{
							selectedCollision->SetOffset(XMFLOAT3(offset[0],offset[1],offset[2]));
						}
						//if (ImGui::InputFloat3("Offset Input", offset))
						//{
						//	selectedCollision->SetOffset(XMFLOAT3(offset[0], offset[1], offset[2]));
						//}
						CharacterBoneCollision* boneCollision = dynamic_cast<CharacterBoneCollision*>(selectedCollision);
						if (boneCollision)
						{
							radius[0] = boneCollision->GetRadius();
							if (ImGui::DragFloat("Radius Slider", radius,0.01f, -10, 10))
							{
								boneCollision->SetRadius(radius[0]);
							}
	/*						if (ImGui::InputFloat("Radius Input", radius, -10, 10))
							{
								boneCollision->SetRadius(radius[0]);
							}*/
						}
						previousKey = selectedKey;
					}
					else
					{
						ImGui::Text("No Collision");
					}
					//	選択したコリジョンを削除
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

				//	ボーンプロファイル選択
				{
					static std::string directoryPath = "asset\\boneProfile";
					static int selectedBoneProfile = 0;
					static std::string selectedFilePath = "";
					static std::vector<const char*> csvFilesName;
					ImGui::Text("Directory:%s", directoryPath.c_str());
					if (csvFilesName.empty())
					{
						LoadCSVFiles(directoryPath, csvFilesName);
					}
					if (ImGui::Button("Reload CSV Files"))
					{
						LoadCSVFiles(directoryPath,csvFilesName);
					}
					if (!m_FileList.empty())
					{
						if (ImGui::ListBox("Character Bone Profile", &selectedBoneProfile, csvFilesName.data(), static_cast<int>(csvFilesName.size()), 4))
						{
							selectedFilePath = directoryPath + "\\" + m_FileList[selectedBoneProfile];
						}
					}
					if (ImGui::Button("Deploy Bone Profile"))
					{
						characterObject->CreateCharacterBoneCollision(selectedFilePath);
					}
;					
				}
				//==================作成可能ボーンリスト===================
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

	//	DrawData合成
	ImGui::Render();			//　この処理した後、GetDrawDataでRenderされる
	//	Renderデータ
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void DebuggerImGuiManager::Uninit()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void DebuggerImGuiManager::LoadCSVFiles(const std::string& Path, std::vector<const char*>& CSVFileName)
{
	m_FileList.clear();
	CSVFileName.clear();

	if (!std::filesystem::exists(Path) || !std::filesystem::is_directory(Path))
	{
		return;
	}

	for (const auto& entry : std::filesystem::directory_iterator(Path))
	{
		if (entry.is_regular_file() && entry.path().extension() == ".csv")
		{
			m_FileList.push_back(entry.path().filename().string());  // ファイル名のみ取得
		}
	}
	
	for (const auto& file : m_FileList)
	{
		CSVFileName.emplace_back(file.c_str());
	}
}
