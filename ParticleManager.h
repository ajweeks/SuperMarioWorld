#pragma once

class Particle;

class ParticleManager
{
public:
	ParticleManager();
	virtual ~ParticleManager();

	void Tick(double deltaTime);
	void Paint();

	void AddParticle(Particle* newParticlePtr);
	void Reset();

private:
	void RemoveParticle(int index);
	void RemoveParticle(Particle* particlePtr);

	std::vector<Particle*> m_ParticlesPtrArr;

};
