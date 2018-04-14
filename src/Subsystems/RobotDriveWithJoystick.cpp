#include "RobotDriveWithJoystick.h"

#include "../Commands/TankControl.h"

#include "../RobotMap.h"
#include <math.h>

#include "WPILib.h"

#include "ctre/Phoenix.h"

#define SPEED_MULTIPLIER 1
#define ANGLE_DEADZONE 1
#define TURN_ANGLE_DEADZONE 2
#define PERCENT_CHANGE_PER_DEGREE .0075
#define MIN_SPEED .55
#define ENCODER_CONVERSION (M_PI * 6 / 84)/4

#define AUTO_SPEED .5
#define AUTO_TURN_SPEED .55

int firstRun = 0;
bool firstPass = false;
float initGyro;

RobotDriveWithJoystick::RobotDriveWithJoystick() :
		Subsystem("RobotDriveWithJoystick") {

	frontRight = new WPI_TalonSRX(1);
	rearRight = new WPI_TalonSRX(2);
	frontLeft = new WPI_TalonSRX(3);
	rearLeft = new WPI_TalonSRX(4);

	rDrive = new DifferentialDrive(*frontLeft, *frontRight);

	frontLeft->SetInverted(true);
	frontRight->SetInverted(true);
	rearLeft->SetInverted(true);
	rearRight->SetInverted(true);

	rearLeft->Follow(*frontLeft);
	rearRight->Follow(*frontRight);

	rDrive->SetSafetyEnabled(false);

	frontRight->ConfigSelectedFeedbackSensor(FeedbackDevice::QuadEncoder, 0, 0);
//	frontRight->SetSensorPhase(false); // polarity of the rotation

//	frontRight->ConfigNominalOutputForward(0, kTimeoutMs);
//	frontRight->ConfigNominalOutputReverse(0, kTimeoutMs);
//	frontRight->ConfigPeakOutputForward(1, kTimeoutMs);
//	frontRight->ConfigPeakOutputReverse(-1, kTimeoutMs);

	frontLeft->ConfigSelectedFeedbackSensor(FeedbackDevice::QuadEncoder, 0, 0);
//	frontLeft->SetSensorPhase(false); // polarity of the rotation

//	frontLeft->ConfigNominalOutputForward(0, kTimeoutMs);
//	frontLeft->ConfigNominalOutputReverse(0, kTimeoutMs);
//	frontLeft->ConfigPeakOutputForward(1, kTimeoutMs);
//	frontLeft->ConfigPeakOutputReverse(-1, kTimeoutMs);

	talonF = defaultF;
	talonP = defaultP;
	talonI = defaultI;
	talonD = defaultD;

}

void RobotDriveWithJoystick::InitDefaultCommand() {
	SetDefaultCommand(new TankControl());
//	SPIGyro.Calibrate();

	autoDriveSpeed = CommandBase::prefs->GetFloat("autoDriveSpeed", 0);
	autoTurnSpeed = CommandBase::prefs->GetFloat("autoTurnSpeed", 0);
	autoTurnAngle = CommandBase::prefs->GetFloat("autoTurnAngle", 0);
	autoDriveDist = CommandBase::prefs->GetFloat("autoDriveDist", 0);
	autoDriveTime = CommandBase::prefs->GetFloat("autoDriveTime", 0);

	encoderReset();

	talonF = CommandBase::prefs->GetFloat("Drive F", defaultF);
	talonP = CommandBase::prefs->GetFloat("Drive P", defaultP);
	talonI = CommandBase::prefs->GetFloat("Drive I", defaultI);
	talonD = CommandBase::prefs->GetFloat("Drive D", defaultD);

//	frontRight->Config_kF(kPIDLoopIdx, talonF, kTimeoutMs);
//	frontRight->Config_kP(kPIDLoopIdx, talonP, kTimeoutMs);
//	frontRight->Config_kI(kPIDLoopIdx, talonI, kTimeoutMs);
//	frontRight->Config_kD(kPIDLoopIdx, talonD, kTimeoutMs);
//	frontRight->Set(ControlMode::PercentOutput, 0);

//	frontLeft->Config_kF(kPIDLoopIdx, talonF, kTimeoutMs);
//	frontLeft->Config_kP(kPIDLoopIdx, talonP, kTimeoutMs);
//	frontLeft->Config_kI(kPIDLoopIdx, talonI, kTimeoutMs);
//	frontLeft->Config_kD(kPIDLoopIdx, talonD, kTimeoutMs);
//	frontLeft->Set(ControlMode::PercentOutput, 0);
	swerved = false;
}

