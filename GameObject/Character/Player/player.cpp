#include "Manager/modelRendererManager.h"
#include "Manager/sceneManager.h"
#include "Manager/shaderManager.h"
#include "Manager/inputManager.h"
#include "Manager/gameObjectManager.h"
#include "Scene/scene.h"
#include "Renderer/modelRenderer.h"
#include "Renderer/animationModel.h"
#include "StateMachine/PlayerState/playerStateIdle.h"
#include "StateMachine/PlayerState/playerStateWalk.h"
#include "GameObject/Character/Player/playerh.h"
#include "GameObject/Camera/camera.h"

constexpr float PLAYER_MAX_SPEED = 300.0f;
constexpr float PLAYER_MAX_ACCL_SPEED = 20.0f;
constexpr float PLAYER_MAX_JUMP_SPEED = 100.0f;
void Player::Init()
{
	m_Model = ModelRendererManager::Load("asset\\model\\1fe8be154891_freedom_gundam_full.obj");
	m_AnimationModel = new AnimationModel;
	if(m_AnimationModel)m_AnimationModel->Load("asset\\model\\playerrobot.fbx");

	m_Shader = ShaderManager::LoadShader(SHADER_NAME::UNLIT_TEXTURE);

	m_MaxMovementSpeed = PLAYER_MAX_SPEED;
	m_MaxHorizontalAcclSpeed = PLAYER_MAX_ACCL_SPEED;


	Scene* scene = SceneManager::GetInstance()->GetCurrentScene();
	if (scene)
	{
		GameObjectManager* objectManager = scene->GetGameObjectManager();
		if (objectManager)
		{
			m_Camera = objectManager->GetGameObject<Camera>(GAMEOBJECT_TYPE::CAMERA);
		}
	}

	m_PlayerState.reserve(static_cast<int>(PLAYER_STATE::MAX_STATE));
	m_PlayerState.try_emplace(PLAYER_STATE::IDLE, new PlayerStateIdle(this, m_Camera));
	m_PlayerState.try_emplace(PLAYER_STATE::WALK, new PlayerStateWalk(this, m_Camera));
	m_CurrentState = m_PlayerState[PLAYER_STATE::IDLE];
	m_Scale = { 2.0f,2.0f,2.0f };

	m_Position.y = 1.0f;
	m_IsGround = true;
	
}

void Player::Uninit()
{
//	m_SE->Uninit();
//	delete m_SE;
//
//	m_Component->Uninit();
//	delete m_Component;
//
//	m_ChildModel->Uninit();
//	delete m_ChildModel;
//	//m_spriteEmitter->SetDestroy();
//
//	m_VertexLayout->Release();
//	m_VertexShader->Release();
//	m_PixelShader->Release();
	m_AnimationModel->Uninit();
}

