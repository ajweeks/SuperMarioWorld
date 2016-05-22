#include "stdafx.h"

#include "LevelData.h"
#include "Enumerations.h"
#include "Level.h"
#include "Player.h"
#include "FileIO.h"
#include "Colour.h"

#include "Entity.h"
#include "Enemy.h"
#include "MontyMole.h"
#include "KoopaTroopa.h"
#include "PiranhaPlant.h"
#include "CharginChuck.h"

#include "Platform.h"
#include "Pipe.h"
#include "Item.h"
#include "PrizeBlock.h"
#include "RotatingBlock.h"
#include "ExclamationMarkBlock.h"
#include "CloudBlock.h"
#include "GrabBlock.h"
#include "SuperMushroom.h"
#include "OneUpMushroom.h"
#include "MessageBlock.h"
#include "ThreeUpMoon.h"
#include "Coin.h"
#include "DragonCoin.h"
#include "PSwitch.h"
#include "Berry.h"
#include "KoopaShell.h"
#include "Beanstalk.h"
#include "MidwayGate.h"
#include "GoalGate.h"

std::vector<LevelData*> LevelData::m_LevelDataPtrArr = std::vector<LevelData*>(Constants::NUM_LEVELS);

LevelData::LevelData(std::string platforms, std::string pipes, std::string items, std::string enemies, Level* levelPtr) : 
	m_LevelPtr(levelPtr)
{
	std::string line;

	// PLATFORMS
	platforms.erase(std::remove_if(platforms.begin(), platforms.end(), isspace), platforms.end());
	int platformTagBeginIndex = -1;
	while ((platformTagBeginIndex = platforms.find("<Platform>", platformTagBeginIndex + 1)) != std::string::npos)
	{
		std::string platformContent = FileIO::GetTagContent(platforms, "Platform", platformTagBeginIndex);

		int comma1 = platformContent.find(",");
		int comma2 = platformContent.find(",", comma1 + 1);
		int left = stoi(platformContent.substr(0, comma1)) * TILE_SIZE;
		int top = stoi(platformContent.substr(comma1 + 1, comma2 - comma1 - 1)) * TILE_SIZE;
		int right = stoi(platformContent.substr(comma2 + 1)) * TILE_SIZE;

		m_PlatformsPtrArr.push_back(new Platform(left, top, right));
	}


	// PIPES
	pipes.erase(std::remove_if(pipes.begin(), pipes.end(), isspace), pipes.end());
	int pipeTagBeginIndex = -1;
	int pipeIndex = 0; // Keep track of how many pipes we've added so far
	while ((pipeTagBeginIndex = pipes.find("<Pipe>", pipeTagBeginIndex + 1)) != std::string::npos)
	{
		std::string pipeContent = FileIO::GetTagContent(pipes, "Pipe", pipeTagBeginIndex);

		std::string boundingBoxString = FileIO::GetTagContent(pipeContent, "BoundingBox");
		RECT2 boundingBox = FileIO::StringToRECT2(boundingBoxString);
		DOUBLE2 topLeft = DOUBLE2(boundingBox.left, boundingBox.top) * TILE_SIZE;
		DOUBLE2 bottomRight = DOUBLE2(boundingBox.right, boundingBox.bottom) * TILE_SIZE;

		Pipe::Orientation pipeOrientation = Pipe::StringToOrientation(FileIO::GetTagContent(pipeContent, "Orientation"));
		bool pipeIsEnterable = FileIO::GetTagContent(pipeContent, "Enterable") == "1";

		std::string spawnsString = FileIO::GetTagContent(pipeContent, "Spawns");
		bool spawnsPiranhaPlant = false;
		if (spawnsString.length() > 0 && !spawnsString.compare("PiranhaPlant"))
		{
			int pipeWidth = int(bottomRight.x - topLeft.x);
			m_EnemiesPtrArr.push_back(new PiranhaPlant(topLeft + DOUBLE2(pipeWidth / 2, Item::TILE_SIZE * 2), levelPtr));
		}

		int warpLevelIndex = -1;
		std::string warpLevelIndexStr = FileIO::GetTagContent(pipeContent, "WarpLevelIndex");
		if (warpLevelIndexStr.length() > 0) warpLevelIndex = stoi(warpLevelIndexStr);

		int warpPipeIndex = -1;
		std::string warpPipeIndexStr = FileIO::GetTagContent(pipeContent, "WarpPipeIndex");
		if (warpPipeIndexStr.length() > 0) warpPipeIndex = stoi(warpPipeIndexStr);

		m_PipesPtrArr.push_back(new Pipe(topLeft, bottomRight, levelPtr, pipeOrientation, pipeIndex, warpLevelIndex, warpPipeIndex));
		++pipeIndex;
	}


	// ITEMS
	items.erase(std::remove_if(items.begin(), items.end(), isspace), items.end());
	int itemTagBeginIndex = -1;
	while ((itemTagBeginIndex = items.find("<Item>", itemTagBeginIndex + 1)) != std::string::npos)
	{
		std::string itemContent = FileIO::GetTagContent(items, "Item", itemTagBeginIndex);

		// GENERAL PROPERTIES
		DOUBLE2 topLeft = StringToDOUBLE2(FileIO::GetTagContent(itemContent, "TopLeft")) * TILE_SIZE;
		std::string itemTypeString = FileIO::GetTagContent(itemContent, "Type");
		int itemType = int(Item::StringToTYPE(itemTypeString));

		// UNIQUE PROPERTIES
		// NOTE: Not all items have the following properties, but there is no harm is seeing if a tag is there
		COLOUR itemColour = Colour::StringToCOLOUR(FileIO::GetTagContent(itemContent, "Colour"));

		switch (itemType)
		{
		case int(Item::TYPE::PRIZE_BLOCK):
		{
			std::string spawnsString = FileIO::GetTagContent(itemContent, "Spawns");
			m_ItemsPtrArr.push_back(new PrizeBlock(topLeft, levelPtr, spawnsString));
		} break;
		case int(Item::TYPE::EXCLAMATION_MARK_BLOCK):
		{
			// LATER: Check if the yellow p-switch has been pressed to determine if ! blocks are solid or not
			m_ItemsPtrArr.push_back(new ExclamationMarkBlock(topLeft, itemColour, true, levelPtr));
		} break;
		case int(Item::TYPE::COIN):
		{
			m_ItemsPtrArr.push_back(new Coin(topLeft, levelPtr));
		} break;
		case int(Item::TYPE::DRAGON_COIN):
		{
			m_ItemsPtrArr.push_back(new DragonCoin(topLeft, levelPtr));
		} break;
		case int(Item::TYPE::MESSAGE_BLOCK):
		{
			String messageString = String(FileIO::GetTagContent(itemContent, "BitmapFilePath").c_str());
			m_ItemsPtrArr.push_back(new MessageBlock(topLeft, messageString, levelPtr));
		} break;
		case int(Item::TYPE::ROTATING_BLOCK):
		{
			std::string spawnItem = FileIO::GetTagContent(itemContent, "Spawns");
			bool spawnsBeanstalk = !spawnItem.compare("Beanstalk");
			m_ItemsPtrArr.push_back(new RotatingBlock(topLeft, levelPtr, spawnsBeanstalk));
		} break;
		case int(Item::TYPE::CLOUD_BLOCK):
		{
			m_ItemsPtrArr.push_back(new CloudBlock(topLeft, levelPtr));
		} break;
		case int(Item::TYPE::GRAB_BLOCK):
		{
			m_ItemsPtrArr.push_back(new GrabBlock(topLeft, levelPtr));
		} break;
		case int(Item::TYPE::P_SWITCH):
		{
			m_ItemsPtrArr.push_back(new PSwitch(topLeft, itemColour, levelPtr));
		} break;
		case int(Item::TYPE::BERRY):
		{
			m_ItemsPtrArr.push_back(new Berry(topLeft, levelPtr, itemColour));
		} break;
		case int(Item::TYPE::KOOPA_SHELL):
		{
			m_ItemsPtrArr.push_back(new KoopaShell(topLeft, levelPtr, itemColour));
		} break;
		case int(Item::TYPE::THREE_UP_MOON):
		{
			m_ItemsPtrArr.push_back(new ThreeUpMoon(topLeft, levelPtr));
		} break;
		case int(Item::TYPE::MIDWAY_GATE):
		{
			int barHeight = stoi(FileIO::GetTagContent(itemContent, "BarHeight"));
			m_ItemsPtrArr.push_back(new MidwayGate(topLeft, levelPtr, barHeight));
		} break;
		case int(Item::TYPE::GOAL_GATE):
		{
			m_ItemsPtrArr.push_back(new GoalGate(topLeft, levelPtr));
		} break;
		default:
		{
			OutputDebugString(String("ERROR: Unhandled item passed LevelData(): ") + String(itemTypeString.c_str()) + String("\n"));
		} break;
		}
	}

	// ENEMIES
	enemies.erase(std::remove_if(enemies.begin(), enemies.end(), isspace), enemies.end());
	int enemyTagBeginIndex = -1;
	while ((enemyTagBeginIndex = enemies.find("<Enemy>", enemyTagBeginIndex + 1)) != std::string::npos)
	{
		std::string enemyContent = FileIO::GetTagContent(enemies, "Enemy", enemyTagBeginIndex);

		DOUBLE2 topLeft = (StringToDOUBLE2(FileIO::GetTagContent(enemyContent, "TopLeft")) * TILE_SIZE) + DOUBLE2(0, -2);
		std::string enemyTypeString = FileIO::GetTagContent(enemyContent, "Type");
		int enemyType = int(Enemy::StringToTYPE(enemyTypeString));

		COLOUR enemyColour = Colour::StringToCOLOUR(FileIO::GetTagContent(enemyContent, "Colour"));

		switch (enemyType)
		{
		case int(Enemy::TYPE::KOOPA_TROOPA):
		{
			m_EnemiesPtrArr.push_back(new KoopaTroopa(topLeft, levelPtr, enemyColour));
		} break;
		case int(Enemy::TYPE::MONTY_MOLE):
		{
			MontyMole::AI_TYPE aiType = MontyMole::StringToAIType(FileIO::GetTagContent(enemyContent, "AIType"));
			MontyMole::SPAWN_LOCATION_TYPE spawnLocationType = MontyMole::StringToSpawnLocationType(FileIO::GetTagContent(enemyContent, "SpawnLocationType"));
			m_EnemiesPtrArr.push_back(new MontyMole(topLeft, levelPtr, spawnLocationType, aiType));
		} break;
		case int(Enemy::TYPE::CHARGIN_CHUCK):
		{
			m_EnemiesPtrArr.push_back(new CharginChuck(topLeft, levelPtr));
		} break;
		default:
		{
			OutputDebugString(String("ERROR: Unhandled enemy passed to LevelData(): ") + String(enemyTypeString.c_str()) + String("\n"));
		} break;
		}
	}
}

