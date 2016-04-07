#include "stdafx.h"

#include "ParticleManager.h"
#include "Game.h"
#include "Particle.h"

ParticleManager::ParticleManager()
{
	Reset();
}

ParticleManager::~ParticleManager()
{
	Reset();
}

void ParticleManager::Reset()
{
	for (size_t i = 0; i < m_ParticlesPtrArr.size(); ++i)
	{
		delete m_ParticlesPtrArr[i];
	}
	m_ParticlesPtrArr.clear();
}

void ParticleManager::AddParticle(Particle* newParticlePtr)
{
	for (size_t i = 0; i < m_ParticlesPtrArr.size(); ++i)
	{
		if (m_ParticlesPtrArr[i] == nullptr)
		{
			m_ParticlesPtrArr[i] = newParticlePtr;
			return;
		}
	}

	m_ParticlesPtrArr.push_back(newParticlePtr);
}

void ParticleManager::Tick(double deltaTime)
{
	for (size_t i = 0; i < m_ParticlesPtrArr.size(); ++i)
	{
		if (m_ParticlesPtrArr[i] != nullptr)
		{
			if (m_ParticlesPtrArr[i]->Tick(deltaTime))
			{
				RemoveParticle(i);
			}
		}
	}
}

void ParticleManager::Paint()
{
	for (size_t i = 0; i < m_ParticlesPtrArr.size(); ++i)
	{
		if (m_ParticlesPtrArr[i] != nullptr)
		{
			m_ParticlesPtrArr[i]->Paint();
		}
	}
}

void ParticleManager::RemoveParticle(int index)
{
	delete m_ParticlesPtrArr[index];
	m_ParticlesPtrArr[index] = nullptr;
}

void ParticleManager::RemoveParticle(Particle* particlePtr)
{
	for (size_t i = 0; i < m_ParticlesPtrArr.size(); ++i)
	{
		if (particlePtr == m_ParticlesPtrArr[i])
		{
			RemoveParticle(i);
		}
	}
}