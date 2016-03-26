//-----------------------------------------------------------------
// Game Engine
// C++ Header - version v2_18a march 2016 
// Copyright Kevin Hoefman, 2006 - 2011 
// Copyright DAE Programming Team, 2012 - 2016 
// http://www.digitalartsandentertainment.be/ 
//-----------------------------------------------------------------

#pragma once
class FmodSystem
{
public:
	FmodSystem();
	virtual ~FmodSystem();

	static bool ErrorCheck(FMOD_RESULT res);
	FMOD::System* GetSystem() const { return m_FmodSystemPtr; }

	// C++11 make the class non-copyable
	FmodSystem(const FmodSystem&) = delete;
	FmodSystem& operator=(const FmodSystem&) = delete;

private:
	void Initialize();
	FMOD::System* m_FmodSystemPtr = nullptr;
};

