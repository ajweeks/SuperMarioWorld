#pragma once
#include "Enumerations.h"
#include "SpriteSheetManager.h"
#include "SpriteSheet.h"
#include "Entity.h"
#include "GameItem.h"

#define GAME_ENGINE (GameEngine::GetSingleton())

/*

This class represents an entire level
It holds the following information:

- Platforms
- Pipes
- Enemies' spawns
- Items (Coins, Blocks, etc.)
- Midway gates & Goal gates

*/
class LevelData
{
public:
	virtual ~LevelData();

	LevelData& operator=(const LevelData&) = delete;
	LevelData(const LevelData&) = delete;

	static LevelData* GetLevel(int level);
	static void Unload();

	void AddItem(Item* newItemPtr);
	void RemoveItem(Item* itemPtr);
	// Platforms and pipes don't need to be individually painted or ticked
	// since they are part of the forground bitmap and static,
	// therefore only tick and paint the items (TODO: Rename item to entity?)
	void PaintItems();
	void TickItems(double deltaTime, Level* levelPtr);

	std::vector<Platform*> GetPlatforms();
	std::vector<Pipe*> GetPipes();
	std::vector<Item*> GetItems();

private:
	LevelData(String platforms, String pipes, String items);

	static LevelData* GenerateLevel(int levelIndex);
	static LevelData* m_LevelOnePtr;

	DOUBLE2 StringToDOUBLE2(std::string double2String);

	std::vector<Platform*> m_PlatformsPtrArr;
	std::vector<Pipe*> m_PipesPtrArr;
	std::vector<Item*> m_ItemsPtrArr;
};
