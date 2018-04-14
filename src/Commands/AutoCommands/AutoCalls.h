#ifndef AUTO_CALLS_H
#define AUTO_CALLS_H

#include "../../CommandBase.h"
#include "WPILib.h"

class AutoCalls
{
private:
	int leftOrRight;

	Timer timerPickup;
	Timer timerDrop;

	int pickupState;
	int dropState;
	int initialRun = 0;

	float maxCurrent;

	enum pickupStates {
		ResetPickupStates,
		RaiseAndFall,
		LowerPickup,
		DropCube,
		WaitForDrop,
		Close,
		RaiseWithCube
	};

	enum dropStates {
		ResetDropStates,
		Open,
		LowerDrop
	};

	void IncrementPickupState();
	void IncrementDropState();
public:
	float autoClawSpeed;

	AutoCalls();
	bool Pickup();
	bool Drop();
	void Reset();
	void ResetTimerPickup();
	void ResetTimerDrop();
	bool clawLoc(int loc);
	bool elevatorLoc(bool loc, float speed);
};

#endif
