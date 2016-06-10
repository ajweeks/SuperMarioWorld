#pragma once

#include <string>

struct SessionInfo
{
	std::string m_Date = "";
	std::string m_Time = "";
	std::string m_PlayerPowerupState = "";
	std::string m_HeldItemType = "";
	int m_PlayerLives = -1;
	int m_PlayerScore = -1;
	int m_RedStarsCollected = -1;
	int m_TimeRemaining = -1;
	int m_CoinsCollected = -1;
	int m_CheckpointCleared = -1;
	int m_PlayerRidingYoshi = -1;
	int m_DragonCoinsCollected = -1;
};