#include "stdafx.h"

#include "LevelSelectState.h"
#include "Game.h"
#include "SpriteSheetManager.h"
#include "SpriteSheet.h"
#include "SMWFont.h"
#include "HUD.h"
#include "FileIO.h"
#include "Keybindings.h"
#include "GameState.h"
#include "StateManager.h"

LevelSelectState::LevelSelectState(StateManager* stateManagerPtr) :
	BaseState(stateManagerPtr, StateType::LEVEL_SELECT)
{
	m_AnimInfoMapMario.secondsPerFrame = 0.14;
	m_AnimInfoTopMario.secondsPerFrame = 0.14;

	ReadLevelSelectScreenDataFromFile();

	m_CurrentDestinationIndex = 0;
	m_CurrentMapMarioPosition.x = m_DestinationNodesArr[m_CurrentDestinationIndex].m_Pos.x;
	m_CurrentMapMarioPosition.y = m_DestinationNodesArr[m_CurrentDestinationIndex].m_Pos.y;
	m_TargetDestinationIndex = -1;

	m_AnimationState = AnimationState::STATIONARY;
	m_MarioDirectionFacing = 0;

	m_FadeOutTimer = SMWTimer(60);
}

LevelSelectState::~LevelSelectState()
{
}

void LevelSelectState::Tick(double deltaTime)
{
	TickAnimations(deltaTime);

	if (m_FadeOutTimer.Tick() && m_FadeOutTimer.IsComplete())
	{
		m_StateManagerPtr->SetState(new GameState(m_StateManagerPtr));
		return;
	}

	if (m_TargetDestinationIndex != -1) // Mario is walking from one destination to another
	{
		m_DestinationTransitionTimer.Tick();

		m_CurrentMapMarioPosition.x += m_MarioDestinationTransitionVel.x;
		m_CurrentMapMarioPosition.y += m_MarioDestinationTransitionVel.y;

		if (m_DestinationTransitionTimer.IsComplete())
		{
			m_AnimationState = AnimationState::STATIONARY;
			m_CurrentDestinationIndex = m_TargetDestinationIndex;
			m_TargetDestinationIndex = -1;
			m_MarioDestinationTransitionVel = DOUBLE2(0, 0);

			SoundManager::PlaySoundEffect(SoundManager::Sound::LEVEL_SELECT_NODE_STOMP);
		}
	}
	else // Mario is standing still
	{
		if (GAME_ENGINE->IsKeyboardKeyPressed(Keybindings::A_BUTTON) ||
			GAME_ENGINE->IsKeyboardKeyPressed(Keybindings::B_BUTTON) ||
			GAME_ENGINE->IsKeyboardKeyPressed(Keybindings::X_BUTTON) ||
			GAME_ENGINE->IsKeyboardKeyPressed(Keybindings::Y_BUTTON))
		{
			m_AnimationState = AnimationState::ENTERING_LEVEL;
			m_FadeOutTimer.Start();
		}
		else if (GAME_ENGINE->IsKeyboardKeyPressed(Keybindings::D_PAD_LEFT))
		{
			const int leftNeighborID = m_DestinationNodesArr[m_CurrentDestinationIndex].m_NeighborIDsArr[LevelDestinationNode::LEFT];
			if (leftNeighborID != -1) SetTargetID(leftNeighborID, LevelDestinationNode::LEFT);
		}
		else if (GAME_ENGINE->IsKeyboardKeyPressed(Keybindings::D_PAD_RIGHT))
		{
			const int rightNeighborID = m_DestinationNodesArr[m_CurrentDestinationIndex].m_NeighborIDsArr[LevelDestinationNode::RIGHT];
			if (rightNeighborID != -1) SetTargetID(rightNeighborID, LevelDestinationNode::RIGHT);
		}
		else if (GAME_ENGINE->IsKeyboardKeyPressed(Keybindings::D_PAD_UP))
		{
			const int topNeighborID = m_DestinationNodesArr[m_CurrentDestinationIndex].m_NeighborIDsArr[LevelDestinationNode::TOP];
			if (topNeighborID != -1) SetTargetID(topNeighborID, LevelDestinationNode::TOP);
		}
		else if (GAME_ENGINE->IsKeyboardKeyPressed(Keybindings::D_PAD_DOWN))
		{
			const int bottomNeighborID = m_DestinationNodesArr[m_CurrentDestinationIndex].m_NeighborIDsArr[LevelDestinationNode::BOTTOM];
			if (bottomNeighborID != -1) SetTargetID(bottomNeighborID, LevelDestinationNode::BOTTOM);
		}
	}
}

