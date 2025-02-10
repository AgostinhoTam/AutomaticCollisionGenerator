#pragma once
#include "collision.h"
class SphereCollision;
class CharacterBoneCollision :public Collision
{
public:
	CharacterBoneCollision(const std::string& HeadBoneName, const std::string& TailBoneName,const XMFLOAT3& Start,const XMFLOAT3& End, const XMFLOAT3& Offset, float Radius);
	virtual bool IsCollisionOverlapping(const Collision* Collision) override;
	bool CheckSphereToSphere(const SphereCollision* Collision);
	bool CheckCapsuleToCapsule(const CharacterBoneCollision* Collision);
	float CheckDistanceSegmentToSegment(const XMVECTOR& Start1, const XMVECTOR& End1, const XMVECTOR& Start2, const XMVECTOR& End2);
	float GetRadius()const { return m_Radius; }
	virtual void UpdateCollision(const XMFLOAT3& Position)override;
	void UpdateBonePosition(const std::string& BoneName, const XMFLOAT3& Position);
	virtual void Init()override;
	virtual void Draw()override;
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

