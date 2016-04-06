#include "stdafx.h"
#include "LevelData.h"

const std::string Pipe::ColourNames[] { "Green", "Yellow", "Blue", "Orange" };

LevelData* LevelData::m_LevelOnePtr = nullptr;

LevelData::LevelData(String platforms, String pipes, String items)
{
	std::string line;

	std::stringstream platformsStream = std::stringstream(platforms.C_str());
	while (std::getline(platformsStream, line))
	{
		int closeBracket = line.find(']');

		DOUBLE2 topLeft = StringToDOUBLE2(line.substr(0, closeBracket + 1));
		DOUBLE2 bottomRight = StringToDOUBLE2(line.substr(closeBracket + 1));

		m_PlatformsPtrArr.push_back(new Platform(topLeft, bottomRight));
	}

	std::stringstream pipesStream = std::stringstream(pipes.C_str());
	while (std::getline(pipesStream, line))
	{
		int closeBracket = line.find(']');

		DOUBLE2 topLeft = StringToDOUBLE2(line.substr(0, closeBracket + 1));
		DOUBLE2 bottomRight = StringToDOUBLE2(line.substr(closeBracket + 1));

		m_PipesPtrArr.push_back(new Pipe(topLeft, bottomRight, false));
	}

	std::stringstream itemsStream = std::stringstream(items.C_str());
	while (std::getline(itemsStream, line))
	{
		int closeBracket = line.find(']');

		DOUBLE2 topLeft = StringToDOUBLE2(line.substr(0, closeBracket + 1));
		int space = line.find(' ', closeBracket);
		int itemType;
		String messageString;
		if (space == -1)
		{
			itemType = std::stoi(line.substr(closeBracket+1).c_str());
		}
		else
		{
			itemType = std::stoi(line.substr(closeBracket + 1, space - closeBracket).c_str());
			messageString = String(line.substr(space + 1).c_str());
		}

		switch (itemType)
		{
		case int(Item::TYPE::PRIZE_BLOCK):
		{
			m_ItemsPtrArr.push_back(new PrizeBlock(topLeft));
		} break;
		case int(Item::TYPE::EXCLAMATION_MARK_BLOCK):
		{
			// LATER: Check if the yellow p-switch has been pressed to determine if ! blocks are solid or not
			m_ItemsPtrArr.push_back(new ExclamationMarkBlock(topLeft, COLOUR::YELLOW, true));
		} break;
		case int(Item::TYPE::COIN):
		{
			m_ItemsPtrArr.push_back(new Coin(topLeft));
		} break;
		case int(Item::TYPE::DRAGON_COIN):
		{
			m_ItemsPtrArr.push_back(new DragonCoin(topLeft));
		} break;
		case int(Item::TYPE::MESSAGE_BLOCK):
		{
			m_ItemsPtrArr.push_back(new MessageBlock(topLeft, messageString));
		} break;
		case int(Item::TYPE::ROTATING_BLOCK):
		{
			m_ItemsPtrArr.push_back(new RotatingBlock(topLeft));
		} break;
		case int(Item::TYPE::P_SWITCH):
		{
			m_ItemsPtrArr.push_back(new PSwitch(topLeft, COLOUR::BLUE));
		} break;
		// NOTE: I don't think 1-UP mushrooms are ever spawned at the start of level..
		//case int(Item::TYPE::ONE_UP_MUSHROOM):
		//{
		//	m_ItemsPtrArr.push_back(new OneUpMushroom(topLeft));
		//} break;
		case int(Item::TYPE::THREE_UP_MOON):
		{
			m_ItemsPtrArr.push_back(new ThreeUpMoon(topLeft));
		} break;
		default:
		{
			OutputDebugString(String("ERROR: Unhandled item in LevelData(): ") + String(int(itemType)) + String("\n"));
		} break;
		}
	}
}

DOUBLE2 LevelData::StringToDOUBLE2(std::string double2String)
{
	DOUBLE2 result;

	int openBracket = double2String.find('[');
	int closeBracket = double2String.find(']', openBracket);
	int comma = double2String.find(',', openBracket);

	if (openBracket != std::string::npos &&
		closeBracket != std::string::npos &&
		comma != std::string::npos)
	{
		result = DOUBLE2(stod(double2String.substr(openBracket + 1, comma - (openBracket + 1))),
			             stod(double2String.substr(comma + 1, closeBracket - (comma + 1))));
	}
	else
	{
		OutputDebugString(String("\nERROR: Malformed platform data string: \n") + String(double2String.c_str()) + String("\n"));
	}

	return result;
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
}

