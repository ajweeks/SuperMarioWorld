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

		m_PipesPtrArr.push_back(new Pipe(topLeft, bottomRight, COLOUR::YELLOW, false));
	}

	std::stringstream itemsStream = std::stringstream(items.C_str());
	while (std::getline(itemsStream, line))
	{
		int closeBracket = line.find(']');

		DOUBLE2 topLeft = StringToDOUBLE2(line.substr(0, closeBracket + 1));
		int itemType = std::stoi(line.substr(closeBracket+1).c_str());

		switch (itemType)
		{
		case int(Item::TYPE::PRIZE_BLOCK):
		{
			m_ItemsPtrArr.push_back(new PrizeBlock(topLeft));
		} break;
		case int(Item::TYPE::EXCLAMATION_MARK_BLOCK):
		{
			m_ItemsPtrArr.push_back(new ExclamationMarkBlock(topLeft, COLOUR::YELLOW, false));
		} break;
		case int(Item::TYPE::COIN):
		{
			m_ItemsPtrArr.push_back(new Coin(topLeft, -1));
		} break;
		case int(Item::TYPE::DRAGON_COIN):
		{
			m_ItemsPtrArr.push_back(new DragonCoin(topLeft));
		} break;
		case int(Item::TYPE::MESSAGE_BLOCK):
		{
			m_ItemsPtrArr.push_back(new MessageBlock(topLeft, String("Messagge!!!! Woo")));
		} break;
		case int(Item::TYPE::ROTATING_BLOCK):
		{
			m_ItemsPtrArr.push_back(new RotatingBlock(topLeft));
		} break;
		case int(Item::TYPE::P_SWITCH):
		{
			m_ItemsPtrArr.push_back(new PSwitch(topLeft, COLOUR::BLUE));
		} break;
		case int(Item::TYPE::ONE_UP_MUSHROOM):
		{
			m_ItemsPtrArr.push_back(new OneUpMushroom(topLeft));
		} break;
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
		// TODO: Make this cleaner/more efficient
		platformsStream << "[" << 305 << "," << 337 << "]";
		platformsStream << "[" << 559 << "," << 342 << "]";
		platformsStream << "\n";
		platformsStream << "[" << 705 << "," << 321 << "]";
		platformsStream << "[" << 815 << "," << 326 << "]";
		platformsStream << "\n";
		platformsStream << "[" << 2993 << "," << 337 << "]";
		platformsStream << "[" << 3070 << "," << 342 << "]";
		platformsStream << "\n";
		platformsStream << "[" << 3025 << "," << 289 << "]";
		platformsStream << "[" << 3150 << "," << 294 << "]";
		platformsStream << "\n";
		platformsStream << "[" << 3121 << "," << 337 << "]";
		platformsStream << "[" << 3214 << "," << 342 << "]";
		platformsStream << "\n";
		platformsStream << "[" << 3393 << "," << 305 << "]";
		platformsStream << "[" << 3550 << "," << 310 << "]";

		pipesStream << "[" << 2705 << "," << 337 << "]";
		pipesStream << "[" << 2733 << "," << 384 << "]";
		pipesStream << std::string(Pipe::ColourNames[int(COLOUR::YELLOW)]);
		pipesStream << "\n";
		pipesStream << "[" << 2737 << "," << 321 << "]";
		pipesStream << "[" << 2767 << "," << 384 << "]";
		pipesStream << std::string(Pipe::ColourNames[int(COLOUR::YELLOW)]);
		pipesStream << "\n";
		pipesStream << "[" << 3873 << "," << 353 << "]";
		pipesStream << "[" << 3902 << "," << 383 << "]";
		pipesStream << std::string(Pipe::ColourNames[int(COLOUR::BLUE)]);
		pipesStream << "\n";
		pipesStream << "[" << 4049 << "," << 337 << "]";
		pipesStream << "[" << 4078 << "," << 383 << "]";
		pipesStream << std::string(Pipe::ColourNames[int(COLOUR::BLUE)]);
		pipesStream << "\n";
		pipesStream << "[" << 4321 << "," << 353 << "]";
		pipesStream << "[" << 4350 << "," << 383 << "]";
		pipesStream << std::string(Pipe::ColourNames[int(COLOUR::GREY)]);
		pipesStream << "\n";
		pipesStream << "[" << 4369 << "," << 337 << "]";
		pipesStream << "[" << 4398 << "," << 383 << "]";
		pipesStream << std::string(Pipe::ColourNames[int(COLOUR::GREEN)]);

		itemsStream << "[" << 609 << "," << 320 << "]";
		itemsStream << int(Item::TYPE::PRIZE_BLOCK) << "\n";
		itemsStream << "[" << 625 << "," << 320 << "]";
		itemsStream << int(Item::TYPE::EXCLAMATION_MARK_BLOCK) << "\n";
		itemsStream << "[" << 641 << "," << 320 << "]";
		itemsStream << int(Item::TYPE::PRIZE_BLOCK) << "\n";

		itemsStream << "[" << 849 << "," << 335 << "]";
		itemsStream << int(Item::TYPE::PRIZE_BLOCK) << "\n";
		itemsStream << "[" << 865 << "," << 335 << "]";
		itemsStream << int(Item::TYPE::PRIZE_BLOCK) << "\n";

		itemsStream << "[" << 1240 << "," << 320 << "]";
		itemsStream << int(Item::TYPE::COIN) << "\n";
		itemsStream << "[" << 1250 << "," << 300 << "]";
		itemsStream << int(Item::TYPE::COIN) << "\n";
		itemsStream << "[" << 1266 << "," << 280 << "]";
		itemsStream << int(Item::TYPE::DRAGON_COIN) << "\n";
		itemsStream << "[" << 1288 << "," << 300 << "]";
		itemsStream << int(Item::TYPE::COIN) << "\n";
		itemsStream << "[" << 1302 << "," << 320 << "]";
		itemsStream << int(Item::TYPE::COIN) << "\n";

		itemsStream << "[" << 2511 << "," << 336 << "]";
		itemsStream << int(Item::TYPE::MESSAGE_BLOCK) << "\n";

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

		itemsStream << "[" << 3575 << "," << 288 << "]";
		itemsStream << int(Item::TYPE::EXCLAMATION_MARK_BLOCK) << "\n";
		itemsStream << "[" << 3648 << "," << 385 << "]";
		itemsStream << int(Item::TYPE::EXCLAMATION_MARK_BLOCK) << "\n";
		itemsStream << "[" << 3664 << "," << 385 << "]";
		itemsStream << int(Item::TYPE::EXCLAMATION_MARK_BLOCK) << "\n";

		itemsStream << "[" << 4498 << "," << 257 << "]";
		itemsStream << int(Item::TYPE::EXCLAMATION_MARK_BLOCK) << "\n";
		itemsStream << "[" << 4514 << "," << 257 << "]";
		itemsStream << int(Item::TYPE::EXCLAMATION_MARK_BLOCK)  << "\n";
		itemsStream << "[" << 4530 << "," << 257 << "]";
		itemsStream << int(Item::TYPE::EXCLAMATION_MARK_BLOCK) << "\n";
		itemsStream << "[" << 4498 << "," << 273 << "]";
		itemsStream << int(Item::TYPE::EXCLAMATION_MARK_BLOCK) << "\n";
		itemsStream << "[" << 4514 << "," << 273 << "]";
		itemsStream << int(Item::TYPE::P_SWITCH) << "\n";
		itemsStream << "[" << 4530 << "," << 273 << "]";
		itemsStream << int(Item::TYPE::EXCLAMATION_MARK_BLOCK) << "\n";
		itemsStream << "[" << 4498 << "," << 289 << "]";
		itemsStream << int(Item::TYPE::EXCLAMATION_MARK_BLOCK) << "\n";
		itemsStream << "[" << 4514 << "," << 289 << "]";
		itemsStream << int(Item::TYPE::EXCLAMATION_MARK_BLOCK) << "\n";
		itemsStream << "[" << 4530 << "," << 289 << "]";
		itemsStream << int(Item::TYPE::EXCLAMATION_MARK_BLOCK) << "\n";

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