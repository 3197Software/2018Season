#ifndef ROBOT_DRIVE_JOYSTICK_H
#define ROBOT_DRIVE_JOYSTICK_H

#include <Commands/Subsystem.h>
#include "WPILib.h"
#include "ctre\phoenix\MotorControl\CAN\WPI_TalonSRX.h"
#include "ctre/Phoenix.h"

class RobotDriveWithJoystick: public frc::Subsystem {
public:
	RobotDriveWithJoystick();
	void InitDefaultCommand() override;
	void driveBot(float left, float right);
	void advancedDriveBot(float left, float right, float preferred);
	bool advancedTurnBot(float speed, float preferred);

	bool sTurnAuto(float gyro, float speed, bool halfTurn, float deltaSpeed);
	bool driveAuto(float distance, float speed);

	float gyroAngle(/*bool a*/);
	void gyroReset();
	float encoderDistance();
	void encoderReset();

	float autoDriveSpeed;
	float autoTurnSpeed;
	float autoTurnAngle;
	float autoDriveDist;
	float autoDriveTime;

	const int kTimeoutMs = 0;
	const int kPIDLoopIdx = 0;

	const float defaultF = 0.1097;
	const float defaultP = 0.22;
	const float defaultI = 0.0;
	const float defaultD = 0.0;
private:
	WPI_TalonSRX *frontRight;
	WPI_TalonSRX *rearRight;
	WPI_TalonSRX *frontLeft;
	WPI_TalonSRX *rearLeft;

	frc::DifferentialDrive *rDrive;
	ADXRS450_Gyro SPIGyro;


	float talonF;
	float talonP;
	float talonI;
	float talonD;

	bool swerved;

};

#endif
