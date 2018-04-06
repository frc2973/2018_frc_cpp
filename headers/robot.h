/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

//Team 2973 2018 Dev Robot - Gear. Competition Legal: 2018 Uses the 2018 control panel, networking version 23.
//@@@@@@@@@@@@@@ IS TO DO
//MEMORY LEAK IN THE client.h
//FIX IT
#ifndef _ROBOT_H_
#define _ROBOT_H_

#include <iostream>
#include <string>
#include <thread>
#include <cmath>
#include <unistd.h>

#include <chrono>

#include <WPILib.h>
#include <IterativeRobot.h>
#include <LiveWindow/LiveWindow.h>
#include <SmartDashboard/SendableChooser.h>
#include <SmartDashboard/SmartDashboard.h>

#include <ctime>

#include "../headers/networking.h"
#include "../headers/ports.h"

//Control settings
#define XBOX_DEADZONE 0.11
#define VAL_POW 2.0
#define MAX_SPEED 0.4
#define RIGHTTRACK_MULT 1.155 //.3
#define RIGHTTRACK_MULT_HS 1.088 //.6

//Build settings
#define DEBUG_BUILD
#define USE_ADJUSTABLE_VAL_POW
#define smoothai
//#define COMPRESSOR_POWER_SAFETY
//#define turngoai

//@@@@@ TO-DO: MAKE CONTROL PANEL SEND UPDATE EVERY SECOND SO THAT LOOP CAN TERMINATE IF NEEDED

#define FEET(_ft) 360 * _ft / 1.05 //Research how to ###ft
#define METERS(_m) 1078 * _m


struct point {
	double p1, p2;
	point operator-(point rhand) {
		return point { p1 - rhand.p1, p2 - rhand.p2 };
	}
};

class Robot: public frc::IterativeRobot, public NetworkingEventHandler {
public:

	/* Messy variables to be sorted */

	XboxController xboxPrimary;
	XboxController xboxSecondary;
	NetworkingClient networkingClient;
	DigitalInput blockIR;
	VictorSP winch;
	MRCCommand::MRCCommandBodyBase::ADATA::T4TYPE autoData;
	bool hasAutoData = false;
	bool terminateThreadsLocal = false;
	DigitalInput frontIR;
	DigitalInput leftAntenna;
	frc::Compressor compressor;
	Solenoid claw0 { PCM_CLAW_LEFT };
	Solenoid claw1 { PCM_CLAW_RIGHT };
	double enforceDoubleAbs(double val) {	//Use double with abs
		if (val < 0.0)
			return -val;

		return val;
	}

	/* Drive train encoders */

	Encoder encLeft;
	Encoder encRight;
	point encOffset = { 0, 0 };
	point getEncoders() {
		return point { encLeft.Get(), -encRight.Get() } - encOffset;
	}
	void encReset() {
		encOffset = point { encLeft.Get(), -encRight.Get() };
	}

	/* Drive train */

	RobotDrive robotDrive;
	void CalibratedRobotDrive(double left, double right, bool s = false) {
		if (right != 0.0) {
			if (right < 0.4)
				right *= RIGHTTRACK_MULT;
			else
				right *= RIGHTTRACK_MULT_HS;

		}
		robotDrive.TankDrive(left, right, s);
	}

	int status = 2;
	void postStatus();

	int trackingStatus = 0;
	float trackingLocX = 0;
	float trackingLocY = 0;
	float trackingWidth = 0;
	bool handleMessage(NetworkingClient* networkingClient, MRCCommand& recCom);

	std::thread* batLoop_Thread;
	void batLoop();
	static void batLoop_Static(Robot* _this);

	/* Robot constructors, destructors, and other maintaining stuff */

	Robot() :
			limitTop(DIO_LIMIT_TOP), armMotor(PWM_ARM_ROTATE), liftMotor(
			PWM_LIFT), leftAntenna(DIO_ANTENNA_LEFT), encLeft(
			DIO_ENC_LEFT_CHANNEL_A, DIO_ENC_LEFT_CHANNEL_B, false,
					Encoder::EncodingType::k4X), encRight(
			DIO_ENC_RIGHT_CHANNEL_A, DIO_ENC_RIGHT_CHANNEL_B, false,
					Encoder::EncodingType::k4X), robotDrive(PWM_LEFT_DRIVE,
			PWM_RIGHT_DRIVE), xboxPrimary(XBOX_PRIMARY), xboxSecondary(XBOX_SECONDARY), blockIR(
			DIO_ANTENNA_BLOCK), frontIR(DIO_ANTENNA_FRONT), limitBottom(
					DIO_LIMIT_BOTTOM), limitArmTop(DIO_LIMIT_ARM_TOP), winch(PWM_WINCH), limitArmBottom(DIO_LIMIT_ARM_BOTTOM) {
		status = 2;
		terminateThreadsLocal = false;
		batLoop_Thread = new std::thread(batLoop_Static, this);
		if (!networkingClient.networkingInit("10.29.73.5", "5802", 0, this)) // @@@@@@@@@ IP NEEDS TO BE MDNS
			SmartDashboard::PutNumber("NETWORK FATAL", clock());
		hasAutoData = false;
		rmReset();
	}

