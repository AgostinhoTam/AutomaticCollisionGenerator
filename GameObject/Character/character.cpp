#include "Main\main.h"
#include "System\Renderer\animationModel.h"
#include "System\Collision\characterBoneCollision.h"
#include <fstream>
#include <sstream>
#include "character.h"
constexpr float FRICTION = 0.8f;
constexpr float MAX_DROP_SPEED = -50.0f;
constexpr float GRAVITY = -9.8f;
void Character::Update(const float& DeltaTime)
{
	XMVECTOR position = XMLoadFloat3(&m_Position);
	XMVECTOR velocity = XMLoadFloat3(&m_Velocity);

	UpdateHorizontalVelocity(velocity, DeltaTime);

	UpdateVerticalVelocity(velocity, DeltaTime);

	position = XMVectorMultiplyAdd(velocity, XMVectorReplicate(DeltaTime), position);

	XMStoreFloat3(&m_Position, position);
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
	Velocity = XMVectorSetY(Velocity, velocityY);
}

void Character::UpdateHorizontalVelocity(XMVECTOR& Velocity, const float& DeltaTime)
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

	for (auto& pair : m_Collisions)
	{
		if (!pair.second)continue;
		const std::string& boneKey = pair.first;
		XMFLOAT3 headPos = m_AnimationModel->GetHeadPosition(boneKey, m_Scale, world);
		CharacterBoneCollision* boneCharacter = dynamic_cast<CharacterBoneCollision*>(pair.second);
		if (boneCharacter)
		{
			boneCharacter->UpdateBonePosition(boneKey, headPos);
		}
		else
		{
			pair.second->UpdateCollision(m_Position);
		}

	}

}
//	初期プロフィール読み込み
void Character::CreateCharacterBoneCollision(const CHARACTER_BONE_TYPE& BoneType)
{
	if (!m_AnimationModel)return;
	m_BoneMap = m_AnimationModel->GetBoneMap();
	if (m_BoneMap.empty())return;

	//	最初全部消す
	if (!m_Collisions.empty())
	{
		for (auto& pair : m_Collisions)
		{
			if (!pair.second)continue;
			delete pair.second;
		}
	}
	m_Collisions.clear();

	std::ifstream file;
	switch (BoneType)
	{
	case CHARACTER_BONE_TYPE::HUMANOID:
		file.open("asset\\boneProfile\\humanoidBone.csv");
		break;
	case CHARACTER_BONE_TYPE::MONSTER:
		file.open("asset\\boneProfile\\monsterBone.csv");
		break;
	default:
		file.open("asset\\boneProfile\\humanoidBone.csv");
		break;
	}
	if (!file.is_open())
	{
		return;
	}

	std::string line;
	std::getline(file, line);	//	最初の一列スキップ

	while (std::getline(file, line))
	{
		std::istringstream ss(line);
		std::string partName, headBone, tailBone;
		std::getline(ss, partName, ',');
		//	データがなければ
		if (partName == "0")
		{
			break;
		}
		std::getline(ss, headBone, ',');
		std::getline(ss, tailBone, ',');

		CreateSingleBoneCollision(headBone, tailBone);
	}
	return;
}

//	実行中プロフィール読み込み
void Character::CreateCharacterBoneCollision(const std::string& FilePath)
{
	if (!m_AnimationModel)return;
	m_BoneMap = m_AnimationModel->GetBoneMap();
	if (m_BoneMap.empty())return;

	//	最初全部消す
	if (!m_Collisions.empty())
	{
		for (auto& pair : m_Collisions)
		{
			if (!pair.second)continue;
			delete pair.second;
		}
	}
	m_Collisions.clear();
	std::ifstream file;

	file.open(FilePath);

	if (!file.is_open())
	{
		return;
	}

	std::string line;
	std::getline(file, line);	//	最初の一列スキップ

	while (std::getline(file, line))
	{
		std::istringstream ss(line);
		std::string partName, headBone, tailBone;
		std::getline(ss, partName, ',');
		//	データがなければ
		if (partName == "0")
		{
			break;
		}
		std::getline(ss, headBone, ',');
		std::getline(ss, tailBone, ',');

		CreateSingleBoneCollision(headBone, tailBone);
	}
	return;
}

void Character::CreateSingleBoneCollision(const std::string& Head, const std::string& Tail, const XMFLOAT3& Offset, const float Radius)
{
	if (!m_AnimationModel)return;
	if (m_BoneMap.empty())return;

	// ワールド行列作成（更新前の初期状態として）
	XMMATRIX scale = XMMatrixScaling(m_Scale.x, m_Scale.y, m_Scale.z);
	XMVECTOR quat = XMQuaternionRotationRollPitchYaw(m_Rotation.x, m_Rotation.y, m_Rotation.z);
	quat = XMQuaternionNormalize(quat);
	XMMATRIX rot = XMMatrixRotationQuaternion(quat);
	XMMATRIX trans = XMMatrixTranslation(m_Position.x, m_Position.y, m_Position.z);
	XMMATRIX world = scale * rot * trans;

	const XMFLOAT3& headPos = m_AnimationModel->GetHeadPosition(Head, m_Scale, world);
	const XMFLOAT3& tailPos = m_AnimationModel->GetHeadPosition(Tail, m_Scale, world);

	std::string keyName = Head + " -> " + Tail;
	if (Radius == 0)	//　メッシュ計算する時
	{
		float radius = m_AnimationModel->CalculateCapsuleRadius(Head, Tail);
		radius *= m_Scale.x;
		m_Collisions.emplace(keyName, new CharacterBoneCollision(Head, Tail, headPos, tailPos, Offset, radius));
	}
	else //　計算しない時は指定
	{
		m_Collisions.emplace(keyName, new CharacterBoneCollision(Head, Tail, headPos, tailPos, Offset, Radius));
	}
}


std::vector<std::string> Character::GetBoneMap() const
{
	return std::vector<std::string>();
}

