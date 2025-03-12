#include "behaviorNode.h"

void BehaviorNode::AddChildNode(BehaviorNode* Node)
{
	m_Child.emplace_back(Node);
}

BEHAVIOR_RESULT BehaviorSelector::Update(const float DeltaTime)
{
	while (m_CurrentIndex < m_Child.size())
	{
		BEHAVIOR_RESULT result = m_Child[m_CurrentIndex]->Update(DeltaTime);
		if (result == BEHAVIOR_RESULT::RUNNING)
		{
			return BEHAVIOR_RESULT::RUNNING;
		}
		if (result == BEHAVIOR_RESULT::SUCCESS)
		{
			m_CurrentIndex = 0;
			return BEHAVIOR_RESULT::SUCCESS;
		}
		++m_CurrentIndex;
	}
	m_CurrentIndex = 0;
	return BEHAVIOR_RESULT::FAILURE;
}


BEHAVIOR_RESULT BehaviorSequence::Update(const float DeltaTime)	//	ëSïîê¨å˜ÇµÇΩÇÁê¨å˜ï‘Ç∑ÅAÇªÇÍà»äOÇÕFAILURE
{

	while (m_CurrentIndex < m_Child.size())
	{
		BEHAVIOR_RESULT result = m_Child[m_CurrentIndex]->Update(DeltaTime);

		if (result == BEHAVIOR_RESULT::RUNNING)
		{
			return BEHAVIOR_RESULT::RUNNING;
		}
		if (result == BEHAVIOR_RESULT::FAILURE)
		{
			m_CurrentIndex = 0;
			return BEHAVIOR_RESULT::FAILURE;
		}
		++m_CurrentIndex;
	}
	m_CurrentIndex = 0;
	return BEHAVIOR_RESULT::SUCCESS;
}