LevelData::~LevelData()
{
	for (size_t i = 0; i < m_PlatformsPtrArr.size(); ++i)
	{
		delete m_PlatformsPtrArr[i];
	}
	for (size_t i = 0; i < m_PipesPtrArr.size(); ++i)
	{
		delete m_PipesPtrArr[i];
	}
	for (size_t i = 0; i < m_ItemsPtrArr.size(); ++i)
	{
		delete m_ItemsPtrArr[i];
	}
	for (size_t i = 0; i < m_EnemiesPtrArr.size(); ++i)
	{
		delete m_EnemiesPtrArr[i];
	}
}

LevelData* LevelData::GetLevelData(int levelIndex, Level* levelPtr)
{
	assert(levelIndex >= 0 && levelIndex < int(m_LevelDataPtrArr.size()));

	if (m_LevelDataPtrArr[levelIndex] == nullptr)
	{
		delete m_LevelDataPtrArr[levelIndex];
		m_LevelDataPtrArr[levelIndex] = CreateLevelData(levelIndex, levelPtr);
	}

	return m_LevelDataPtrArr[levelIndex];
}

LevelData* LevelData::CreateLevelData(int levelIndex, Level* levelPtr)
{
	std::ifstream fileInStream;
	std::stringstream stringStream;

	std::stringstream levelIndexStr;
	levelIndexStr << std::setw(2) << std::setfill('0') << levelIndex;
	fileInStream.open("Resources/levels/" + levelIndexStr.str() + "/level-data.txt");
	
	if (fileInStream.fail())
	{
		GAME_ENGINE->MessageBox(String(("Invalid level index: " + levelIndexStr.str() + "\n").c_str()));
		return nullptr;
	}

	std::string line;
	while (fileInStream.eof() == false)
	{
		std::getline(fileInStream, line);
		stringStream << line << "\n";
	}
	fileInStream.close();

	std::string entireFileContents = stringStream.str();
	
	std::string platformsString = FileIO::GetTagContent(entireFileContents, "Platforms");
	std::string pipesString = FileIO::GetTagContent(entireFileContents, "Pipes");
	std::string itemsString = FileIO::GetTagContent(entireFileContents, "Items");
	std::string enemiesString = FileIO::GetTagContent(entireFileContents, "Enemies");

	return new LevelData(platformsString, pipesString, itemsString, enemiesString, levelPtr);
}

