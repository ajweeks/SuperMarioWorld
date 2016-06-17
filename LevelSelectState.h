#pragma once

#include "BaseState.h"
#include "AnimationInfo.h"
#include "INT2.h"
#include <map>

class StateManager;

struct LevelDestinationNode
{
	static const int LEFT = 0;
	static const int RIGHT = 1;
	static const int TOP = 2;
	static const int BOTTOM = 3;

	INT2 m_Pos = { -1, -1};
	int m_ID = -1;
	std::string m_Name = "";
	// -1 if no neighbor in that direction
	int m_NeighborIDsArr[4] = { -1, -1, -1, -1}; 
};

class LevelSelectState : public BaseState
{
public:
	enum class AnimationState
	{
		STATIONARY, WALKING, ENTERING_LEVEL
	};

	LevelSelectState(StateManager* stateManagerPtr);
	virtual ~LevelSelectState();

	LevelSelectState(const LevelSelectState&) = delete;
	LevelSelectState& operator=(const LevelSelectState&) = delete;

	void Tick(double deltaTime);
	void Paint();

private:
	void TickAnimations(double deltaTime);
	void SetTargetID(int targetDestinationID, int direction);
	INT2 CalculateAnimationFrame();

	void ReadLevelSelectScreenDataFromFile();
	void ParseFileString(const std::string& fileString);
	int GetNumberOfDestinations(const std::string& fileString) const;
		LevelDestinationNode GetDestinationNode(const std::string& destinationString);

	AnimationInfo m_AnimInfoTopMario;
	AnimationInfo m_AnimInfoMapMario;

	DOUBLE2 m_CurrentMapMarioPosition;
	int m_CurrentDestinationIndex;
	int m_TargetDestinationIndex;
	// Keys are level IDs, values are node objects
	std::map<int, LevelDestinationNode> m_DestinationNodesMap;

	AnimationState m_AnimationState;
	int m_MarioDirectionFacing;
	SMWTimer m_DestinationTransitionTimer;
	DOUBLE2 m_MarioDestinationTransitionVel;

	SMWTimer m_FadeOutTimer;

};
