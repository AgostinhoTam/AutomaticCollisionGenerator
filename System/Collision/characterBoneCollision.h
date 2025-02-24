#pragma once
#include "collision.h"
class SphereCollision;
class CharacterBoneCollision :public Collision
{
private:
	XMFLOAT3 m_StartPosition;
	XMFLOAT3 m_EndPosition;
	int m_HeadBoneIndex;
	int m_TailBoneIndex;
	float m_Radius{};
	std::vector<LINE_VERTEX> m_CylinderLineVertices;
	std::vector<LINE_VERTEX> m_StartSphereVertices;
	std::vector<LINE_VERTEX> m_EndSphereVertices;
	ID3D11Buffer* m_CylinderBuffer;
	ID3D11Buffer* m_StartSphereBuffer;
	ID3D11Buffer* m_EndSphereBuffer;
public:
	CharacterBoneCollision(const int HeadBoneIndex, const int TailBoneIndex,const XMFLOAT3& Start,const XMFLOAT3& End, const XMFLOAT3& Offset, float Radius);
	virtual bool IsCollisionOverlapping(const Collision* Collision) override;
	bool CheckSphereToSphere(const SphereCollision* Collision);
	bool CheckCapsuleToCapsule(const CharacterBoneCollision* Collision);
	float CheckDistanceSegmentToSegment(const XMVECTOR& Start1, const XMVECTOR& End1, const XMVECTOR& Start2, const XMVECTOR& End2);
	float GetRadius()const { return m_Radius; }
	void SetRadius(const float Radius) { m_Radius = Radius; }
	virtual void UpdateCollision(const XMFLOAT3& Position)override;
	void UpdateBonePosition(const int FirstIndex,const int SecondIndex, const XMFLOAT3& HeadPos, const XMFLOAT3& TailPos);
	virtual void Init()override;
	virtual void Draw()override;
	void CreateSphereLine(const XMFLOAT4& Color,std::vector<LINE_VERTEX>& SphereVertices);
	void CreateCylinderLine(const XMFLOAT4& Color, std::vector<LINE_VERTEX>& CylinderVertices);
	void CreateBufferVertices(const std::vector<LINE_VERTEX>& Vertices, ID3D11Buffer*& Buffer);
	void MakeSphereMatrix(const XMFLOAT3& Position);
	void MakeCapsuleMatrix(const XMFLOAT3& StartPos, const XMFLOAT3& EndPos);
	int GetHeadBoneIndex() const{ return m_HeadBoneIndex; }
	int GetTailBoneIndex() const{ return m_TailBoneIndex; }
};