void LevelData::AddItem(Item* newItemPtr)
{
	for (size_t i = 0; i < m_ItemsPtrArr.size(); ++i)
	{
		if (m_ItemsPtrArr[i] == nullptr)
		{
			m_ItemsPtrArr[i] = newItemPtr;
			return;
		}
	}

	m_ItemsPtrArr.push_back(newItemPtr);
}

void LevelData::RemoveItem(Item* itemPtr)
{
	for (size_t i = 0; i < m_ItemsPtrArr.size(); ++i)
	{
		if (m_ItemsPtrArr[i] == itemPtr)
		{
			delete m_ItemsPtrArr[i];
			m_ItemsPtrArr[i] = nullptr;
			return;
		}
	}

	OutputDebugString(String("ERROR: Could not delete item in LevelData::RemoveItem\n"));
	assert(false);
}

void LevelData::RemoveItem(int itemIndex)
{
	assert(itemIndex >= 0 && itemIndex < int(m_ItemsPtrArr.size()));

	delete m_ItemsPtrArr[itemIndex];
	m_ItemsPtrArr[itemIndex] = nullptr;
}

void LevelData::AddEnemy(Enemy* newEnemyPtr)
{
	for (size_t i = 0; i < m_EnemiesPtrArr.size(); ++i)
	{
		if (m_EnemiesPtrArr[i] == nullptr)
		{
			m_EnemiesPtrArr[i] = newEnemyPtr;
			return;
		}
	}

	m_EnemiesPtrArr.push_back(newEnemyPtr);
}