void RobotDriveWithJoystick::driveBot(float left, float right) {
	rDrive->TankDrive(left * SPEED_MULTIPLIER, right * SPEED_MULTIPLIER, false);
	SmartDashboard::PutNumber("L Speed", left);
	SmartDashboard::PutNumber("R Speed", right);
	SmartDashboard::PutData("Drive", rDrive);
}

void RobotDriveWithJoystick::advancedDriveBot(float left, float right,
		float preferred) {
	float actual = gyroAngle();
	float deltaAngle = (preferred - actual);
	if (abs(deltaAngle) > ANGLE_DEADZONE) {
		float speedChange = deltaAngle * PERCENT_CHANGE_PER_DEGREE;
		right -= speedChange;
		left += speedChange;
	}

	driveBot(left, right);
}

bool RobotDriveWithJoystick::advancedTurnBot(float speed, float preferred) {
	float actual = gyroAngle();
	float deltaAngle = (preferred - actual);
	bool turn = abs(deltaAngle) > TURN_ANGLE_DEADZONE;
	if (turn) {
		if (abs(speed) < MIN_SPEED) {
			speed = MIN_SPEED;

		}
//		if (deltaAngle < 0) {
//			speed *= -1;
//		}
	} else {
		speed = 0;
	}
	driveBot(speed, -speed);
	return !turn;
}

float RobotDriveWithJoystick::gyroAngle(/*bool adjusted*/) {
	bool adjusted = false;
	float raw = SPIGyro.GetAngle();

	if (adjusted) {
		while (raw < -180) {
			raw += 360;
		}
		while (raw > 180) {
			raw -= 360;
		}
	}
	SmartDashboard::PutNumber("GyroNumber", raw);
	SmartDashboard::PutData("Gyro", &SPIGyro);
	return raw;
}

void RobotDriveWithJoystick::gyroReset() {
	SPIGyro.Reset();
}

float RobotDriveWithJoystick::encoderDistance() {
//	float raw = (frontLeft->GetSelectedSensorPosition(kPIDLoopIdx)
//			+ frontRight->GetSelectedSensorPosition(kPIDLoopIdx)) / 2;
	float r = frontRight->GetSelectedSensorPosition(0) * ENCODER_CONVERSION;
	float l = frontLeft->GetSelectedSensorPosition(0) * ENCODER_CONVERSION;
	SmartDashboard::PutNumber("Right Encoder", r);
	SmartDashboard::PutNumber("Left Encoder", l);

//	float l, r;
//	l = CommandBase::oi->encodersL();
//	r = CommandBase::oi->encodersR();
	return (l + r) / 2;
}
void RobotDriveWithJoystick::encoderReset() {
	frontLeft->SetSelectedSensorPosition(0, 0, 0);
	frontRight->SetSelectedSensorPosition(0, 0, 0);
	CommandBase::oi->encoderReset();
}

//Get in the zone, the AUTO ZONE

bool RobotDriveWithJoystick::sTurnAuto(float gyro, float speed, bool halfTurn,
		float deltaSpeed) {
	// desired angle , desired speed, return to 0?
	float l, r;
	if(firstPass)
	{
		firstPass = false;
		initGyro = gyroAngle();
	}
	if (!swerved) {
		if (gyro - gyroAngle() > -ANGLE_DEADZONE
				&& gyro - gyroAngle() < ANGLE_DEADZONE) {
			l = gyro < (gyroAngle() - initGyro) ? speed : speed + deltaSpeed;
			r = gyro < (gyroAngle() - initGyro) ? speed + deltaSpeed : speed;
			driveBot(l, r);
		} else {
			swerved = true;
		}
		return false;
	} else if (!halfTurn) {
		if (!(gyroAngle() < ANGLE_DEADZONE && gyroAngle() > -ANGLE_DEADZONE)) {
			r = gyro < (gyroAngle() - initGyro) ? speed : speed + deltaSpeed;
			l = gyro < (gyroAngle() - initGyro) ? speed + deltaSpeed : speed;
			driveBot(l, r);
			return false;
		}
	}
	swerved = false;
	return true;
}

bool withinDesired(float current, float desired, float zone) {
	return (desired - current) > -zone && (desired - current) < zone;
}

bool RobotDriveWithJoystick::driveAuto(float distance, float speed) {
	float currentDist = (frontLeft->GetSelectedSensorPosition(0)
			* ENCODER_CONVERSION);
	if (currentDist < distance) {
		driveBot(speed, speed);
		return false;
	} else {
		firstRun = 0;
		return true;

	}
}
