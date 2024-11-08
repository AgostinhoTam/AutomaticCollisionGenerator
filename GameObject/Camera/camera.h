#pragma once
/*===================================================================================

�J��������(camera.h)

====================================================================================*/
#include "GameObject/gameobject.h"

class Camera:public GameObject 
{
private:
	XMFLOAT3			_Position;			// �J�����̎��_(�ʒu)
	XMFLOAT3			_Target;				// �J�����̒����_
	XMFLOAT3			_Up;				// �J�����̏�����x�N�g��
	XMFLOAT3			_rot;			// �J�����̉�]
	float				_len;			// �J�����̎��_�ƒ����_�̋���

	XMFLOAT4X4			_mtxView;		// �r���[�}�g���b�N�X
	XMMATRIX			_mtxInvView;		// �r���[�}�g���b�N�X
	XMMATRIX			_mtxProjection;	// �v���W�F�N�V�����}�g���b�N�X
public:
	void Init();
	void Update();
	void Uninit();
	void Draw();
};