void Player::Update(const float& DeltaTime)
{
	if (!m_Model)return;
	if (!m_CurrentState)return;
	if (!m_AnimationModel)return;
	if (!m_Camera)return;

	m_CurrentState->Update();

	UpdatePlayerRotation();

	Character::Update(DeltaTime);


//	if (InputManager::GetKeyPress('A'))
//	{
//		//m_Position.x -= right.x * 0.1f;
//		//m_Position.y -= right.y * 0.1f;
//		//m_Position.z -= right.z * 0.1f;
////		m_Rotation.y = camera->GetRotation().y-3.14f*0.5f;
//		m_Position.x -= 0.1f;
//		m_Quaternion.y -= 0.1f;
//		m_Quaternion.y = fmod(m_Quaternion.y, XM_2PI);
//		//XMVECTOR quat = XMQuaternionRotationRollPitchYaw(0.0f, m_Quaternion.y, 0.0f);
////		XMVECTOR camquat = XMQuaternionRotationRollPitchYaw(0.0f, camera->GetRotation().y, 0.0f);
////		quat = XMQuaternionMultiply(quat, camquat);
////		//XMVECTOR quat = XMQuaternionRotationRollPitchYaw(0.0f, -0.1f, 0.0f);
////		//quat = XMQuaternionMultiply(XMLoadFloat4(&m_Quaternion), quat);	//	行列演算で回転を計算する
////		XMStoreFloat4(&m_Quaternion, quat);
////		m_spriteEmitter->SpawnSprite(m_Position, XMFLOAT3(right.x, right.y, right.z), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
////		
////		m_NextAnimationName = "Run";
////		isInput = true;
//	}
////
//	if (InputManager::GetKeyPress('D'))
//	{
////		m_Position.x += right.x * 0.1f;
////		m_Position.y += right.y * 0.1f;
////		m_Position.z += right.z * 0.1f;
//		m_Position.x += 0.1f;
// 		m_Quaternion.y += 0.1f;
//		m_Quaternion.y = fmod(m_Quaternion.y, -XM_2PI);
////		XMVECTOR quat = XMQuaternionRotationRollPitchYaw(0.0f, AI_MATH_PI * 0.5f, 0.0f);
////		XMVECTOR camquat = XMQuaternionRotationRollPitchYaw(0.0f, camera->GetRotation().y, 0.0f);
////		quat = XMQuaternionMultiply(quat, camquat);
////		//XMVECTOR quat = XMQuaternionRotationRollPitchYaw(0.0f, 0.1f, 0.0f);
////		//quat = XMQuaternionMultiply(XMLoadFloat4(&m_Quaternion), quat);	//	行列演算で回転を計算する
////		XMStoreFloat4(&m_Quaternion, quat);
////		//m_Rotation.y = camera->GetRotation().y + 3.14f * 0.5f;
////		m_spriteEmitter->SpawnSprite(m_Position, XMFLOAT3(-right.x, -right.y, -right.z),XMFLOAT4(1.0f,1.0f,1.0f,1.0f));
////		m_NextAnimationName = "Run";
////		isInput = true;
//	}
////
////
////	if (Input::GetKeyPress('W'))
////	{
////		
////		m_Position.x += forward.x * 0.1f;
////		m_Position.y += forward.y * 0.1f;
////		m_Position.z += forward.z * 0.1f;
////		m_Rotation.y = camera->GetRotation().y;
////		XMVECTOR quat = XMQuaternionRotationRollPitchYaw(0.0f, 0.0f, 0.0f);
////		XMVECTOR camquat = XMQuaternionRotationRollPitchYaw(0.0f, camera->GetRotation().y, 0.0f);
////		quat = XMQuaternionMultiply(quat, camquat);
////		//quat = XMQuaternionMultiply(XMLoadFloat4(&m_Quaternion), quat);	//	行列演算で回転を計算する
////		XMStoreFloat4(&m_Quaternion, quat);
////		m_spriteEmitter->SpawnSprite(m_Position,XMFLOAT3(-forward.x,-forward.y,-forward.z), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
////		m_NextAnimationName = "Run";
////		isInput = true;
////	}
////
////	if (Input::GetKeyPress('S'))
////	{
////		
////		m_Position.x -= forward.x * 0.1f;
////		m_Position.y -= forward.y * 0.1f;
////		m_Position.z -= forward.z * 0.1f;
////		m_Rotation.y = camera->GetRotation().y+3.14f;
////		XMVECTOR quat = XMQuaternionRotationRollPitchYaw(0.0f, AI_MATH_PI, 0.0f);
////		XMVECTOR camquat = XMQuaternionRotationRollPitchYaw(0.0f, camera->GetRotation().y, 0.0f);
////		quat = XMQuaternionMultiply(quat, camquat);
////		//quat = XMQuaternionMultiply(XMLoadFloat4(&m_Quaternion), quat);	//	行列演算で回転を計算する
////		XMStoreFloat4(&m_Quaternion, quat);
////		m_spriteEmitter->SpawnSprite(m_Position, XMFLOAT3(forward.x, forward.y, forward.z), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
////
////		m_NextAnimationName = "Run";
////		isInput = true;
////	}
////
//
//	if (Input::GetKeyTrigger(VK_SPACE))
//	{
//		m_Velocity.y = 10.0f;
//	}
//
//	if (Input::GetKeyTrigger('K'))
//	{
//		Bullet* bullet = scene->AddGameObject<Bullet>(1);
//		bullet->SetPosition(m_Position);
//		m_SE->Play(false);
//
//	}
//
//	m_Velocity.y += -10.0f * dt;
//
//	m_Position.y += m_Velocity.y * dt;
//
//	if (m_Position.y < 0.0f)
//	{
//		m_Velocity.y = 0.0f;
//		m_Position.y = 0.0f;
//	}
//	//メッシュフィールド取る
//	MeshField* meshfield = scene->GetGameObject<MeshField>();
//
//	float groundHeight = meshfield->GetHeight(m_Position);
//
//	auto cylinderList = scene->GetGameObjects<Cylinder>();
//
//	for (Cylinder* cylinder : cylinderList)
//	{
//		XMFLOAT3 cylinderPosition = cylinder->GetPosition();
//		XMFLOAT3 cylinderScale = cylinder->GetScale();
//
//		XMFLOAT3 direction;
//		direction.x = cylinderPosition.x - m_Position.x;
//		direction.y = cylinderPosition.y - m_Position.y;
//		direction.z = cylinderPosition.z - m_Position.z;
//
//		float length;
//		length = sqrtf(direction.x * direction.x +
//			direction.z * direction.z);
//
//		if (length < cylinderScale.x)
//		{
//			if (-direction.y > cylinderScale.y - 0.5f)
//			{
//				groundHeight = cylinderPosition.y + cylinderScale.y;
//			}
//			else
//			{
//				m_Position = oldPosition;
//			}
//
//		}
//	}
//	auto boxList = scene->GetGameObjects<Box>();
//
//	for (Box* box : boxList)
//	{
//		XMFLOAT3 boxPosition = box->GetPosition();
//		XMFLOAT3 boxScale = box->GetScale();
//
//		XMFLOAT3 direction;
//		direction.x = boxPosition.x - m_Position.x;
//		direction.y = boxPosition.y - m_Position.y;
//		direction.z = boxPosition.z - m_Position.z;
//
//		float length;
//		length = sqrtf(direction.x * direction.x +
//			direction.z * direction.z);
//
//		if (length < boxScale.x)
//		{
//			if (-direction.y > boxScale.y - 0.5f)
//			{
//				groundHeight = boxPosition.y + boxScale.y;
//			}
//			else
//			{
//				m_Position = oldPosition;
//			}
//
//		}
//	}
//	if (m_Position.y < groundHeight)
//	{
//		m_Position.y = groundHeight;
//		m_Velocity.y = 0.0f;
//	}
//
//
//	// Enterキーで遷移
//	if (Input::GetKeyTrigger(VK_RETURN)) {
//		Manager::SetScene<Result>();
//	}
//	if (isInput) {
//		m_BlendRatio += 0.05f;
//		if (m_BlendRatio > 1.0f)m_BlendRatio = 1.0f;
//	}
//	else {
//		m_BlendRatio -= 0.05f;
//		if (m_BlendRatio < 0.0f)m_BlendRatio = 0.0f;
//	}
}