void LevelSelectState::SetTargetID(int targetDestinationID, int direction)
{
	m_AnimationState = AnimationState::WALKING;
	m_TargetDestinationIndex = targetDestinationID;
	m_MarioDirectionFacing = direction;

	const double dx = m_DestinationNodesArr[m_TargetDestinationIndex].m_Pos.x - m_CurrentMapMarioPosition.x;
	const double dy = m_DestinationNodesArr[m_TargetDestinationIndex].m_Pos.y - m_CurrentMapMarioPosition.y;

	const int numOfFrames = int(sqrt(dx * dx + dy * dy) * 1.25);

	m_MarioDestinationTransitionVel.x = dx / numOfFrames;
	m_MarioDestinationTransitionVel.y = dy / numOfFrames;

	m_DestinationTransitionTimer = SMWTimer(numOfFrames);
	m_DestinationTransitionTimer.Start();
}

void LevelSelectState::TickAnimations(double deltaTime)
{
	m_AnimInfoTopMario.Tick(deltaTime);
	m_AnimInfoTopMario.frameNumber %= 2;

	m_AnimInfoMapMario.Tick(deltaTime);
	switch (m_AnimationState)
	{
	case AnimationState::STATIONARY:
	{
		m_AnimInfoMapMario.frameNumber %= 4;
	} break;
	case AnimationState::WALKING:
	{
		m_AnimInfoMapMario.frameNumber %= 2;
	} break;
	case AnimationState::ENTERING_LEVEL:
	{
		m_AnimInfoMapMario.frameNumber = 0;
	} break;
	}
}

void LevelSelectState::Paint()
{
	MATRIX3X2 matPrevWorld = GAME_ENGINE->GetWorldMatrix();

	GAME_ENGINE->DrawBitmap(SpriteSheetManager::GetBitmapPtr(SpriteSheetManager::LEVEL_SELECT_WINDOW));
	GAME_ENGINE->DrawBitmap(SpriteSheetManager::GetBitmapPtr(SpriteSheetManager::LEVEL_SELECT_BACKGROUND), 17, 40);

	SMWFont::PaintPhrase("YOSHI'S HOUSE", 87, 30, SMWFont::INVERTED);

	HUD::PaintSeveralDigitNumber(64, 31, 5);
	
	{
		// Top mario
		const int topMarioCenterX = 38;
		const int topMarioCenterY = 22;
		MATRIX3X2 matTranslate = MATRIX3X2::CreateTranslationMatrix(topMarioCenterX, topMarioCenterY);
		MATRIX3X2 matReflection = MATRIX3X2::CreateScalingMatrix(-1, 1);
		MATRIX3X2 matTotalWorld = matTranslate.Inverse() * matReflection * matTranslate;
		GAME_ENGINE->SetWorldMatrix(matTotalWorld);
		SpriteSheetManager::GetSpriteSheetPtr(SpriteSheetManager::SMALL_MARIO)->Paint(topMarioCenterX, topMarioCenterY, 2 + m_AnimInfoTopMario.frameNumber, 0);
		GAME_ENGINE->SetWorldMatrix(matPrevWorld);
	}

	// Map mario
	const double mapMarioCenterX = m_CurrentMapMarioPosition.x;
	const double mapMarioCenterY = m_CurrentMapMarioPosition.y;
	if (m_MarioDirectionFacing == LevelDestinationNode::LEFT)
	{
		MATRIX3X2 matTranslate = MATRIX3X2::CreateTranslationMatrix(mapMarioCenterX, mapMarioCenterY);
		MATRIX3X2 matReflection = MATRIX3X2::CreateScalingMatrix(-1, 1);
		MATRIX3X2 matTotalWorld = matTranslate.Inverse() * matReflection * matTranslate;
		GAME_ENGINE->SetWorldMatrix(matTotalWorld);
	}
	const INT2 animationFrame = CalculateAnimationFrame();
	SpriteSheetManager::GetSpriteSheetPtr(SpriteSheetManager::LEVEL_SELECT_MARIO)->Paint(mapMarioCenterX, mapMarioCenterY, animationFrame.x, animationFrame.y);
	GAME_ENGINE->SetWorldMatrix(matPrevWorld);

	if (m_FadeOutTimer.IsActive())
	{
		int alpha = min(int((double(m_FadeOutTimer.FramesElapsed()) / double(m_FadeOutTimer.TotalFrames() - 10)) * 255.0), 255);
		GAME_ENGINE->SetColor(COLOR(0, 0, 0, alpha));
		GAME_ENGINE->FillRect(0, 0, Game::WIDTH, Game::HEIGHT);

		if (m_FadeOutTimer.FramesElapsed() > m_FadeOutTimer.TotalFrames() - 10)
		{
			SpriteSheetManager::GetSpriteSheetPtr(SpriteSheetManager::TITLE_TEXT)->Paint(Game::WIDTH / 2, Game::HEIGHT / 2, 0, 0);
		}
	}
}

