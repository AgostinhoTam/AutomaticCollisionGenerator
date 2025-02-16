#pragma once
#include "GameObject/gameobject.h"
#include "System\Enum/moveDirection.h"
#include "System\Renderer\animationModel.h"

enum class CHARACTER_BONE_TYPE
{
	HUMANOID,
	MONSTER,
	MAX_CHARACTER_TYPES
};
struct MODEL;
class Collision;
class AnimationModel;
class Character :public GameObject
{
protected:
	XMFLOAT3 m_Velocity{};
	XMFLOAT3 m_Accl{};
	AnimationModel* m_AnimationModel{};
	XMFLOAT3 m_MoveDirection{};
	float	m_MaxMovementSpeed = 0;
	float	m_MaxHorizontalAcclSpeed = 0;
	float	m_MaxJumpSpeed = 0;
	bool	m_IsGround = false;
	std::unordered_map<std::string, Collision*> m_Collisions;
	std::unordered_map<std::string, BONE> m_BoneMap;
public:
	virtual void Init()override{}
	virtual void Update(const float& DeltaTime)override;
	virtual void Draw()override{}
	virtual void Uninit()override{}
	const XMFLOAT3& GetVelocity() const { return m_Velocity; }
	const XMFLOAT3& GetAccl()const { return m_Accl; }
	const XMFLOAT3& GetMoveDirection()const { return m_MoveDirection; }
	const float GetMaxMovementSpeed()const { return m_MaxMovementSpeed; }
	void SetMoveForwardDirection(float forward) { m_MoveDirection.z = forward; }
	void SetMoveRightDirection(float right) { m_MoveDirection.x = right; }
	void SetMoveDirection(XMFLOAT3 direction) { m_MoveDirection = direction; }
	void UpdateVerticalVelocity(XMVECTOR& Velocity, const float& DeltaTime);
	void UpdateHorizontalVelocity(XMVECTOR& Velocity, const float& DeltaTime);
	void UpdateBoneCollision();
	AnimationModel* GetAnimationModel() const{ return m_AnimationModel; }
	void CreateCharacterBoneCollision(const CHARACTER_BONE_TYPE& BoneType);
	void CreateCharacterBoneCollision(const std::string& FilePath);
	void CreateSingleBoneCollision(const std::string& Head,const std::string& Tail,const XMFLOAT3& Offset={0.0f,0.0f,0.0f}, const float Radius = 0.0f);
	std::unordered_map<std::string, Collision*>& GetCollisionList() { return m_Collisions; }
	std::vector<std::string> GetBoneMap();
	Collision* GetSelectedCollision(const std::string& KeyName) { return m_Collisions[KeyName]; }
};