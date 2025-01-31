#pragma once
#include "collision.h"
class CharacterBoneCollision :public Collision
{
public:
	CharacterBoneCollision(const std::string& HeadBoneName, const std::string& TailBoneName,const XMFLOAT3& Start,const XMFLOAT3& End, const XMFLOAT3& Offset, float Radius);
	virtual bool IsCollisionOverlapping(const Collision* Collision)const override;
	bool CheckSphereToSphere(const CharacterBoneCollision* Collision)const;
	float GetRadius()const { return m_Radius; }
	virtual void UpdateCollision(const XMFLOAT3& Position)override;
	void UpdateBonePosition(const std::string& BoneName, const XMFLOAT3& Position);
	virtual void Init()override;
	virtual void Draw()override;
	void CreateCapsuleLineVertex();
	void CreateSphereLine(const XMFLOAT4& Color,std::vector<LINE_VERTEX>& SphereVertices);
	void CreateCylinderLine(const XMFLOAT4& Color, std::vector<LINE_VERTEX>& CylinderVertices);
	void CreateBufferVertices(const std::vector<LINE_VERTEX>& Vertices, ID3D11Buffer*& Buffer);
	void MakeSphereMatrix(const XMFLOAT3& Position);
	void MakeCapsuleMatrix(const XMFLOAT3& StartPos, const XMFLOAT3& EndPos);
	std::string GetHeadBoneName() { return m_HeadBoneName; }
	std::string GetTailBoneName() { return m_TailBoneName; }
private:
	XMFLOAT3 m_StartPosition;
	XMFLOAT3 m_EndPosition;
	std::string m_HeadBoneName;
	std::string m_TailBoneName;
	float m_Radius{};
	std::vector<LINE_VERTEX> m_CylinderLineVertices;
	std::vector<LINE_VERTEX> m_StartSphereVertices;
	std::vector<LINE_VERTEX> m_EndSphereVertices;
	ID3D11Buffer* m_CylinderBuffer;
	ID3D11Buffer* m_StartSphereBuffer;
	ID3D11Buffer* m_EndSphereBuffer;
};