LevelData* LevelData::GetLevel(int levelIndex)
{
	// LATER: Add more levels in here
	// TODO: Make m_LevelOnePtr an array with all the levels?
	if (m_LevelOnePtr == nullptr)
	{
		m_LevelOnePtr = GenerateLevel(levelIndex);
	}

	return m_LevelOnePtr;
}

void LevelData::RegenerateLevel(int levelIndex)
{
	if (levelIndex == 1)
	{
		delete m_LevelOnePtr;
		m_LevelOnePtr = GenerateLevel(levelIndex);
		return;
	}
	else
	{
		OutputDebugString(String("ERROR: Invalid level index passed to LevelData::RegenerateLevel: ") + String(levelIndex) + String("\n"));
		return;
	}
}

LevelData* LevelData::GenerateLevel(int levelIndex)
{
	std::stringstream platformsStream;	// [ DOUBLE2(TOP LEFT, BTM RIGHT) ]
	std::stringstream pipesStream;		// [ DOUBLE2(TOP LEFT, BTM RIGHT), Color?(color) ]
	std::stringstream itemsStream;		// [ DOUBLE2(TOP LEFT, BTM RIGHT), ItemName, ItemProperties... ]

	switch (levelIndex)
	{
	case 1:
	{
		// TODO: Make this cleaner/more efficient, maybe use xml or json? (probably xml cause it's easier to parse manually)
		// Definitely store in an external file
		platformsStream << "[" << 305 << "," << 329 << "]";
		platformsStream << "[" << 559 << "," << 334 << "]";
		platformsStream << "\n";
		platformsStream << "[" << 705 << "," << 313 << "]";
		platformsStream << "[" << 815 << "," << 318 << "]";
		platformsStream << "\n";
		platformsStream << "[" << 2993 << "," << 329 << "]";
		platformsStream << "[" << 3070 << "," << 334 << "]";
		platformsStream << "\n";
		platformsStream << "[" << 3025 << "," << 281 << "]";
		platformsStream << "[" << 3150 << "," << 286 << "]";
		platformsStream << "\n";
		platformsStream << "[" << 3121 << "," << 329 << "]";
		platformsStream << "[" << 3214 << "," << 334 << "]";
		platformsStream << "\n";
		platformsStream << "[" << 3393 << "," << 297 << "]";
		platformsStream << "[" << 3550 << "," << 302 << "]";

		pipesStream << "[" << 2704 << "," << 329 << "]";
		pipesStream << "[" << 2734 << "," << 376 << "]";
		pipesStream << "\n";
		pipesStream << "[" << 2736 << "," << 313 << "]";
		pipesStream << "[" << 2767 << "," << 376 << "]";
		pipesStream << "\n";
		pipesStream << "[" << 3872 << "," << 345 << "]";
		pipesStream << "[" << 3903 << "," << 375 << "]";
		pipesStream << "\n";
		pipesStream << "[" << 4048 << "," << 329 << "]";
		pipesStream << "[" << 4079 << "," << 375 << "]";
		pipesStream << "\n";
		pipesStream << "[" << 4320 << "," << 345 << "]";
		pipesStream << "[" << 4351 << "," << 375 << "]";
		pipesStream << "\n";
		pipesStream << "[" << 4368 << "," << 329 << "]";
		pipesStream << "[" << 4399 << "," << 375 << "]";

		itemsStream << "[" << 609 << "," << 312 << "]";
		itemsStream << int(Item::TYPE::PRIZE_BLOCK) << "\n";
		itemsStream << "[" << 625 << "," << 312 << "]";
		itemsStream << int(Item::TYPE::EXCLAMATION_MARK_BLOCK) << "\n";
		itemsStream << "[" << 641 << "," << 312 << "]";
		itemsStream << int(Item::TYPE::PRIZE_BLOCK) << "\n";

		itemsStream << "[" << 849 << "," << 328 << "]";
		itemsStream << int(Item::TYPE::PRIZE_BLOCK) << "\n";
		itemsStream << "[" << 865 << "," << 328 << "]";
		itemsStream << int(Item::TYPE::PRIZE_BLOCK) << "\n";

		itemsStream << "[" << 1024 << "," << 330 << "]";
		itemsStream << int(Item::TYPE::MESSAGE_BLOCK) << " ";
		itemsStream << "Resources/levels/01/message_box_01.png" << "\n";

		itemsStream << "[" << 1234 << "," << 320 << "]";
		itemsStream << int(Item::TYPE::COIN) << "\n";
		itemsStream << "[" << 1250 << "," << 306 << "]";
		itemsStream << int(Item::TYPE::COIN) << "\n";
		itemsStream << "[" << 1268 << "," << 298 << "]";
		itemsStream << int(Item::TYPE::DRAGON_COIN) << "\n";
		itemsStream << "[" << 1285 << "," << 306 << "]";
		itemsStream << int(Item::TYPE::COIN) << "\n";
		itemsStream << "[" << 1300 << "," << 320 << "]";
		itemsStream << int(Item::TYPE::COIN) << "\n";

		itemsStream << "[" << 1727 << "," << 280 << "]";
		itemsStream << int(Item::TYPE::DRAGON_COIN) << "\n";

		itemsStream << "[" << 2511 << "," << 336 << "]";
		itemsStream << int(Item::TYPE::MESSAGE_BLOCK) << " ";
		itemsStream << "Resources/levels/01/message_box_02.png" << "\n";

		itemsStream << "[" << 2285 << "," << 256 << "]";
		itemsStream << int(Item::TYPE::COIN) << "\n";
		itemsStream << "[" << 2300 << "," << 240 << "]";
		itemsStream << int(Item::TYPE::COIN) << "\n";
		itemsStream << "[" << 2315 << "," << 240 << "]";
		itemsStream << int(Item::TYPE::COIN) << "\n";
		itemsStream << "[" << 2330 << "," << 233 << "]";
		itemsStream << int(Item::TYPE::DRAGON_COIN) << "\n";

		itemsStream << "[" << 2850 << "," << 336 << "]";
		itemsStream << int(Item::TYPE::PRIZE_BLOCK) << "\n";
		itemsStream << "[" << 2866 << "," << 336 << "]";
		itemsStream << int(Item::TYPE::PRIZE_BLOCK) << "\n";
		itemsStream << "[" << 2882 << "," << 336 << "]";
		itemsStream << int(Item::TYPE::PRIZE_BLOCK) << "\n";

		itemsStream << "[" << 3185 << "," << 257 << "]";
		itemsStream << int(Item::TYPE::ROTATING_BLOCK) << "\n";
		itemsStream << "[" << 3201 << "," << 257 << "]";
		itemsStream << int(Item::TYPE::ROTATING_BLOCK) << "\n";

		itemsStream << "[" << 3439 << "," << 240 << "]";
		itemsStream << int(Item::TYPE::COIN) << "\n";
		itemsStream << "[" << 3454 << "," << 240 << "]";
		itemsStream << int(Item::TYPE::COIN) << "\n";
		itemsStream << "[" << 3470 << "," << 240 << "]";
		itemsStream << int(Item::TYPE::COIN) << "\n";
		itemsStream << "[" << 3501 << "," << 227 << "]";
		itemsStream << int(Item::TYPE::DRAGON_COIN) << "\n";

		itemsStream << "[" << 3575 << "," << 288 << "]";
		itemsStream << int(Item::TYPE::EXCLAMATION_MARK_BLOCK) << "\n";
		itemsStream << "[" << 3648 << "," << 385 << "]";
		itemsStream << int(Item::TYPE::EXCLAMATION_MARK_BLOCK) << "\n";
		itemsStream << "[" << 3664 << "," << 385 << "]";
		itemsStream << int(Item::TYPE::EXCLAMATION_MARK_BLOCK) << "\n";

		itemsStream << "[" << 4498 << "," << 257 << "]";
		itemsStream << int(Item::TYPE::ROTATING_BLOCK) << "\n";
		itemsStream << "[" << 4514 << "," << 257 << "]";
		itemsStream << int(Item::TYPE::ROTATING_BLOCK)  << "\n";
		itemsStream << "[" << 4530 << "," << 257 << "]";
		itemsStream << int(Item::TYPE::ROTATING_BLOCK) << "\n";
		itemsStream << "[" << 4498 << "," << 273 << "]";
		itemsStream << int(Item::TYPE::ROTATING_BLOCK) << "\n";
		itemsStream << "[" << 4514 << "," << 273 << "]";
		itemsStream << int(Item::TYPE::P_SWITCH) << "\n";
		itemsStream << "[" << 4530 << "," << 273 << "]";
		itemsStream << int(Item::TYPE::ROTATING_BLOCK) << "\n";
		itemsStream << "[" << 4498 << "," << 289 << "]";
		itemsStream << int(Item::TYPE::ROTATING_BLOCK) << "\n";
		itemsStream << "[" << 4514 << "," << 289 << "]";
		itemsStream << int(Item::TYPE::ROTATING_BLOCK) << "\n";
		itemsStream << "[" << 4530 << "," << 289 << "]";
		itemsStream << int(Item::TYPE::ROTATING_BLOCK) << "\n";

		int coin_xo = 15;
		int coin_yo = -15;
		int x = 4556;
		int y = 330;
		itemsStream << "[" << x << "," << y << "]";
		itemsStream << int(Item::TYPE::COIN) << "\n";
		itemsStream << "[" << x + coin_xo * 1 << "," << y + coin_yo * 1 << "]";
		itemsStream << int(Item::TYPE::COIN) << "\n";
		itemsStream << "[" << x + coin_xo * 2 << "," << y + coin_yo * 2 << "]";
		itemsStream << int(Item::TYPE::COIN) << "\n";
		itemsStream << "[" << x + coin_xo * 3 << "," << y + coin_yo * 3 << "]";
		itemsStream << int(Item::TYPE::COIN) << "\n";
		y += coin_yo * 3;
		itemsStream << "[" << x + coin_xo * 4 << "," << y << "]";
		itemsStream << int(Item::TYPE::COIN) << "\n";
		itemsStream << "[" << x + coin_xo * 5 << "," << y << "]";
		itemsStream << int(Item::TYPE::COIN) << "\n";
		itemsStream << "[" << x + coin_xo * 6 << "," << y << "]";
		itemsStream << int(Item::TYPE::COIN) << "\n";
		itemsStream << "[" << x + coin_xo * 7 << "," << y << "]";
		itemsStream << int(Item::TYPE::COIN) << "\n";
		itemsStream << "[" << x + coin_xo * 8 << "," << y << "]";
		itemsStream << int(Item::TYPE::COIN) << "\n";
		itemsStream << "[" << x + coin_xo * 9 << "," << y << "]";
		itemsStream << int(Item::TYPE::COIN) << "\n";
		itemsStream << "[" << x + coin_xo * 10 << "," << y << "]";
		itemsStream << int(Item::TYPE::COIN) << "\n";
		itemsStream << "[" << x + coin_xo * 11 << "," << y << "]";
		itemsStream << int(Item::TYPE::COIN) << "\n";
		itemsStream << "[" << x + coin_xo * 12 << "," << y << "]";
		itemsStream << int(Item::TYPE::COIN) << "\n";
		itemsStream << "[" << x + coin_xo * 13 << "," << y << "]";
		itemsStream << int(Item::TYPE::COIN) << "\n";


	} break;
	default:
	{
		OutputDebugString(String("ERROR: Unhandled level index in LevelData::GenerateLevel: ") + String(levelIndex) + String("\n"));
		return nullptr;
	} break;
	}

	String platformsString = String(platformsStream.str().c_str());
	String pipesString = String(pipesStream.str().c_str());
	String itemsString = String(itemsStream.str().c_str());

	return new LevelData(platformsString, pipesString, itemsString);
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

void LevelData::TickItems(double deltaTime, Level* levelPtr)
{
	for (size_t i = 0; i < m_ItemsPtrArr.size(); ++i)
	{
		if (m_ItemsPtrArr[i] != nullptr)
		{
			if (m_ItemsPtrArr[i]->Tick(deltaTime, levelPtr))
			{
				RemoveItem(i);
			}
		}
	}
}

void LevelData::PaintItems()
{
	for (size_t i = 0; i < m_ItemsPtrArr.size(); ++i)
	{
		if (m_ItemsPtrArr[i] != nullptr)
		{
			m_ItemsPtrArr[i]->Paint();
		}
	}
}

void LevelData::Unload()
{
	delete m_LevelOnePtr;
}

void LevelData::TogglePaused(bool paused)
{
	for (size_t i = 0; i < m_ItemsPtrArr.size(); ++i)
	{
		if (m_ItemsPtrArr[i] != nullptr)
		{
			m_ItemsPtrArr[i]->TogglePaused(paused);
		}
	}
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