void LevelData::RemoveEnemy(Enemy* enemyPtr)
{
	for (size_t i = 0; i < m_EnemiesPtrArr.size(); ++i)
	{
		if (m_EnemiesPtrArr[i] == enemyPtr)
		{
			delete m_EnemiesPtrArr[i];
			m_EnemiesPtrArr[i] = nullptr;
			return;
		}
	}

	OutputDebugString(String("ERROR: Could not delete item in LevelData::RemoveItem\n"));
	assert(false);
}

void LevelData::RemoveEnemy(int enemyIndex)
{
	assert(enemyIndex >= 0 && enemyIndex < int(m_EnemiesPtrArr.size()));

	delete m_EnemiesPtrArr[enemyIndex];
	m_EnemiesPtrArr[enemyIndex] = nullptr;
}

void LevelData::TickItemsAndEnemies(double deltaTime, Level* levelPtr)
{
	for (size_t i = 0; i < m_ItemsPtrArr.size(); ++i)
	{
		if (m_ItemsPtrArr[i] != nullptr)
		{
			m_ItemsPtrArr[i]->Tick(deltaTime);
		}
	}

	for (size_t i = 0; i < m_EnemiesPtrArr.size(); ++i)
	{
		if (m_EnemiesPtrArr[i] != nullptr)
		{
			m_EnemiesPtrArr[i]->Tick(deltaTime);
		}
	}
}

