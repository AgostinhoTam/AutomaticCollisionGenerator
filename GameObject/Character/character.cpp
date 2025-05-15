/*===================================================================================

Character基底クラス(character.cpp)

====================================================================================*/
#include "Main\main.h"
#include "System\Renderer\animationModel.h"
#include "System\Collision\sphereCollision.h"
#include "System\Collision\characterBoneCollision.h"
#include <fstream>
#include <sstream>
#include "character.h"
//	=========================物理パラメータ=========================
constexpr float FRICTION = 0.8f;	//	摩擦力
constexpr float MAX_DROP_SPEED = -50.0f;	//	落ちるスピード上限
constexpr float GRAVITY = -9.8f;	//	重力
//	==============================================================

//	=========================キャラクター共通部分更新=========================
//	DeltaTime	:	float	デルタタイム
void Character::Update(const float& DeltaTime)
{
	//	SIMD用にVECTORに変換
	XMVECTOR position = XMLoadFloat3(&m_Position);
	XMVECTOR velocity = XMLoadFloat3(&m_Velocity);

	//	平面速度計算
	UpdateHorizontalVelocity(velocity, DeltaTime);

	//	上限速度計算
	UpdateVerticalVelocity(velocity, DeltaTime);

	//	位置計算
	position = XMVectorMultiplyAdd(velocity, XMVectorReplicate(DeltaTime), position);

	//	XMFLOAT3に戻す
	XMStoreFloat3(&m_Position, position);
	XMStoreFloat3(&m_Velocity, velocity);

	//	計算終わった後方向リセット
	m_MoveDirection = { 0,0,0 };
}

//	=========================上下移動計算=========================
//	Velocity	:	XMVECTOR	速度
//	DeltaTime	:	float		デルタタイム
void Character::UpdateVerticalVelocity(XMVECTOR& Velocity, const float& DeltaTime)
{
	float groundHeight = 0.0f;

	float velocityY = XMVectorGetY(Velocity);

	//	接地してるかどうか
	if (m_IsGround)
	{
		velocityY = 0.0f;
	}
	else
	{
		velocityY += GRAVITY * DeltaTime;
		//	上限越えないように
		velocityY = std::min(velocityY,MAX_DROP_SPEED);
	}
	//	Y設定
	Velocity = XMVectorSetY(Velocity, velocityY);
}

//	=========================左右移動=========================
//	Velocity	:	XMVECTOR	速度
//	DeltaTime	:	float		デルタタイム
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

//	=========================キャラクター用ボーンコリジョン更新=========================
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
			//	
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

