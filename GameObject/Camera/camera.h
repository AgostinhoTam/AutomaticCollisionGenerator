#pragma once
/*===================================================================================

�J��������(camera.h)

====================================================================================*/
#include "GameObject/gameobject.h"
class Player;
class Camera:public GameObject 
{
private:

	XMFLOAT3			m_Target = XMFLOAT3{0.0f,0.0f,0.0f};				// �J�����̒����_
	XMFLOAT3			m_Up = XMFLOAT3{ 0.0f,0.0f,0.0f };;				// �J�����̏�����x�N�g��
	float				m_Len = 0.0f;			// �J�����̎��_�ƒ����_�̋���

	XMFLOAT4X4			m_MtxView{};		// �r���[�}�g���b�N�X
	XMMATRIX			m_MtxInvView{};		// �r���[�}�g���b�N�X
	XMMATRIX			m_MtxProjection{};	// �v���W�F�N�V�����}�g���b�N�X
	Player*				m_Player;
	float				m_Sensitivity = 0.0f;
public:
	void Init();
	void Update(const float& DeltaTime);
	void Uninit();
	void Draw();
	const XMMATRIX& GetViewMatrix() const { return XMLoadFloat4x4(&m_MtxView); }
};
