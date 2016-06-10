#pragma once

class Particle;

class ParticleManager
{
public:
	ParticleManager();
	virtual ~ParticleManager();

	ParticleManager(const ParticleManager&) = delete;
	ParticleManager& operator=(const ParticleManager&) = delete;

	void Tick(double deltaTime);
	void Paint();

	void AddParticle(Particle* newParticlePtr);
	void RemoveParticle(Particle* particlePtr);
	void Reset();

private:
	void RemoveParticle(int index);

	std::vector<Particle*> m_ParticlesPtrArr;

};
