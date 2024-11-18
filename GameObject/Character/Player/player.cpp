#include "GameObject/Character/Player/playerh.h"

void Player::Init()
{
	//m_Component = new ModelRenderer(this);
	//((ModelRenderer*)m_Component)->Load("asset\\model\\player.obj");

	//m_Component = new AnimationModel(this);
	//((AnimationModel*)m_Component)->Load("asset\\model\\Maria.fbx");
	//((AnimationModel*)m_Component)->LoadAnimation("asset\\model\\Maria_Idle.fbx","Idle");
	//((AnimationModel*)m_Component)->LoadAnimation("asset\\model\\Maria_Running.fbx", "Run");

	//m_ChildModel = new ModelRenderer(this);
	//((ModelRenderer*)m_ChildModel)->Load("asset\\model\\6ce0b353b442_excalibur_sword__3d.obj");

	//Renderer::CreateVertexShader(&m_VertexShader, &m_VertexLayout,
	//	"shader\\unlitTextureVS.cso");

	//Renderer::CreatePixelShader(&m_PixelShader,
	//	"shader\\unlitTexturePS.cso");

	//m_SE = new Audio(this);
	//m_SE->Load("asset\\audio\\wan.wav");
	//
	//Scene* scene = Manager::GetScene();

	//m_spriteEmitter = scene->AddGameObject<SpriteEmitter>(1);

	////	クォータニオン初期化
	//m_Quaternion.x = 0.0f;
	//m_Quaternion.y = 0.0f;
	//m_Quaternion.z = 0.0f;
	//m_Quaternion.w = 1.0f;

	//m_Scale = { 0.01f,0.01f,0.01f };

	//m_AnimationName = "Idle";
	//m_NextAnimationName = "Idle";

	
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
//}
//
//void Player::Update()
//{
//	XMFLOAT3 oldPosition = m_Position;
//
//	float dt = 1.0f / 60.0f;
//
//	//m_Component->Update();
//	Scene* scene;
//	scene = Manager::GetScene();
//
//	//　雪のEmitterはプレイヤーに追従する
//	scene->GetGameObject<SnowEmitter>()->SetPosition(XMFLOAT3(m_Position.x,m_Position.y+10.0f,m_Position.z));
//
//	Camera* camera = scene->GetGameObject<Camera>();
//	XMFLOAT3 forward = camera->GetForward();
//	XMFLOAT3 right = camera->GetRight();
//	bool isInput = false;
//
//	if (Input::GetKeyPress('A'))
//	{
//		m_Position.x -= right.x * 0.1f;
//		m_Position.y -= right.y * 0.1f;
//		m_Position.z -= right.z * 0.1f;
//		m_Rotation.y = camera->GetRotation().y-3.14f*0.5f;
//		XMVECTOR quat = XMQuaternionRotationRollPitchYaw(0.0f, -AI_MATH_PI*0.5f, 0.0f);
//		XMVECTOR camquat = XMQuaternionRotationRollPitchYaw(0.0f, camera->GetRotation().y, 0.0f);
//		quat = XMQuaternionMultiply(quat, camquat);
//		//XMVECTOR quat = XMQuaternionRotationRollPitchYaw(0.0f, -0.1f, 0.0f);
//		//quat = XMQuaternionMultiply(XMLoadFloat4(&m_Quaternion), quat);	//	行列演算で回転を計算する
//		XMStoreFloat4(&m_Quaternion, quat);
//		m_spriteEmitter->SpawnSprite(m_Position, XMFLOAT3(right.x, right.y, right.z), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
//		
//		m_NextAnimationName = "Run";
//		isInput = true;
//	}
//
//	if (Input::GetKeyPress('D'))
//	{
//		m_Position.x += right.x * 0.1f;
//		m_Position.y += right.y * 0.1f;
//		m_Position.z += right.z * 0.1f;
//		XMVECTOR quat = XMQuaternionRotationRollPitchYaw(0.0f, AI_MATH_PI * 0.5f, 0.0f);
//		XMVECTOR camquat = XMQuaternionRotationRollPitchYaw(0.0f, camera->GetRotation().y, 0.0f);
//		quat = XMQuaternionMultiply(quat, camquat);
//		//XMVECTOR quat = XMQuaternionRotationRollPitchYaw(0.0f, 0.1f, 0.0f);
//		//quat = XMQuaternionMultiply(XMLoadFloat4(&m_Quaternion), quat);	//	行列演算で回転を計算する
//		XMStoreFloat4(&m_Quaternion, quat);
//		//m_Rotation.y = camera->GetRotation().y + 3.14f * 0.5f;
//		m_spriteEmitter->SpawnSprite(m_Position, XMFLOAT3(-right.x, -right.y, -right.z),XMFLOAT4(1.0f,1.0f,1.0f,1.0f));
//		m_NextAnimationName = "Run";
//		isInput = true;
//	}
//
//
//	if (Input::GetKeyPress('W'))
//	{
//		
//		m_Position.x += forward.x * 0.1f;
//		m_Position.y += forward.y * 0.1f;
//		m_Position.z += forward.z * 0.1f;
//		m_Rotation.y = camera->GetRotation().y;
//		XMVECTOR quat = XMQuaternionRotationRollPitchYaw(0.0f, 0.0f, 0.0f);
//		XMVECTOR camquat = XMQuaternionRotationRollPitchYaw(0.0f, camera->GetRotation().y, 0.0f);
//		quat = XMQuaternionMultiply(quat, camquat);
//		//quat = XMQuaternionMultiply(XMLoadFloat4(&m_Quaternion), quat);	//	行列演算で回転を計算する
//		XMStoreFloat4(&m_Quaternion, quat);
//		m_spriteEmitter->SpawnSprite(m_Position,XMFLOAT3(-forward.x,-forward.y,-forward.z), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
//		m_NextAnimationName = "Run";
//		isInput = true;
//	}
//
//	if (Input::GetKeyPress('S'))
//	{
//		
//		m_Position.x -= forward.x * 0.1f;
//		m_Position.y -= forward.y * 0.1f;
//		m_Position.z -= forward.z * 0.1f;
//		m_Rotation.y = camera->GetRotation().y+3.14f;
//		XMVECTOR quat = XMQuaternionRotationRollPitchYaw(0.0f, AI_MATH_PI, 0.0f);
//		XMVECTOR camquat = XMQuaternionRotationRollPitchYaw(0.0f, camera->GetRotation().y, 0.0f);
//		quat = XMQuaternionMultiply(quat, camquat);
//		//quat = XMQuaternionMultiply(XMLoadFloat4(&m_Quaternion), quat);	//	行列演算で回転を計算する
//		XMStoreFloat4(&m_Quaternion, quat);
//		m_spriteEmitter->SpawnSprite(m_Position, XMFLOAT3(forward.x, forward.y, forward.z), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
//
//		m_NextAnimationName = "Run";
//		isInput = true;
//	}
//
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
