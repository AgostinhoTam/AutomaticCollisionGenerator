#include "Manager\animationRendererManager.h"
#include "Manager\shaderManager.h"
#include "System\Renderer\animationModel.h"
#include "enemy.h"
namespace EnemyTypeA
{
	constexpr float MAX_ENEMY_SPEED = 20.0f;
	constexpr float ENEMY_MAX_ACCL_SPEED = 50.0f;
	constexpr float ENEMY_MAX_JUMP_SPEED = 100.0f;
	constexpr float ENEMY_SCALE = 0.01f;
}

Enemy::Enemy(ENEMY_TYPE EnemyType)
{
	switch (EnemyType)
	{
	case ENEMY_TYPE::ENEMY:
		m_Name = "Enemy";
		m_AnimationModel = AnimationRendererManager::LoadAnimationModel(MODEL_NAME::ENEMY);
		m_MaxMovementSpeed = EnemyTypeA::MAX_ENEMY_SPEED;
		m_MaxHorizontalAcclSpeed = EnemyTypeA::ENEMY_MAX_ACCL_SPEED;
		m_Scale = { EnemyTypeA::ENEMY_SCALE,EnemyTypeA::ENEMY_SCALE,EnemyTypeA::ENEMY_SCALE };
		break;
	}
}
void Enemy::Init()
{

	m_Shader = ShaderManager::LoadShader(SHADER_NAME::UNLIT_TEXTURE);

	//	‚Æ‚è‚ ‚¦‚¸Ú’n
	m_Position.y = 0;
	m_IsGround = true;

}

void Enemy::Uninit()
{
	if (!m_AnimationModel)return;
	m_AnimationModel->Uninit();
}

void Enemy::Update(const float& DeltaTime)
{
	if (!m_AnimationModel)return;

	Character::Update(DeltaTime);
}

void Enemy::Draw()
{
	if (!m_AnimationModel)return;
	m_AnimationModel->Draw(this);
}
