#include <Commands/AutoCommands/SwerveScale.h>

SwerveScale::SwerveScale() {
	Requires(robotDrive);
//	Requires(auxMotors);
}

// Called just before this Command runs the first time
void SwerveScale::Initialize() {
	timer.Reset();
	timer.Start();

	state = 0;
	finished = false;

	driveOverDistance = CommandBase::prefs->GetFloat("scaleDriveOverDistance",
			0);
	backupDistance = CommandBase::prefs->GetFloat("scaleBackupDistance", 0);

	eleTime = CommandBase::prefs->GetFloat("scaleEleTime", 0);

	eleSpeedUp = CommandBase::prefs->GetFloat("eleSpeedUp", 0);
	eleSpeedDown = -CommandBase::prefs->GetFloat("eleSpeedDown", 0);

	baseSpeed = CommandBase::prefs->GetFloat("scaleBaseSpeed", 0);
	backupSpeed = CommandBase::prefs->GetFloat("scaleBackupSpeed", 0);

	straightDistance = CommandBase::prefs->GetFloat("scaleOppStraightDistance",
			0);
	straightAcrossDistance = CommandBase::prefs->GetFloat(
			"scaleOppStraightAcrossDistance", 0);

	if (CommandBase::oi->getGamePrefs() == 1) {
		extraSpeed = CommandBase::prefs->GetFloat("scaleOppExtraSpeedRight", 0);
		swerveAngle = CommandBase::prefs->GetFloat("scaleOppAngleRight", 0);
	} else {
		extraSpeed = CommandBase::prefs->GetFloat("scaleOppExtraSpeedLeft", 0);
		swerveAngle = CommandBase::prefs->GetFloat("scaleOppAngleLeft", 0);
	}
	claw->ResetTimerPickup();
}

// Called repeatedly when this Command is scheduled to run
void SwerveScale::Execute() {
//	SmartDashboard::PutNumber("Scale State", state);
	bool up = claw->Pickup(true);
	float gyroAngle = robotDrive->gyroAngle();
	if (CommandBase::oi->getGamePrefs() == -1) {
		gyroAngle *= -1;
	}
	float l = 0;
	float r = 0;
	float eleSpeed = 0;
	if (up) {
		switch (state) {
		case StraightToScale: //drive straight distance using encoders
			if (robotDrive->encoderDistance() > straightDistance)
				IncrementState();
			l = baseSpeed + extraSpeed;
			r = baseSpeed + extraSpeed;
			break;
		case SwerveIn:
			if (gyroAngle > swerveAngle) {
				l = baseSpeed;
				r = baseSpeed + extraSpeed;
			} else {
				IncrementState();
			}
			break;
		case StraightAcross:
//			if(side = same)
			if (robotDrive->encoderDistance() > straightAcrossDistance)
				IncrementState();
			l = baseSpeed + extraSpeed;
			r = baseSpeed + extraSpeed;
			break;
			break;
		case SwerveTowardScale:
			if (gyroAngle < -5) {
				l = baseSpeed + extraSpeed;
				r = baseSpeed;
			} else {
				IncrementState();
			}
			break;
		case RaiseFully:
			l = 0;
			r = 0;
			IncrementState();
			break;
		case DriveOverScale:
			l = baseSpeed + extraSpeed;
			r = baseSpeed + extraSpeed;
			if (robotDrive->encoderDistance() > driveOverDistance) {
				IncrementState();
				claw->ResetTimerDrop();
			}
			break;
		case Drop:
			l = 0;
			r = 0;
			if (claw->Drop()) {
				IncrementState();
			}
			break;
		case Backup:
			l = -backupSpeed;
			r = -backupSpeed;
			if (robotDrive->encoderDistance() < -backupDistance) {
				IncrementState();
			}
			break;
		case Lower:
			l = 0;
			r = 0;
			if (!timer.HasPeriodPassed(eleTime)) {
				eleSpeed = eleSpeedDown;
			} else {
				IncrementState();
			}
			break;
		default:
			l = 0;
			r = 0;
			End();
		}
	}
	if (state >= SwerveTowardScale && state <= DriveOverScale) {
		eleSpeed = eleSpeedUp;
	}
	if (eleSpeed != 0)
		auxMotors->ElevatorClaw(eleSpeed);
	if (up && !(state >= Drop))
		auxMotors->Claw(-.4);
	if (CommandBase::oi->getGamePrefs() == 1)
		Drive(l, r);
	else
		Drive(r, l);

}

// Make this return true when this Command no longer needs to run execute()
bool SwerveScale::IsFinished() {
	return finished;
}

// Called once after isFinished returns true
void SwerveScale::End() {
	Drive(0, 0);
	finished = true;
	timer.Stop();
}

void SwerveScale::IncrementState() {
	state++;
	timer.Reset();
	timer.Start();
	robotDrive->encoderReset();
}

// Called when another command which requires one or more of the same
// subsystems is scheduled to run
void SwerveScale::Interrupted() {

}

void SwerveScale::Drive(float l, float r) {
	robotDrive->driveBot(l, r);
}
