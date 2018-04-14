#include <Commands/AutoCommands/AutoCalls.h>

AutoCalls::AutoCalls() {
	Reset();
//	double rAndF = AutoTimings->GetDouble("RaiseAndFall (Time)", 0.5);
//	double rAndFSpeed = AutoTimings->GetDouble("RaiseAndFall (Speed)", -0.5);
//	double lPickup = AutoTimings->GetDouble("Lower Pickup (Speed)", 0.5);
//	double close = AutoTimings->GetDouble("Close (Speed)", -0.5);
	autoClawSpeed = CommandBase::prefs->GetFloat("autoClawSpeed", 0);

}

bool AutoCalls::Pickup() {
	SmartDashboard::PutNumber("Pickup States", pickupState);

	float clawSpeed = 0;
	float eleClawSpeed = 0;

	if (pickupState == ResetPickupStates) {
		Reset();
		pickupState++;
	}


	switch (pickupState) {
	case RaiseAndFall: //raise claw at speed for time (block falls)
//		if (!timerPickup.HasPeriodPassed(0.75)) {
//			eleClawSpeed = (0.5);
//		} else {
//			IncrementPickupState();
//		}
		IncrementPickupState();
		break;
	case LowerPickup:
		//lower claw until lower limit
//		if (!CommandBase::auxMotors->ElevatorClawBotLim()) {
//			eleClawSpeed = (-0.5);
//		} else {
//			IncrementPickupState();
		IncrementPickupState();
//		}
		break;
	case DropCube:
		if (!timerPickup.HasPeriodPassed(.25)) {
			clawSpeed = (-autoClawSpeed);
		} else {
			IncrementPickupState();
		}
		break;
	case WaitForDrop:
		if (timerPickup.HasPeriodPassed(0.1)) {
			IncrementPickupState();
		}
		break;
	case Close:
		//close claw until current limit
		if (!timerPickup.HasPeriodPassed(.75)) {
			clawSpeed = (-autoClawSpeed);
		} else {
			IncrementPickupState();
		}
		break;
	case RaiseWithCube:
		//raise claw at speed for time (raise block)
		if (!timerPickup.HasPeriodPassed(1)) {
			eleClawSpeed = (1);
		} else {
			IncrementPickupState();
		}
		break;
	default:
		break;
	}
	CommandBase::auxMotors->Claw(clawSpeed);
	CommandBase::auxMotors->ElevatorClaw(eleClawSpeed);

	if (pickupState > Close) {
		Reset();
		return true;
	} else
		return false;
}

bool AutoCalls::Drop() {
	if (dropState == ResetDropStates) {
		Reset();
		dropState++;
	}

	switch (dropState) {
	case Open:
//		if (CommandBase::auxMotors->ClawCurrent() < maxCurrent ) {
//		if (!timerDrop.HasPeriodPassed(1)) {
		if (!CommandBase::auxMotors->ClawRetractLim()) {
			CommandBase::auxMotors->Claw(0.65);
		} else {
			IncrementDropState();
		}
		break;
//	case LowerDrop:
//		if (!CommandBase::auxMotors->ElevatorClawForwardLimit()) {
//			CommandBase::auxMotors->ElevatorClaw(-.5);
//		} else {
//			IncrementPickupState();
//		}
//		break;
	default:
		Reset();
		return true;
	}
	return false;
}

void AutoCalls::Reset() {
	leftOrRight = CommandBase::oi->getGamePrefs();
	pickupState = LowerPickup;
	dropState = -1;
	IncrementPickupState();
	IncrementDropState();
}

void AutoCalls::IncrementPickupState() {
	pickupState++;
	ResetTimerPickup();
}

void AutoCalls::IncrementDropState() {
	dropState++;
	ResetTimerDrop();
}

void AutoCalls::ResetTimerDrop() {
	timerDrop.Reset();
	timerDrop.Start();

}

void AutoCalls::ResetTimerPickup() {
	timerPickup.Reset();
	timerPickup.Start();
}
//Get in the zone, AUTO ZONE
bool AutoCalls::clawLoc(int loc)
{
		if(loc == 0) // Close inside bot
		{
			CommandBase::auxMotors->Claw(0.65);
			return CommandBase::auxMotors->ClawRetractLim();
		}
		else // Close around cube
		{
			CommandBase::auxMotors->Claw(-0.65);
			return CommandBase::auxMotors->ClawGrabLim();
		}
}
bool AutoCalls::elevatorLoc(bool loc, float speed)
{
	if(loc == 0) // At bottom
	{
		CommandBase::auxMotors->ElevatorClaw(-1 * speed);
		return CommandBase::auxMotors->ElevatorClawBotLim();
	}
	else // At top
	{
		CommandBase::auxMotors->ElevatorClaw(speed);
		return CommandBase::auxMotors->ElevatorClawTopLim();
	}
}