//	=========================自動でキャラクターのボーンコリジョン設置関数（デフォルト用）=========================
//	BoneType	:	Character_Bone_Type	ボーンの種類
void Character::CreateCharacterBoneCollision(const Character_Bone_Type& BoneType)
{
	if (!m_AnimationModel)return;

	//	ボーンのインデックス取得
	const std::unordered_map<std::string, int>& BoneIndexMap = m_AnimationModel->GetBoneIndexMap();

	if (BoneIndexMap.empty())return;

	//	自動設置する前に現在設置しているコリジョンを削除
	if (!m_Collisions.empty())
	{
		for (auto& pair : m_Collisions)
		{
			if (!pair.second)continue;
			delete pair.second;
		}
	}
	m_Collisions.clear();

	//	ボーンのプロファイル読み込み
	std::ifstream file;
	switch (BoneType)
	{
	case Character_Bone_Type::Humanoid:
		file.open("asset\\boneProfile\\humanoidBone.csv");
		break;
	case Character_Bone_Type::Monster:
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

	//	文字列読み込み
	while (std::getline(file, line))
	{
		std::istringstream ss(line);
		std::string partName, headBone, tailBone;
		std::getline(ss, partName, ',');	//	,で分割しているデータを分ける
		
		//	データがなければ
		if (partName == "0")
		{
			break;
		}
		std::getline(ss, headBone, ',');		//	,で分割しているデータを分ける
		std::getline(ss, tailBone, ',');		//	,で分割しているデータを分ける

		//	取り出した名前にモデルのボーンインデックスと合わせる
		auto headit = BoneIndexMap.find(headBone);
		auto tailit = BoneIndexMap.find(tailBone);

		//	取り出した名前でコリジョン作成
		if (headit != BoneIndexMap.end() && tailit != BoneIndexMap.end())
		{
			CreateSingleBoneCollision(headit->first, tailit->first);
		}
		else
		{
			assert("BoneName Failure");	//	ボーンの名前が間違った場合はassert出す
		}
	}
}

//	=========================自動でキャラクターのボーンコリジョン設置関数（指定フォルダー用）=========================
//	FilePath	:	std::string	ファイルのパスを入力
void Character::CreateCharacterBoneCollision(const std::string& FilePath)
{
	if (!m_AnimationModel)return;
	
	//	ボーンのインデックス取得
	const std::unordered_map<std::string, int>& boneIndexMap = m_AnimationModel->GetBoneIndexMap();

	//	ボーンの情報取得
	const std::vector<BONE>& bones = m_AnimationModel->GetBones();
	
	if (boneIndexMap.empty() || bones.empty())return;
	
	//	自動設置する前に現在設置しているコリジョンを削除
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
	
	//	文字列読み込み
	while (std::getline(file, line))
	{
		std::istringstream ss(line);
		std::string partName, headBone, tailBone;
		std::getline(ss, partName, ',');	//	,で分割しているデータを分ける
		//	データがなければ
		if (partName == "0")
		{
			break;
		}
		std::getline(ss, headBone, ',');	//	,で分割しているデータを分ける
		std::getline(ss, tailBone, ',');	//	,で分割しているデータを分ける

		//	取り出した名前でコリジョン作成
		CreateSingleBoneCollision(headBone, tailBone);
	}
}

//	=========================自動でキャラクターのボーンコリジョン設置関数（指定フォルダー用）=========================
//	Head	:	std::string	始点のボーンの名前
//	Tail	:	std::string	終点のボーンの名前
//	Offset	:	XMFLOAT3	オフセット値	（デフォルトは0）
//	Radius	:	float		カプセルの半径	（デフォルトは0）
void Character::CreateSingleBoneCollision(const std::string& Head, const std::string& Tail, const XMFLOAT3& Offset, const float Radius)
{
	//	ボーンのインデックス取得
	const std::unordered_map<std::string, int>& boneIndexMap = m_AnimationModel->GetBoneIndexMap();

	//	ボーンの情報取得
	const std::vector<BONE>& bones = m_AnimationModel->GetBones();

	//	ボーンのマップから探す
	auto headit = boneIndexMap.find(Head);
	auto tailit = boneIndexMap.find(Tail);

	//	モデルにボーンそのボーンがあるなら
	if (headit != boneIndexMap.end() && tailit != boneIndexMap.end())
	{
		//	カプセルの名前をボーンの始点と終点にする
		std::string keyName = Head + " -> " + Tail;

		//	半径が０の時、もしくは何も入力されていない時に、自動でカプセルの半径を計算する
		if (Radius == 0)	//　メッシュ計算する時
		{
			//	半径を計算
			float radius = m_AnimationModel->CalculateCapsuleRadius(Head, Tail);
			//	スケール適用（モデルは1:1:1で拡大縮小処理するので1つの要素だけ取り出して計算）
			radius *= m_Scale.x;
			//	コリジョンリストに入れる
			m_Collisions.emplace(keyName, new CharacterBoneCollision(headit->second, tailit->second, bones[headit->second].HeadPosition, bones[tailit->second].HeadPosition, Offset, radius));
		}
		else //　指定の半径を入れる
		{
			m_Collisions.emplace(keyName, new CharacterBoneCollision(headit->second, tailit->second, bones[headit->second].HeadPosition, bones[tailit->second].HeadPosition, Offset, Radius));
		}
	}
}

//	=========================ボーンコリジョンマップ取り出す=========================
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