INT2 LevelSelectState::CalculateAnimationFrame()
{
	switch (m_AnimationState)
	{
	case AnimationState::STATIONARY:
	{
		int mapMarioFrameNumber = m_AnimInfoMapMario.frameNumber;
		if (mapMarioFrameNumber == 3) mapMarioFrameNumber = 1; // Ping pong: [0, 1, 2, 1, ...]
		return INT2(0 + mapMarioFrameNumber, 0);
	} break;
	case AnimationState::WALKING:
	{
		return INT2(3 + m_AnimInfoMapMario.frameNumber, 0);
	} break;
	case AnimationState::ENTERING_LEVEL:
	{
		return INT2(8, 0);
	} break;
	}
	return INT2(0, 0);
}

void LevelSelectState::ReadLevelSelectScreenDataFromFile()
{
	const std::string filePath = "Resources/level-select-screen-data.xml";

	std::ifstream fileInStream;
	std::string totalFileString;

	fileInStream.open(filePath);
	if (!fileInStream) // The file does not yet exist
	{
		std::ofstream fileOutStream;
		fileOutStream.open(filePath); // Create the file
		fileOutStream.close();
	}

	if (fileInStream.fail() == false)
	{
		std::string line;
		std::stringstream stringStream;
		while (fileInStream.eof() == false)
		{
			std::getline(fileInStream, line);
			stringStream << line;
		}

		totalFileString = stringStream.str();
	}

	fileInStream.close();

	// Remove non-quoted whitespace
	totalFileString.erase(std::remove_if(totalFileString.begin(), totalFileString.end(), IsWhitespace()), totalFileString.end());
	ParseFileString(totalFileString);
}

void LevelSelectState::ParseFileString(const std::string& fileString)
{
	const size_t numberOfDestinations = GetNumberOfDestinations(fileString);
	m_DestinationNodesArr.resize(numberOfDestinations);

	int currentIndexInFileString = 0;
	for (size_t i = 0; i < numberOfDestinations; ++i)
	{
		const int destinationStartTag = currentIndexInFileString;
		const int destinationEndTag = fileString.find("</Destination>", currentIndexInFileString);

		if (destinationStartTag != std::string::npos && destinationEndTag != std::string::npos)
		{
			const std::string destinationStr = fileString.substr(destinationStartTag, destinationEndTag - destinationStartTag);
			m_DestinationNodesArr[i] = GetDestinationNode(destinationStr);
			currentIndexInFileString = destinationEndTag + std::string("</Destination>").length();
		}
	}
	int x = 1;
	++x;
}

int LevelSelectState::GetNumberOfDestinations(const std::string& fileString) const
{
	int sessionsFound = 0;
	int currentIndex = -1;
	while ((currentIndex = fileString.find("<Destination>", currentIndex + 1)) != std::string::npos)
	{
		++sessionsFound;
	}
	return sessionsFound;
}

LevelDestinationNode LevelSelectState::GetDestinationNode(const std::string& destinationString)
{
	LevelDestinationNode destinationNode = {};

	const std::string pixelCoordStr = FileIO::GetTagContent(destinationString, "PixelCoordinate");
	if (pixelCoordStr.length() > 0)  destinationNode.m_Pos = FileIO::StringToINT2(pixelCoordStr);

	const std::string nameStr = FileIO::GetTagContent(destinationString, "Name");
	if (nameStr.length() > 0)  destinationNode.m_Name = nameStr;

	const std::string idStr = FileIO::GetTagContent(destinationString, "ID");
	if (idStr.length() > 0)  destinationNode.m_ID = stoi(idStr);

	const std::string neighborsStr = FileIO::GetTagContent(destinationString, "Neighbors");
	if (neighborsStr.length() > 0)
	{
		int currentNeighborIndex = -1;
		while ((currentNeighborIndex = destinationString.find("<Neighbor>", currentNeighborIndex + 1)) != std::string::npos)
		{
			const std::string neighborsStr = FileIO::GetTagContent(destinationString, "Neighbor");

			const int neighborStrComma = neighborsStr.find(',');
			const std::string directionStr = neighborsStr.substr(0, neighborStrComma);
			const int neighborID = stoi(neighborsStr.substr(neighborStrComma + 1));

			if (!directionStr.compare("Left")) destinationNode.m_NeighborIDsArr[LevelDestinationNode::LEFT] = neighborID;
			else if (!directionStr.compare("Right")) destinationNode.m_NeighborIDsArr[LevelDestinationNode::RIGHT] = neighborID;
			else if (!directionStr.compare("Top")) destinationNode.m_NeighborIDsArr[LevelDestinationNode::TOP] = neighborID;
			else if (!directionStr.compare("Bottom")) destinationNode.m_NeighborIDsArr[LevelDestinationNode::BOTTOM] = neighborID;
		}
	}

	return destinationNode;
}
