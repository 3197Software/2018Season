	#include "Mechanisms.h"

#include "../Commands/AuxiliaryMotors.h"

#include "../RobotMap.h"
#include <math.h>

#include "WPILib.h"

#include "ctre/Phoenix.h"

#define MAXRPM 534
#define LARGE_AMOUNT_OF_CURRENT 50
#define PEAK_CLAW_CURRENT 5

Mechanisms::Mechanisms() :
		Subsystem("AuxiliaryMotors") {

	winchA = new WPI_TalonSRX(5);			//Pulls rope down for climb
	winchB = new WPI_TalonSRX(6);			//Pulls rope down for climb
	claw = new WPI_TalonSRX(7);				//Shoots balls
	elevatorWinch = new WPI_TalonSRX(8);	//Moves climber mech up
	elevatorClawA = new WPI_TalonSRX(9);	//Moves claw up and down
	elevatorClawB = new WPI_TalonSRX(10);	//Moves claw up and down

	elevatorClawB->Follow(*elevatorClawA);
	elevatorClawB->SetInverted(true);

	winchB->Follow(*winchA);

}

void Mechanisms::InitDefaultCommand() {
	SetDefaultCommand(new AuxiliaryMotors());

	maxObservedClawCurrent = 0;

//	elevatorWinch->Config_kF(CommandBase::robotDrive->kPIDLoopIdx,	//ID (0), data, timeout
//			CommandBase::robotDrive->prefs->GetFloat("ElevatorClaw F",
//					CommandBase::robotDrive->defaultF),
//			CommandBase::robotDrive->kTimeoutMs);
//
//	elevatorWinch->Config_kP(CommandBase::robotDrive->kPIDLoopIdx,	//ID (0), data, timeout
//			CommandBase::robotDrive->prefs->GetFloat("ElevatorClaw P",
//					CommandBase::robotDrive->defaultP),
//			CommandBase::robotDrive->kTimeoutMs);
//
//	elevatorWinch->Config_kI(CommandBase::robotDrive->kPIDLoopIdx,	//ID (0), data, timeout
//			CommandBase::robotDrive->prefs->GetFloat("ElevatorClaw I",
//					CommandBase::robotDrive->defaultI),
//			CommandBase::robotDrive->kTimeoutMs);
//
//	elevatorWinch->Config_kD(CommandBase::robotDrive->kPIDLoopIdx,	//ID (0), data, timeout
//			CommandBase::robotDrive->prefs->GetFloat("ElevatorClaw D",
//					CommandBase::robotDrive->defaultD),
//			CommandBase::robotDrive->kTimeoutMs);
}

void Mechanisms::Winch(float speed) {
	winchA->Set(speed);
}

void Mechanisms::Claw(float speed) {
	claw->Set(speed);
}

void Mechanisms::ElevatorWinch(float speed) {
	//elevatorWinch->Set(ControlMode::Velocity, (speed * MAXRPM * 4096 * 600 );
	elevatorWinch->Set(speed);
	SmartDashboard::PutNumber("Elevator Winch Encoder",
			elevatorWinch->GetSensorCollection().GetQuadraturePosition());
	SmartDashboard::PutNumber("Winch Current",
			elevatorWinch->GetOutputCurrent());
}

void Mechanisms::ElevatorClaw(float speed) {
	elevatorClawA->Set(speed);
	float current = elevatorClawA->GetOutputCurrent();
	if (current > maxObservedClawCurrent)
		maxObservedClawCurrent = current;
	SmartDashboard::PutNumber("Max Elevator Claw Current", maxObservedClawCurrent);
}

bool Mechanisms::ClawRetractLim() {
	bool trip = 0 != claw->GetSensorCollection().IsFwdLimitSwitchClosed();
	SmartDashboard::PutBoolean("Claw Forward Limit (retract)", trip);
	return trip;
}

bool Mechanisms::ClawGrabLim() {
	bool trip = 0 != claw->GetSensorCollection().IsRevLimitSwitchClosed();
	SmartDashboard::PutBoolean("Claw Reverse Limit (grab)", trip);
	return trip;
}

bool Mechanisms::ElevatorClawBotLim() {
	bool trip = 0
			!= elevatorClawA->GetSensorCollection().IsRevLimitSwitchClosed();
	SmartDashboard::PutBoolean("Elevator Claw Forward Limit (bot)", trip);
	return trip;
}

bool Mechanisms::ElevatorClawTopLim() {
	bool trip = 0
			!= elevatorClawA->GetSensorCollection().IsFwdLimitSwitchClosed();
	SmartDashboard::PutBoolean("Elevator Claw Reverse Limit (top)", trip);
	return trip;
}

bool Mechanisms::ElevatorWinchForwardLimit() {
	bool trip = 0
			!= elevatorWinch->GetSensorCollection().IsFwdLimitSwitchClosed();
	SmartDashboard::PutBoolean("Elevator Winch Forward Limit (bot)", trip);
	return trip;
}

bool Mechanisms::ElevatorWinchReverseLimit() {
	bool trip = 0
			!= elevatorWinch->GetSensorCollection().IsRevLimitSwitchClosed();
	SmartDashboard::PutBoolean("Elevator Winch Reverse Limit (top)", trip);
	return trip;
}

void Mechanisms::UpdateCurrent() {
	float clawCurrent = claw->GetOutputCurrent();
	SmartDashboard::PutNumber("Claw Current", clawCurrent);
	float winchCurrentA = winchA->GetOutputCurrent();
	SmartDashboard::PutNumber("Winch A Current", winchCurrentA);
	float winchCurrentB = winchB->GetOutputCurrent();
	SmartDashboard::PutNumber("Winch B Current", winchCurrentB);
	float eleClawCurrent = elevatorClawA->GetOutputCurrent();
	SmartDashboard::PutNumber("Ele Claw Current", eleClawCurrent);
	float eleWinchCurrent = elevatorWinch->GetOutputCurrent();
	SmartDashboard::PutNumber("Ele Winch Current", eleWinchCurrent);

}
