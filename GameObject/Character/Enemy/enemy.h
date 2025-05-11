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
private:
	BehaviorNode* m_BehaviorRoot{};
	Player* m_Player{};
	Enemy_Type m_EnemyType;
};

