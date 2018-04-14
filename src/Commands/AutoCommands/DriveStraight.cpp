#include <Commands/AutoCommands/DriveStraight.h>
#include <Subsystems/RobotDriveWithJoystick.h>
DriveStraight::DriveStraight() {
	Requires(robotDrive);
}

// Called just before this Command runs the first time
void DriveStraight::Initialize() {
	timer.Reset();
	timer.Start();

	state = 0;
	finished = false;
}

// Called repeatedly when this Command is scheduled to run
void DriveStraight::Execute() {
	SmartDashboard::PutNumber("State DriveStraight", state);
	switch (state) {
	case 0:
		if (claw->Pickup()) // returns true when finished
			state++;
		break;
	case 1:
		if (!timer.HasPeriodPassed(robotDrive->autoDriveTime)){
			Drive(-robotDrive->autoDriveSpeed);
			auxMotors->ElevatorClaw(.4);
		}
		else
			state++;
		break;
	default:
	case 2:
		auxMotors->ElevatorClaw(0);
		End();
	}
}

// Make this return true when this Command no longer needs to run execute()
bool DriveStraight::IsFinished() {
	return finished;
}

// Called once after isFinished returns true
void DriveStraight::End() {
	Drive(0);
	finished = true;
	timer.Stop();
}

// Called when another command which requires one or more of the same
// subsystems is scheduled to run
void DriveStraight::Interrupted() {

}

void DriveStraight::Drive(float speed) {
	speed *= -1;
	robotDrive->advancedDriveBot(speed, speed, 0);
}
