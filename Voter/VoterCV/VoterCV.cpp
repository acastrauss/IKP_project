#include "VoterCV.h"
#include <iostream>

VoterCV::VoterCV(USHORT maxVoters)
	: m_CurrentVotersNum(0), m_MaxVoters(maxVoters)
{
	InitializeCriticalSection(&csVoters);
	InitializeConditionVariable(&cv);
}

VoterCV::~VoterCV()
{
	DeleteCriticalSection(&csVoters);
}

void VoterCV::StopIfMax()
{
	EnterCriticalSection(&csVoters);
	
	while (m_CurrentVotersNum == m_MaxVoters)
	{
		std::cout << "Stopping because there is already max number of voters voting." << std::endl;
		SleepConditionVariableCS(&cv, &csVoters, INFINITE);
	}

	LeaveCriticalSection(&csVoters);
}

void VoterCV::IncreaseVoters()
{
	EnterCriticalSection(&csVoters);

	m_CurrentVotersNum++;

	LeaveCriticalSection(&csVoters);
}

void VoterCV::VoterFinished()
{
	EnterCriticalSection(&csVoters);

	m_CurrentVotersNum--;

	LeaveCriticalSection(&csVoters);

	WakeConditionVariable(&cv);
}