#pragma once

#include <Windows.h>

/// <summary>
/// Class used to stop client application when there is already max 
/// amount of clients voting
/// </summary>
class VoterCV
{
private:
	CRITICAL_SECTION csVoters;
	CONDITION_VARIABLE cv;

	USHORT m_CurrentVotersNum;
	USHORT m_MaxVoters;
public:
	VoterCV(USHORT maxVoters);
	~VoterCV();

	/// <summary>
	/// Method that sleeps if there is max number of clients
	/// </summary>
	void StopIfMax();

	void IncreaseVoters();

	/// <summary>
	/// Decreases number of voters used, and wakes StopIfMax
	/// </summary>
	void VoterFinished();
};

