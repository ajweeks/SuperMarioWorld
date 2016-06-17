#pragma once

#include "Enemy.h"

struct INT2;

/*
	Monty Moles spawn from dirt patches in walls and in the ground when the player comes nearby.
	If the player doesn't kill them, they will respawn when the player goes away and comes back.
	Once a mole has been spawned and has jumped out, whether or not they were killed, there will
	be a hole sprite rendered there for the rest of the game.

	All moles are spawned at the start of the game and are invisible until
	the player comes within a certain distance of them. When a mole is killed by 
	the player, this instance is told to not spawn a mole again. 
*/

class MontyMole : public Enemy
{
public:
	enum class AnimationState
	{
		INVISIBLE, IN_GROUND, JUMPING_OUT_OF_GROUND, WALKING, DEAD
	};
	enum class SpawnLocationType
	{
		GROUND, WALL, NONE
	};

	/*
		There are two types of AI:
	  1) Stupid roomba
	    - When these moles spawn, they will see which direction the player is and start walking
		  towards them. They jump every half second or so, and walk until they hit an obstacle,
		  and then turn around.
	  2) Less-stupid rooma
	    - When these moles spawn they also first go in the direction that the player is, but once 
		  they walk past the player, they turn around and walk back, just to turn around soon after.
	*/
	enum class AIType
	{
		DUMB, SMART, NONE
	};

	MontyMole(DOUBLE2& startingPos, Level* levelPtr, SpawnLocationType spawnLocationType, AIType aiType);
	virtual ~MontyMole();

	MontyMole(const MontyMole&) = delete;
	MontyMole&operator=(const MontyMole&) = delete;

	void Tick(double deltaTime);
	void Paint();
	INT2 GetAnimationFrame();

	int GetWidth() const;
	int GetHeight() const;

	void HeadBonk();
	void StompKill();

	bool IsAlive() const;
	void SetDead();

	void SetPaused(bool paused);
	bool Raycast(DOUBLE2 point1, DOUBLE2 point2, DOUBLE2 &intersectionRef, DOUBLE2 &normalRef, double &fractionRef);

	static AIType StringToAIType(std::string aiTypeString);
	static SpawnLocationType StringToSpawnLocationType(std::string spawnLocationTypeString);

private:
	void UpdatePosition(double deltaTime);
	void CalculateNewTarget();

	static const int WIDTH = 12;
	static const int HEIGHT = 16;

	static const double HORIZONTAL_ACCELERATION;
	static const double MAX_HORIZONTAL_VEL;
	static const double TARGET_OVERSHOOT_DISTANCE;

	static const double JUMP_VEL; // How high the "dumb" moles jump while walking around
	static const double LAUNCH_OUT_OF_GROUND_VEL;

	static const int PLAYER_PROXIMITY = 100; // How close the player has to get before we spawn

	AnimationState m_AnimationState;

	SMWTimer m_SpawnDustCloudTimer;
	SMWTimer m_FramesSpentWrigglingInDirtTimer;
	SMWTimer m_FramesSinceLastHop; // Only used by "dumb" moles

	double m_TargetX;

	bool m_HasBeenKilledByPlayer = false;
	bool m_HaveSpawnedMole = false;
	bool m_ShouldRemoveActor = false; // Only ever set to true when the player kills us (or we fall off the level)

	SpawnLocationType m_SpawnLocationType;
	AIType m_AiType;
};
