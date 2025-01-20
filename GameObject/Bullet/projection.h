#pragma once
#include "System\Enum\projectionTypeEnum.h"
#include "GameObject\gameobject.h"

class Collision;
class Projection : public GameObject
{
public:
	Projection(){}
	~Projection(){}
	virtual void Init()override;
	virtual void Update(const float& DeltaTime)override;
	virtual void Draw()override;
	virtual void Uninit()override;
	void SetProjectionType(PROJECTION_TYPE& Type) { m_ProjectionType = Type; }
protected:
	PROJECTION_TYPE m_ProjectionType;
	Collision* m_Collision{};
};