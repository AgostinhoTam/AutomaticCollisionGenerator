#include "Main\main.h"
#include "System\Renderer\animationModel.h"
#include "System\Collision\characterBoneCollision.h"
#include "character.h"
constexpr float FRICTION = 0.9f;
constexpr float MAX_DROP_SPEED = -50.0f;
constexpr float GRAVITY = -9.8f;
void Character::Update(const float& DeltaTime)
{
	XMVECTOR position = XMLoadFloat3(&m_Position);
	XMVECTOR velocity = XMLoadFloat3(&m_Velocity);

	UpdateHorizontalVelocity(velocity, DeltaTime);

	UpdateVerticalVelocity(velocity,DeltaTime);

	position = XMVectorMultiplyAdd(velocity, XMVectorReplicate(DeltaTime), position);
	
	XMStoreFloat3(&m_Position,position);
	XMStoreFloat3(&m_Velocity, velocity);

	m_MoveDirection = { 0,0,0 };
}

void Character::UpdateVerticalVelocity(XMVECTOR& Velocity, const float& DeltaTime)
{
	float groundHeight = 0.0f;

	float velocityY = XMVectorGetY(Velocity);

	if (m_IsGround)
	{
		velocityY = 0.0f;
	}
	else
	{
		velocityY += GRAVITY * DeltaTime;
		if (velocityY < MAX_DROP_SPEED)
		{
			velocityY = MAX_DROP_SPEED;
		}
	}
	Velocity = XMVectorSetY(Velocity,velocityY);
}

void Character::UpdateHorizontalVelocity(XMVECTOR& Velocity,const float& DeltaTime)
{
	//	平面方向正規化
	XMVECTOR dirNormalize = XMVectorSet(m_MoveDirection.x, 0.0f, m_MoveDirection.z, 0.0f); //xz移動
	dirNormalize = XMVector3Normalize(dirNormalize);

	//	加速度
	XMVECTOR accl = dirNormalize * m_MaxHorizontalAcclSpeed * DeltaTime;

	//	加速度適用
	Velocity = XMVectorAdd(Velocity, accl);

	XMVECTOR velocityXZ = XMVectorSet(XMVectorGetX(Velocity), 0.0f, XMVectorGetZ(Velocity), 0.0f);
	if (XMVectorGetX(dirNormalize) == 0.0f && XMVectorGetZ(dirNormalize) == 0.0f)
	{
		velocityXZ *= FRICTION;
		Velocity = XMVectorSet(XMVectorGetX(velocityXZ), XMVectorGetY(Velocity), XMVectorGetZ(velocityXZ), 0.0f);
	}

	//	速度上限
	float velocityMagnitude = XMVectorGetX(XMVector3Length(velocityXZ));
	if (velocityMagnitude > m_MaxMovementSpeed)
	{
		//	速度調整
		velocityXZ = XMVector3Normalize(velocityXZ) * m_MaxMovementSpeed;

		Velocity = XMVectorSet(XMVectorGetX(velocityXZ), XMVectorGetY(Velocity), XMVectorGetZ(velocityXZ), 0.0f);
	}
}

void Character::UpdateBoneCollision()
{
	if (m_Collisions.empty())return;
	if (m_BoneMap.empty())return;
	XMMATRIX world, scale, rot, trans;
	const XMFLOAT3& objPosition = m_Position;
	const XMFLOAT3& objScale = m_Scale;
	const XMFLOAT3& objRotation = m_Rotation;

	scale = XMMatrixScaling(objScale.x, objScale.y, objScale.z);

	XMVECTOR quaternion = XMQuaternionRotationRollPitchYaw(objRotation.x, objRotation.y, objRotation.z);
	quaternion = XMQuaternionNormalize(quaternion);
	rot = XMMatrixRotationQuaternion(quaternion);

	trans = XMMatrixTranslation(objPosition.x, objPosition.y, objPosition.z);
	world = scale * rot * trans;

	for (auto& bone : m_BoneMap)
	{
		std::string boneName = bone.first.c_str();
		for (auto& capsule : m_Collisions)
		{
			CharacterBoneCollision* boneCollision = dynamic_cast<CharacterBoneCollision*>(capsule.second);
			if (boneCollision)
			{
				XMFLOAT3 headPos = m_AnimationModel->GetHeadPosition(boneName, m_Scale, world);
				boneCollision->UpdateBonePosition(boneName, headPos);
			}
		}
	}
}