void Player::Draw()
{

	//ModelRenderer::Draw(m_Model,this);
	m_AnimationModel->Draw(this);
	//((AnimationModel*)m_Component)->Update(m_AnimationName.c_str(), m_AnimationFrame);

	//((AnimationModel*)m_Component)->Update(m_AnimationName.c_str(), m_AnimationFrame, m_NextAnimationName.c_str(), m_AnimationFrame, m_BlendRatio);
	//++m_AnimationFrame;
	//
	//Renderer::GetDeviceContext()->IASetInputLayout(m_VertexLayout);


	//Renderer::GetDeviceContext()->VSSetShader(m_VertexShader, NULL, 0);
	//Renderer::GetDeviceContext()->PSSetShader(m_PixelShader, NULL, 0);


	////	ワールドマトリクス設定
	//XMMATRIX world, scale, rot, trans;
	//scale = XMMatrixScaling(m_Scale.x, m_Scale.y, m_Scale.z);
	//
	////	クォータニオン使うのでこれなし
	////rot = XMMatrixRotationRollPitchYaw(m_Rotation.x, m_Rotation.y+XM_PI, m_Rotation.z);
	//rot = XMMatrixRotationQuaternion(XMLoadFloat4(&m_Quaternion));

	//trans = XMMatrixTranslation(m_Position.x, m_Position.y, m_Position.z);
	//world = scale * rot * trans;
	//Renderer::SetWorldMatrix(world);
	//m_Component->Draw();



	//XMMATRIX rightHandMatrix = ((AnimationModel*)m_Component)->GetBoneMatrix("mixamorig:RightHand");
	////子モデル描画
	//XMMATRIX childScale, childRot, childTrans, childWorld;
	//childScale = XMMatrixScaling(1.0f/m_Scale.x, 1.0f/m_Scale.y, 1.0f/m_Scale.z);
	//childRot = XMMatrixRotationRollPitchYaw(0.0f, 0.0f, 0.0f);
	//childTrans = XMMatrixTranslation(0.0f,0.0f, 0.0f);
	//childWorld = childRot * childTrans * childScale *rightHandMatrix* world;		//*worldを追加すると親の位置情報も計算されるので追加できる

	//Renderer::SetWorldMatrix(childWorld);

	//m_ChildModel->Draw();
}

void Player::ChangeState(PLAYER_STATE State)
{
	//	同じStateに遷移しないように
	if (m_CurrentState == m_PlayerState[State])return;

	m_CurrentState = m_PlayerState[State];

	if (!m_CurrentState) return;
	
	m_CurrentState->Init();
}

void Player::UpdatePlayerRotation()
{
	
	// 移動入力がある場合に回転を更新
	if (m_MoveDirection.x != 0.0f || m_MoveDirection.z != 0.0f)
	{
		const XMFLOAT3& cameraForward = m_Camera->GetForward();
		const XMFLOAT3& cameraRight = m_Camera->GetRight();

		float moveX = m_MoveDirection.x * cameraRight.x + m_MoveDirection.z * cameraForward.x;
		float moveZ = m_MoveDirection.x * cameraRight.z + m_MoveDirection.z * cameraForward.z;

		// 正規化して方向ベクトル
		XMVECTOR moveVector = XMVectorSet(moveX, 0.0f, moveZ, 0.0f);
		moveVector = XMVector3Normalize(moveVector);

		XMFLOAT3 normalizeMove;
		XMStoreFloat3(&normalizeMove, moveVector);

		m_MoveDirection = XMFLOAT3(normalizeMove.x, 0.0f, normalizeMove.z);

		// 回転更新
		float yaw = atan2f(normalizeMove.x, normalizeMove.z);
		m_Rotation.y = yaw;

	}
}