	~Robot() {
		//Shutdown networking
		terminateThreadsLocal = true;
		if (batLoop_Thread) {
			batLoop_Thread->join();
			delete batLoop_Thread;
			batLoop_Thread = NULL;
		}
		terminateThreadsLocal = false;
		encRight.SetReverseDirection(true);
	}

	void RobotInit() override {
		rmReset();
		winch.Set(0.0);
	}

	void DisabledInit() override {
		claw0.Set(false);
		claw1.Set(false);
		status = 2;
		postStatus();
		armMotor.Set(0.0);
		liftMotor.Set(0.0);
		winch.Set(0.0);
		compressor.SetClosedLoopControl(false);
		encReset();
	}

	void TestInit() override {

		winch.Set(0.0);
	}

	void TestPeriodic() override {

	}

	/* Robot Underlying System */
	/* Handles tasks on the robot that are simple but need to be done properly */

	//The following motors are only to be used by the RM (/RUS).
	Victor liftMotor;
	Victor armMotor;

	DigitalInput limitTop;
	DigitalInput limitBottom;
	DigitalInput limitArmTop;
	DigitalInput limitArmBottom;

	struct RM {
		enum LIFT {
			LIFT_UP, LIFT_DOWN
		} lift;
		enum LIFTSTATUS {
			LIFTSTATUS_MOVING, LIFTSTATUS_REACHED
		} liftStatus;

		void setLift(LIFT nLift) {
			liftStatus = LIFTSTATUS_MOVING;
			lift = nLift;
		}

		//Not set up yet, either
		enum ARM {
			ARM_UP, ARM_DOWN
		} arm;
		enum ARMSTATUS {
			ARMSTATUS_MOVING, ARMSTATUS_REACHED
		} armStatus;
		void setArm(ARM nArm) {
			armStatus = ARMSTATUS_MOVING;
			arm = nArm;
		}
	} rm;

	void rmReset();
	void rmUpdate();

	/* Autonomous */

	clock_t timeApproachTrack = 0;
	int autoStage = 0;
	long long waitUntil; // AUTOACTION_WAIT
	int shaftAssistDistance;
	std::string gSM; // Untrustworthy

	enum {
		AUTOPLAN_CORNERTRACKPLACERIGHT,
		AUTOPLAN_CORNERTRACKPLACELEFT,
		AUTOPLAN_NONE,
		AUTOPLAN_CROSSAUTONOMOUSLINE,
		AUTOPLAN_DEV,
		AUTOPLAN_CROSSSWITCHTOLEFT,
		AUTOPLAN_RIGHTSWITCH,
		AUTOPLAN_FALLBACKLEFT,
		AUTOPLAN_FALLBACKRIGHT,
		AUTOPLAN_FALLBACKFORWARDNOSHAFT,
		AUTOPLAN_FALLBACKLEFTSHAFTASSIST,
		AUTOPLAN_FALLBACKRIGHTSHAFTASSIST,
		AUTOPLAN_BOTHSWITCH
	} autoPlan;

	enum {
		AUTOACTION_APPROACHTRACK,
		AUTOACTION_STOP,
		AUTOACTION_PLACECUBESWITCH,
		AUTOACTION_TIMEAPPROACHTRACK,
		AUTOACTION_APPROACHTRACKGRAB,
		AUTOACTION_SHAFTASSISTRIGHT90,
		AUTOACTION_SHAFTASSISTLEFT90,
		AUTOACTION_SHAFTASSISTDISTANCE,
		AUTOACTION_SHAFTOVERCOMPENSATELEFT,
		AUTOACTION_DEV_ENDLESSLEFT,
		AUTOACTION_WAIT,
		AUTOACTION_APPROACHUNTILLEFTANTENNA,
		AUTOACTION_LIFT0d5S,
		AUTOACTION_TIMEAPPROACH,
		AUTOACTION_SHAFTASSISTTIMEAPPROACH,
		AUTOACTION_RUNRIGHTDRIVE,
		AUTOACTION_RUNLEFTDRIVE
	} autoAction;

	void AutonomousInit() override;
	void stepAutoStage();
	void AutonomousPeriodic() override;

	/* Teleop */

	clock_t startTime = 0;

	enum {
		TELEOP_CONTROL, TELEOP_SEEK, TELEOP_SPIN, TELEOP_SHAFT_MOVE_ASSIST
	} currentTeleopState = TELEOP_CONTROL;

	void TeleopInit() override;
	void TeleopPeriodic() override;

};

#endif
