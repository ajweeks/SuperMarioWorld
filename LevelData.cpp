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

		m_PipesPtrArr.push_back(new Pipe(topLeft, bottomRight, Pipe::Colour::YELLOW, false));
	}

	std::stringstream itemsStream = std::stringstream(items.C_str());
	while (std::getline(itemsStream, line))
	{
		int closeBracket = line.find(']');

		DOUBLE2 topLeft = StringToDOUBLE2(line.substr(0, closeBracket + 1));
		String itemType = String(line.substr(closeBracket+1).c_str());

		if (itemType.Compare(String("PRIZE")))
		{
			m_ItemsPtrArr.push_back(new PrizeBlock(topLeft));
		}
		else if (itemType.Compare(String("EXCLAMATION_MARK")))
		{
			m_ItemsPtrArr.push_back(new ExclamationMarkBlock(topLeft, ExclamationMarkBlock::COLOUR::YELLOW));
		}
		else
		{
			m_ItemsPtrArr.push_back(new Block(topLeft));
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
		// e.g. Move scale to the constructor
		double scale = 2.0;
		platformsStream << "[" << 305 * scale << "," << 337 * scale << "]";
		platformsStream << "[" << 559 * scale << "," << 342 * scale << "]";
		platformsStream << "\n";
		platformsStream << "[" << 705 * scale << "," << 321 * scale << "]";
		platformsStream << "[" << 815 * scale << "," << 326 * scale << "]";
		platformsStream << "\n";
		platformsStream << "[" << 2993 * scale << "," << 337 * scale << "]";
		platformsStream << "[" << 3070 * scale << "," << 342 * scale << "]";
		platformsStream << "\n";
		platformsStream << "[" << 3025 * scale << "," << 289 * scale << "]";
		platformsStream << "[" << 3150 * scale << "," << 294 * scale << "]";
		platformsStream << "\n";
		platformsStream << "[" << 3121 * scale << "," << 337 * scale << "]";
		platformsStream << "[" << 3214 * scale << "," << 342 * scale << "]";
		platformsStream << "\n";
		platformsStream << "[" << 3393 * scale << "," << 305 * scale << "]";
		platformsStream << "[" << 3550 * scale << "," << 310 * scale << "]";

		pipesStream << "[" << 2705 * scale << "," << 337 * scale << "]";
		pipesStream << "[" << 2733 * scale << "," << 384 * scale << "]";
		pipesStream << std::string(Pipe::ColourNames[int(Pipe::Colour::YELLOW)]);
		pipesStream << "\n";
		pipesStream << "[" << 2737 * scale << "," << 321 * scale << "]";
		pipesStream << "[" << 2767 * scale << "," << 384 * scale << "]";
		pipesStream << std::string(Pipe::ColourNames[int(Pipe::Colour::YELLOW)]);
		pipesStream << "\n";
		pipesStream << "[" << 3873 * scale << "," << 353 * scale << "]";
		pipesStream << "[" << 3902 * scale << "," << 383 * scale << "]";
		pipesStream << std::string(Pipe::ColourNames[int(Pipe::Colour::BLUE)]);
		pipesStream << "\n";
		pipesStream << "[" << 4049 * scale << "," << 337 * scale << "]";
		pipesStream << "[" << 4078 * scale << "," << 383 * scale << "]";
		pipesStream << std::string(Pipe::ColourNames[int(Pipe::Colour::BLUE)]);
		pipesStream << "\n";
		pipesStream << "[" << 4321 * scale << "," << 353 * scale << "]";
		pipesStream << "[" << 4350 * scale << "," << 383 * scale << "]";
		pipesStream << std::string(Pipe::ColourNames[int(Pipe::Colour::GREY)]);
		pipesStream << "\n";
		pipesStream << "[" << 4369 * scale << "," << 337 * scale << "]";
		pipesStream << "[" << 4398 * scale << "," << 383 * scale << "]";
		pipesStream << std::string(Pipe::ColourNames[int(Pipe::Colour::GREEN)]);

		itemsStream << "[" << 609 * scale << "," << 320 * scale << "]";
		itemsStream << "PRIZE";
		itemsStream << "\n";
		itemsStream << "[" << 625 * scale << "," << 320 * scale << "]";
		itemsStream << "EXCLAMATION_MARK";
		itemsStream << "\n";
		itemsStream << "[" << 641 * scale << "," << 320 * scale << "]";
		itemsStream << "PRIZE";
		itemsStream << "\n";
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

void LevelData::TickItems(double deltaTime, Level* levelPtr)
{
	for (size_t i = 0; i < m_ItemsPtrArr.size(); ++i)
	{
		m_ItemsPtrArr[i]->Tick(deltaTime, levelPtr);
	}
}

void LevelData::PaintItems()
{
	for (size_t i = 0; i < m_ItemsPtrArr.size(); ++i)
	{
		m_ItemsPtrArr[i]->Paint();
	}
}