#include <memory>

#include <Commands/Command.h>
#include <Commands/Scheduler.h>
#include <IterativeRobot.h>
#include <LiveWindow/LiveWindow.h>
#include <SmartDashboard/SendableChooser.h>
#include <SmartDashboard/SmartDashboard.h>

#include <Commands/AutoCommands/Nothing.h>
#include <Commands/AutoCommands/DriveInside.h>
#include <Commands/AutoCommands/DriveStraight.h>
#include <Commands/AutoCommands/DriveStraightDist.h>
#include <Commands/AutoCommands/DriveOutsideSame.h>
#include <Commands/AutoCommands/DriveOutsideOpp.h>
#include <Commands/AutoCommands/SwerveSwitch.h>
#include <Commands/AutoCommands/SwerveScaleSame.h>
#include <Commands/AutoCommands/SwerveScaleOpp.h>

#include "CommandBase.h"

class Robot: public frc::IterativeRobot {

private:
	std::unique_ptr<frc::Command> autonomousCommand;
	frc::SendableChooser<std::string> positionChooser;

public:
	void RobotInit() {
		CommandBase::init();
		positionChooser.AddDefault("Straight", "S");
		positionChooser.AddObject("Nothing", "N");
		positionChooser.AddObject("Middle", "M");
		positionChooser.AddObject("Straight Dist", "SD");
		positionChooser.AddObject("Left", "L");
		positionChooser.AddObject("Right", "R");
//		positionChooser.AddObject("Swerve Switch", "SS");
		positionChooser.AddObject("Swerve Scale Same", "SSS");
//		positionChooser.AddObject("Swerve Scale Opposite", "SSO`");

		SmartDashboard::PutData("Position", &positionChooser);

	}

	void DisabledInit() override {

	}

	void DisabledPeriodic() override {
		frc::Scheduler::GetInstance()->Run();
	}

	void AutonomousInit() override {
		CommandBase::oi->setInTele(false);

		std::string position = positionChooser.GetSelected();
		std::string gameData =
				frc::DriverStation::GetInstance().GetGameSpecificMessage();
		std::string selected = "";


		bool sideRight = true;
		if (position.compare("S") == 0) {
			selected = "straight";
			autonomousCommand.reset(new DriveStraight());

		} else if (position.compare("SD") == 0) {
			selected = "straightDist";
			autonomousCommand.reset(new DriveStraightDist());

		} else if (position.compare("M") == 0) {
			selected = "switch";
			sideRight = gameData.substr(0, 1).compare("R") == 0;
			autonomousCommand.reset(new SwerveSwitch());

		} else if (position.compare("R") == 0 || position.compare("L") == 0) {
			sideRight = gameData.substr(1, 1).compare("R") != 0;
			if (position.compare(gameData.substr(1, 1)) == 0) {
				selected = "scaleSame";
				autonomousCommand.reset(new SwerveScaleSame());
			} else {
				selected = "scaleOpp";
				autonomousCommand.reset(new SwerveScaleOpp());
			}

		} else if (position.compare("SSS") == 0) {
			selected = "SwerveScaleSame";
			autonomousCommand.reset(new SwerveScaleSame());

		} else {
			autonomousCommand.reset(new Nothing());
		}

		int gamePrefs = 1;
		if (sideRight) {
			gamePrefs = 1;
		} else {
			gamePrefs = -1;
		}
		CommandBase::oi->setGamePrefs(gamePrefs);

		SmartDashboard::PutString("Selected", selected);
		SmartDashboard::PutString("Position", position);
		SmartDashboard::PutString("SwitchSide",  gameData.substr(0, 1));
		SmartDashboard::PutString("ScaleSide",  gameData.substr(1, 1));
		SmartDashboard::PutNumber("GamePrefs", gamePrefs);

		if (autonomousCommand.get() != nullptr) {
//			autonomousCommand->_Initialize();
			autonomousCommand->Start();
		}
	}

	void AutonomousPeriodic() override {
		frc::Scheduler::GetInstance()->Run();
		RobotPeriodic();
	}

	void TeleopInit() override {
		CommandBase::oi->setInTele(true);

		if (autonomousCommand != nullptr) {
			autonomousCommand->Cancel();
		}
	}

	void TeleopPeriodic() override {
		frc::Scheduler::GetInstance()->Run();
		RobotPeriodic();
	}

	void TestPeriodic() override {
		frc::LiveWindow::GetInstance()->Run();
	}

	void RobotPeriodic() {
		CommandBase::oi->updateSensors();
	}

};

START_ROBOT_CLASS(Robot)
