#include "Main\main.h"
#include "System\Collision\sphereCollision.h"
#include "System\Collision\characterBoneCollision.h"
#include "System\Renderer\animationModelInstance.h"
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

	//	キャッシュ
	std::unordered_map<int, XMFLOAT3> bonePosition;
	const std::unordered_map<std::string,int>& boneIndexMap = m_AnimationModel->GetBoneIndexMap();

	//	現在のMatrix取得
	XMMATRIX world, scale, rot, trans;

	scale = XMMatrixScaling(m_Scale.x, m_Scale.y, m_Scale.z);

	XMVECTOR quaternion = XMQuaternionRotationRollPitchYaw(m_Rotation.x, m_Rotation.y, m_Rotation.z);
	quaternion = XMQuaternionNormalize(quaternion);
	rot = XMMatrixRotationQuaternion(quaternion);

	trans = XMMatrixTranslation(m_Position.x, m_Position.y, m_Position.z);
	world = scale * rot * trans;

	//	コリジョン更新
	for (const auto& pair : m_Collisions)
	{
		if (!pair.second)continue;

		if (CharacterBoneCollision* boneCollision = dynamic_cast<CharacterBoneCollision*>(pair.second))
		{
			int headBoneIndex = boneCollision->GetHeadBoneIndex();
			int tailBoneIndex = boneCollision->GetTailBoneIndex();

			//	位置をキャシュー
			if (bonePosition.find(headBoneIndex) == bonePosition.end())
			{
				bonePosition.try_emplace(headBoneIndex, m_AnimationModel->GetBonePosition(headBoneIndex, world));
			}
			if (bonePosition.find(tailBoneIndex) == bonePosition.end())
			{
				bonePosition.try_emplace(tailBoneIndex, m_AnimationModel->GetBonePosition(tailBoneIndex, world));
			}

			boneCollision->UpdateBonePosition(headBoneIndex, tailBoneIndex, bonePosition[headBoneIndex], bonePosition[tailBoneIndex]);
		}
		else
		{
			pair.second->UpdateCollision(m_Position);
		}
	}

}

Animator*& Character::GetAnimator()
{
	return m_AnimationModel->GetAnimator();
}

void Character::CreateCharacterBoneCollision(const CHARACTER_BONE_TYPE& BoneType)
{
	if (!m_AnimationModel)return;

	const std::unordered_map<std::string, int>& BoneIndexMap = m_AnimationModel->GetBoneIndexMap();

	if (BoneIndexMap.empty())return;

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

		auto headit = BoneIndexMap.find(headBone);
		auto tailit = BoneIndexMap.find(tailBone);
		if (headit != BoneIndexMap.end() && tailit != BoneIndexMap.end())
		{
			CreateSingleBoneCollision(headit->first, tailit->first);
		}
	}
	return;
}

void Character::CreateCharacterBoneCollision(const std::string& FilePath)
{
	if (!m_AnimationModel)return;

	const std::unordered_map<std::string, int>& boneIndexMap = m_AnimationModel->GetBoneIndexMap();
	const std::vector<BONE>& bones = m_AnimationModel->GetBones();
	if (boneIndexMap.empty() || bones.empty())return;

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
	const std::unordered_map<std::string, int>& boneIndexMap = m_AnimationModel->GetBoneIndexMap();
	const std::vector<BONE>& bones = m_AnimationModel->GetBones();
	auto headit = boneIndexMap.find(Head);
	auto tailit = boneIndexMap.find(Tail);

	if (headit != boneIndexMap.end() && tailit != boneIndexMap.end())
	{

		std::string keyName = Head + " -> " + Tail;
		if (Radius == 0)	//　メッシュ計算する時
		{
			float radius = m_AnimationModel->CalculateCapsuleRadius(Head, Tail);
			radius *= m_Scale.x;
			m_Collisions.emplace(keyName, new CharacterBoneCollision(headit->second, tailit->second, bones[headit->second].HeadPosition, bones[tailit->second].HeadPosition, Offset, radius));
		}
		else //　計算しない時は指定
		{
			m_Collisions.emplace(keyName, new CharacterBoneCollision(headit->second, tailit->second, bones[headit->second].HeadPosition, bones[tailit->second].HeadPosition, Offset, Radius));
		}
	}
}

std::vector<std::string> Character::GetBoneMap()
{
	std::vector<std::string> boneMap;
	for (const auto& pair : m_Collisions)
	{
		if (!pair.second)continue;
		boneMap.emplace_back(pair.first);
	}
	return boneMap;
}

