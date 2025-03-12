#pragma once
#include <string>
#include <any>
#include <unordered_map>
#include "System\Enum\behaviorTreeEnum.h"

class BlackBoard
{
private:
	std::unordered_map<BEHAVIOR_CONDITION, std::any> m_Data;
public:
	template<typename T>
	void SetValue(const BEHAVIOR_CONDITION& Key, const T& Value)
	{
		m_Data[static_cast<BEHAVIOR_CONDITION>(Key)] = Value;
	}

	template<typename T>
	const T GetValue(const BEHAVIOR_CONDITION& Key)const
	{
		auto it = m_Data.find(static_cast<BEHAVIOR_CONDITION>(Key));

		if (it == m_Data.end())
		{
			return T{};
		}
		return std::any_cast<T>(it->second);	
	}

	virtual void Uninit()
	{
		m_Data.clear();
	}

	bool IsPlayerInAttackRange();
	bool IsPlayerInSight();
	bool IsIdle();
	bool IsAttackCoolTime();
};