void LevelData::PaintItemsAndEnemies()
{
	for (size_t i = 0; i < m_ItemsPtrArr.size(); ++i)
	{
		if (m_ItemsPtrArr[i] != nullptr)
		{
			m_ItemsPtrArr[i]->Paint();
		}
	}

	for (size_t i = 0; i < m_EnemiesPtrArr.size(); ++i)
	{
		// NOTE: Piranha plants are drawn behind everything else, they are not drawn here!
		if (m_EnemiesPtrArr[i] != nullptr && m_EnemiesPtrArr[i]->GetType() != Enemy::TYPE::PIRHANA_PLANT)
		{
			m_EnemiesPtrArr[i]->Paint();
		}
	}
}

void LevelData::PaintItemsInForeground()
{
	for (size_t i = 0; i < m_ItemsPtrArr.size(); ++i)
	{
		if (m_ItemsPtrArr[i] != nullptr)
		{
			if (m_ItemsPtrArr[i]->GetType() == Item::TYPE::GOAL_GATE ||
				m_ItemsPtrArr[i]->GetType() == Item::TYPE::MIDWAY_GATE)
			{
				((Gate*)m_ItemsPtrArr[i])->PaintFrontPole();
			}
		}
	}
}

void LevelData::PaintEnemiesInBackground()
{
	for (size_t i = 0; i < m_EnemiesPtrArr.size(); ++i)
	{
		if (m_EnemiesPtrArr[i] != nullptr)
		{
			if (m_EnemiesPtrArr[i]->GetType() == Enemy::TYPE::PIRHANA_PLANT)
			{
				m_EnemiesPtrArr[i]->Paint();
			}
		}
	}
}

DOUBLE2 LevelData::StringToDOUBLE2(std::string double2String)
{
	DOUBLE2 result;

	int comma = double2String.find(',');

	if (comma != std::string::npos)
	{
		result = DOUBLE2(stod(double2String.substr(0, comma)), stod(double2String.substr(comma + 1)));
	}
	else
	{
		OutputDebugString(String("\nERROR: Malformed platform data string: \n") + String(double2String.c_str()) + String("\n"));
	}

	return result;
}

void LevelData::UnloadAllLevels()
{
	for (size_t i = 0; i < m_LevelDataPtrArr.size(); ++i)
	{
		UnloadLevel(i);
	}
}

void LevelData::UnloadLevel(int index)
{
	if (m_LevelDataPtrArr[index] != nullptr)
	{
		delete m_LevelDataPtrArr[index];
		m_LevelDataPtrArr[index] = nullptr;
	}
}

void LevelData::SetItemsAndEnemiesPaused(bool paused)
{
	for (size_t i = 0; i < m_ItemsPtrArr.size(); ++i)
	{
		if (m_ItemsPtrArr[i] != nullptr)
		{
			m_ItemsPtrArr[i]->SetPaused(paused);
		}
	}

	for (size_t i = 0; i < m_EnemiesPtrArr.size(); ++i)
	{
		if (m_EnemiesPtrArr[i] != nullptr)
		{
			m_EnemiesPtrArr[i]->SetPaused(paused);
		}
	}
}

Pipe* LevelData::GetPipeWithIndex(int index)
{
	for (size_t i = 0; i < m_PipesPtrArr.size(); ++i)
	{
		if (m_PipesPtrArr[i] != nullptr && 
			m_PipesPtrArr[i]->GetIndex() == index)
		{
			return m_PipesPtrArr[i];
		}
	}

	return nullptr;
}

std::vector<Platform*> LevelData::GetPlatforms()
{
	return m_PlatformsPtrArr;
}

std::vector<Pipe*> LevelData::GetPipes()
{
	return m_PipesPtrArr;
}

std::vector<Item*> LevelData::GetItems()
{
	return m_ItemsPtrArr;
}

std::vector<Enemy*> LevelData::GetEnemies()
{
	return m_EnemiesPtrArr;
}
