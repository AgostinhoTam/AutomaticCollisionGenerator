#pragma once
#include "GameObject\Character\character.h"
#include "System\Enum\behaviorTreeEnum.h"
#include "System\Enum\enemyTypeEnum.h"
class BehaviorNode;
class ConditionManager;
class Player;
class BlackBoard;

struct ENEMY_DATA
{
	ENEMY_TYPE Type;
	float SightRange;
	float AttackRange;
	float AttackSpeed;
	float Health;
};

class Enemy : public Character
{

private:
	BehaviorNode* m_BehaviorRoot{};
	Player* m_Player{};
	ConditionManager* m_ConditionManager{};
	BlackBoard* m_BlackBoard{};
	ENEMY_DATA m_EnemyData{};

public:
	Enemy(const ENEMY_TYPE& EnemyType);
	virtual void Init()override;
	virtual void Uninit()override;
	virtual void Update(const float& DeltaTime)override;
	virtual void Draw()override;
	void CollisionCheck();
	void BlackBoardUpdate();
	void SetEnemyType(const ENEMY_TYPE& Type);
	BlackBoard*& GetBlackBoard() { return m_BlackBoard; }
	void FaceTargetDirection();
	void MoveToTargetDirection();
	float GetTargetDistance();
};

