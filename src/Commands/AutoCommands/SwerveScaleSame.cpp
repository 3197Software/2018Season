#include <Commands/AutoCommands/SwerveScaleSame.h>

SwerveScaleSame::SwerveScaleSame() {
	Requires(robotDrive);
//	Requires(auxMotors); dont require 2 or bad things :(
}

// Called just before this Command runs the first time
void SwerveScaleSame::Initialize() {
	timer.Reset();
	timer.Start();

	state = 0;
	finished = false;

	driveOverDistance = CommandBase::prefs->GetFloat(
			"scaleSameDriveOverDistance", 0);
	backupDistance = CommandBase::prefs->GetFloat("scaleBackupDistance", 0);

	eleTime = CommandBase::prefs->GetFloat("scaleEleTime", 0);

	eleSpeedUp = CommandBase::prefs->GetFloat("eleSpeedUp", 0);
	eleSpeedDown = -CommandBase::prefs->GetFloat("eleSpeedDown", 0);

	baseSpeed = CommandBase::prefs->GetFloat("scaleSameBaseSpeed", 0);
	backupSpeed = CommandBase::prefs->GetFloat("scaleBackupSpeed", 0);

	straightDistance = CommandBase::prefs->GetFloat("scaleSameStraightDistance",
			0);

	if (CommandBase::oi->getGamePrefs() == 1) {
		extraSpeed = CommandBase::prefs->GetFloat("scaleSameExtraSpeedRight",
				0);
		swerveAngle = CommandBase::prefs->GetFloat("scaleSameAngleRight", 0);
	} else {
		extraSpeed = CommandBase::prefs->GetFloat("scaleSameExtraSpeedLeft", 0);
		swerveAngle = CommandBase::prefs->GetFloat("scaleSameAngleLeft", 0);
	}
	claw->ResetTimerPickup();
	claw->Reset();
}

void SwerveScaleSame::Execute() {
	SmartDashboard::PutNumber("Scale State", state);
	bool up = claw->Pickup();
	float gyroAngle = robotDrive->gyroAngle();
	if (CommandBase::oi->getGamePrefs() == -1) {
		gyroAngle *= -1;
	}
	float l = 0;
	float r = 0;
	float eleSpeed = 0;
	if (up) {
		switch (state) {
		case Straight: //drive straight distance using encoders
			if (robotDrive->encoderDistance() > straightDistance)
				IncrementState();
			l = extraSpeed;
			r = extraSpeed;
			break;
		case SwerveIn:
			if (gyroAngle > swerveAngle) {
				l = baseSpeed;
				r = baseSpeed + extraSpeed;
			} else {
				IncrementState();
			}
			break;
		case SwerveOut:
			if (gyroAngle < 0) {
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
			l = extraSpeed;
			r = extraSpeed;
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
	if (up && state <= Backup) {
		eleSpeed = eleSpeedUp;
	}
	if (up && eleSpeed != 0 && (state <= Backup))
		auxMotors->ElevatorClaw(eleSpeed);
	if (up && !(state >= Drop))
		auxMotors->Claw(-.4);
	if (CommandBase::oi->getGamePrefs() == 1)
		Drive(l, r);
	else
		Drive(r, l);
//	SmartDashboard::PutNumber("Scale State", state);
//	bool up = claw->Pickup();
//	float gyroAngle = robotDrive->gyroAngle();
//	if (CommandBase::oi->getGamePrefs() == -1) {
//		gyroAngle *= -1;
//	}
//	float eleSpeed = 0;
//	if (up) {
//		switch (state) {
//		case Straight: //drive straight distance using encoders
//			if(robotDrive->driveAuto(straightDistance, extraSpeed))
//				IncrementState();
//			break;
//		case Swerve:
//			if (robotDrive->sTurnAuto(swerveAngle, baseSpeed, false,
//					extraSpeed))
//				IncrementState();
//			break;
//		case RaiseFully:
//			if(claw->elevatorLoc(true, 1))
//			IncrementState();
//			break;
//		case DriveOverScale:
//			if (robotDrive->driveAuto(driveOverDistance, extraSpeed)) {
//				IncrementState();
//				claw->ResetTimerDrop();
//			}
//			break;
//		case Drop:
//			if (claw->Drop()) {
//				IncrementState();
//			}
//			break;
//		case Lower:
//			if (claw->elevatorLoc(false, .8)) {
//				IncrementState();
//			}
//			break;
//			End();
//		}
//	}
//	if (up && !(state >= Drop))
//		auxMotors->Claw(-.4);

}

bool SwerveScaleSame::IsFinished() {
	return finished;
}

void SwerveScaleSame::End() {
	Drive(0, 0);
	finished = true;
	timer.Stop();
}

void SwerveScaleSame::IncrementState() {
	state++;
	timer.Reset();
	timer.Start();
	robotDrive->encoderReset();
}

void SwerveScaleSame::Interrupted() {

}

void SwerveScaleSame::Drive(float l, float r) {
	robotDrive->driveBot(l, r);
}
