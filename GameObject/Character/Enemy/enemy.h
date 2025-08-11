/*===================================================================================

エネミー制御(enemy.cpp)

====================================================================================*/
#pragma once
#include "GameObject/Character/character.h"
#include "System/Enum/enemyTypeEnum.h"

class BehaviorNode;
class Player;
class Enemy : public Character
{
public:
	Enemy(Enemy_Type EnemyType) :m_EnemyType(EnemyType){}
	virtual void Init()override;
	virtual void Uninit()override;
	virtual void Update(const float& DeltaTime)override;
	virtual void Draw()override;
	void CollisionCheck();
	float GetSenseDistance()const{return m_SenseDistance;}
	float GetAttackDistance()const{return m_SenseDistance;}
	float GetAttackCoolDownTime()const{return m_AttackCoolDownTime;}
	float GetChaseDistance()const{return m_ChaseDistance;}
private:
	BehaviorNode* m_BehaviorRoot{};
	Player* m_Player{};
	Enemy_Type m_EnemyType;
	float m_SenseDistance =0.0f;
	float m_AttackDistance =0.0f;
	float m_AttackCoolDownTime=0.0f;
	float m_ChaseDistance=0.0f;
};

