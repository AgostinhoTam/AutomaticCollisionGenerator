#pragma once
#include "GameObject\Character\character.h"
#include "System\Enum\enemyTypeEnum.h"
class BehaviorNode;
class Enemy : public Character
{
public:
	Enemy(ENEMY_TYPE EnemyType);
	virtual void Init()override;
	virtual void Uninit()override;
	virtual void Update(const float& DeltaTime)override;
	virtual void Draw()override;
private:
	BehaviorNode* m_BehaviorRoot{};
};