void Character::CreateCharacterBoneCollision()
{
	if (!m_AnimationModel)return;
	m_BoneMap = m_AnimationModel->GetBoneMap();
	if (m_BoneMap.empty())return;
	
	//　上半身
	CreateSingleBoneCollision("mixamorig:Hips", "mixamorig:Spine",XMFLOAT3(0,0,0),0.1f);
	CreateSingleBoneCollision("mixamorig:Spine", "mixamorig:Spine1", XMFLOAT3(0, 0, 0), 0.1f);
	CreateSingleBoneCollision("mixamorig:Spine1", "mixamorig:Spine2", XMFLOAT3(0, 0, 0), 0.1f);
	CreateSingleBoneCollision("mixamorig:Spine2", "mixamorig:Neck", XMFLOAT3(0, 0, 0), 0.1f);

	//	頭
	CreateSingleBoneCollision("mixamorig:Neck","mixamorig:Head", XMFLOAT3(0, 0, 0), 0.1f);
	CreateSingleBoneCollision("mixamorig:Head", "mixamorig:HeadTop_End", XMFLOAT3(0, 0, 0), 0.1f);

	//　左手
	CreateSingleBoneCollision("mixamorig:Spine2", "mixamorig:LeftShoulder", XMFLOAT3(0, 0, 0), 0.1f);
	CreateSingleBoneCollision("mixamorig:LeftShoulder", "mixamorig:LeftArm", XMFLOAT3(0, 0, 0), 0.1f);
	CreateSingleBoneCollision("mixamorig:LeftArm", "mixamorig:LeftForeArm", XMFLOAT3(0, 0, 0), 0.1f);
	CreateSingleBoneCollision("mixamorig:LeftForeArm", "mixamorig:LeftHand", XMFLOAT3(0, 0, 0), 0.1f);

	//　右手
	CreateSingleBoneCollision("mixamorig:Spine2", "mixamorig:RightShoulder", XMFLOAT3(0, 0, 0), 0.1f);
	CreateSingleBoneCollision("mixamorig:RightShoulder", "mixamorig:RightArm", XMFLOAT3(0, 0, 0), 0.1f);
	CreateSingleBoneCollision("mixamorig:RightArm", "mixamorig:RightForeArm", XMFLOAT3(0, 0, 0), 0.1f);
	CreateSingleBoneCollision("mixamorig:RightForeArm", "mixamorig:RightHand", XMFLOAT3(0, 0, 0), 0.1f);
	
	//　左足
	CreateSingleBoneCollision("mixamorig:Hips", "mixamorig:LeftUpLeg", XMFLOAT3(0, 0, 0), 0.1f);
	CreateSingleBoneCollision("mixamorig:LeftUpLeg", "mixamorig:LeftLeg", XMFLOAT3(0, 0, 0), 0.1f);
	CreateSingleBoneCollision("mixamorig:LeftLeg", "mixamorig:LeftFoot", XMFLOAT3(0, 0, 0), 0.1f);
	CreateSingleBoneCollision("mixamorig:LeftFoot", "mixamorig:LeftToeBase", XMFLOAT3(0, 0, 0), 0.1f);
	CreateSingleBoneCollision("mixamorig:LeftToeBase", "mixamorig:LeftToe_End", XMFLOAT3(0, 0, 0), 0.1f);

	//	右足
	CreateSingleBoneCollision("mixamorig:Hips", "mixamorig:RightUpLeg", XMFLOAT3(0, 0, 0), 0.1f);
	CreateSingleBoneCollision("mixamorig:RightUpLeg", "mixamorig:RightLeg", XMFLOAT3(0, 0, 0), 0.1f);
	CreateSingleBoneCollision("mixamorig:RightLeg", "mixamorig:RightFoot", XMFLOAT3(0, 0, 0), 0.1f);
	CreateSingleBoneCollision("mixamorig:RightFoot", "mixamorig:RightToeBase", XMFLOAT3(0, 0, 0), 0.1f);
	CreateSingleBoneCollision("mixamorig:RightToeBase", "mixamorig:RightToe_End", XMFLOAT3(0, 0, 0), 0.1f);


}

void Character::CreateSingleBoneCollision(const std::string& Head, const std::string& Tail, const XMFLOAT3& Offset, const float Radius)
{
	if (m_BoneMap.empty())return;
	auto headBone = m_BoneMap.find(Head);
	auto tailBone = m_BoneMap.find(Tail);
	if (headBone != m_BoneMap.end() && tailBone != m_BoneMap.end())
	{
		std::string keyName = Head + Tail;
		m_Collisions.emplace(keyName,new CharacterBoneCollision(Head,Tail,headBone->second.HeadPosition, tailBone->second.HeadPosition, Offset, Radius));
	}
}
