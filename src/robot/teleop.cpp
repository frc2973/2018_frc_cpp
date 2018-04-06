#include "../../headers/robot.h"

void Robot::TeleopInit() {
	status = 4;
	postStatus();
	currentTeleopState = TELEOP_CONTROL;
	MRCCommand com;
	com.setType2(MRCCommand::Commands::MRCC_SETAUTOPILOT, 0);
	networkingClient.deliver(com);

	winch.Set(0.0);
	rmReset();

	compressor.SetClosedLoopControl(true);
}

void Robot::TeleopPeriodic() {
	point pt7 = getEncoders();
	SmartDashboard::PutNumber("pt7L", pt7.p1);
	SmartDashboard::PutNumber("pt7R", pt7.p2);

	//Winch
	if (xboxSecondary.GetYButton()) {

		winch.Set(-1.0);//negative
	}
	else if (xboxSecondary.GetXButton()) {
		winch.Set(1.0);//positive down
	}
		else{



		winch.Set(0.0);
	}

	//Lift

	//Override switch
	if (xboxSecondary.GetRawAxis(5) > 0.15)
		liftMotor.Set(0.8*xboxSecondary.GetRawAxis(5) );
	else if (xboxSecondary.GetRawAxis(5) < -0.15)
		liftMotor.Set(0.8*xboxSecondary.GetRawAxis(5) );
	//Use switch
	else if (xboxSecondary.GetRawAxis(1) > 0.15)
		liftMotor.Set(0.8*xboxSecondary.GetRawAxis(1)  );
	else if (xboxSecondary.GetRawAxis(1) < -0.15&&limitTop.Get())
		liftMotor.Set(0.8*xboxSecondary.GetRawAxis(1)  );
	else
		liftMotor.Set(0.0);

	//Arm
	if (xboxSecondary.GetAButton())
	{
		armMotor.Set(0.40); //down pos
	}
	else if (xboxSecondary.GetBButton())
	{
		if (blockIR.Get()) {
			if (!limitArmTop.Get())
				armMotor.Set(-1.0); //up
			else
				armMotor.Set(0.0);
		} else {
			if (!limitArmTop.Get())
				armMotor.Set(-0.80); //up
			else
				armMotor.Set(0.0);
		}
	}else
	{	armMotor.Set(0.0);
	}

	//end rm pull

#ifdef COMPRESSOR_POWER_SAFETY
	if (compressor.Enabled()) {
		CalibratedRobotDrive(0.0, 0.0, false);
		return;
	}
#endif

	point encs = getEncoders();
	SmartDashboard::PutNumber("endLeft", encs.p1);
	SmartDashboard::PutNumber("endRight", encs.p2);
	SmartDashboard::PutNumber("POV", xboxPrimary.GetPOV());

	if (currentTeleopState == TELEOP_CONTROL) {
		if (xboxPrimary.GetBumper(XboxController::kLeftHand)) {//Open
			claw0.Set(true);
			claw1.Set(false);
		} else if (xboxPrimary.GetBumper(XboxController::kRightHand)) {//Close
			claw0.Set(false);
			claw1.Set(true);
		} else if (xboxPrimary.GetStartButton()) {
			claw0.Set(false);
			claw1.Set(false);
		}

		if (xboxPrimary.GetXButton()) {
					currentTeleopState = TELEOP_SEEK;
					MRCCommand com;
					com.setType4(MRCCommand::Commands::MRCC_TRACKIMAGE, 0, 0, 1, 0);
					networkingClient.deliver(com);
					com.setType2(MRCCommand::Commands::MRCC_SETAUTOPILOT, 1);
					networkingClient.deliver(com);
				}

		/*
		if (xboxPrimary.GetAButton()) {
			currentTeleopState = TELEOP_SHAFT_MOVE_ASSIST;
			MRCCommand com;
			com.setType2(MRCCommand::Commands::MRCC_SETAUTOPILOT, 1);
			networkingClient.deliver(com);
			encReset();
		} else if (xboxPrimary.GetXButton()) {
			currentTeleopState = TELEOP_SEEK;
			MRCCommand com;
			com.setType4(MRCCommand::Commands::MRCC_TRACKIMAGE, 0, 3, 0, 0);
			networkingClient.deliver(com);
			com.setType2(MRCCommand::Commands::MRCC_SETAUTOPILOT, 1);
			networkingClient.deliver(com);
		} else if (xboxPrimary.GetYButton()) {
			currentTeleopState = TELEOP_SEEK;
			MRCCommand com;
			com.setType4(MRCCommand::Commands::MRCC_TRACKIMAGE, 0, 0, 1, 0);
			networkingClient.deliver(com);
			com.setType2(MRCCommand::Commands::MRCC_SETAUTOPILOT, 1);
			networkingClient.deliver(com);
		} else if (xboxPrimary.GetBButton()) {
			startTime = clock();
			currentTeleopState = TELEOP_SPIN;
			MRCCommand com;
			com.setType2(MRCCommand::Commands::MRCC_SETAUTOPILOT, 1);
			networkingClient.deliver(com);
		}
		*/
		double leftIn = -xboxPrimary.GetY(
				XboxController::JoystickHand::kLeftHand);
		double rightIn = -xboxPrimary.GetY(
				XboxController::JoystickHand::kRightHand);
		//leftIn += (xboxPrimary.GetRawAxis(3) - xboxPrimary.GetRawAxis(2)) * 0.25;
		//rightIn += (xboxPrimary.GetRawAxis(3) - xboxPrimary.GetRawAxis(2)) * 0.25;
		//Deadzone
		if (enforceDoubleAbs(leftIn) < XBOX_DEADZONE) {
			leftIn = 0.0;
		}
		if (enforceDoubleAbs(rightIn) < XBOX_DEADZONE) {
			rightIn = 0.0;
		}

		//Adjust inputs
		double leftEndEffect = 1.0;
		if (leftIn < 0.0)
		leftEndEffect = -1.0;
		double rightEndEffect = 1.0;
		if (rightIn < 0.0)
		rightEndEffect = -1.0;

		//Remove deadzone
		if (leftIn != 0.0)
		leftIn = leftEndEffect
		* (enforceDoubleAbs(leftIn) - XBOX_DEADZONE)
		/ (1.0 - XBOX_DEADZONE);
		if (rightIn != 0.0)
		rightIn = rightEndEffect
		* (enforceDoubleAbs(rightIn) - XBOX_DEADZONE)
		/ (1.0 - XBOX_DEADZONE);

		double leftDriveValue = 0.0, rightDriveValue = 0.0;

		//Apply curves to input then put into drive train
#ifdef USE_ADJUSTABLE_VAL_POW
		double adjVal = SmartDashboard::GetNumber("DB/Slider 0", VAL_POW);
		if (adjVal <= 0.1)
		adjVal = VAL_POW;

		leftDriveValue = leftEndEffect
		* pow(enforceDoubleAbs(leftIn), adjVal);
		rightDriveValue = rightEndEffect
		* pow(enforceDoubleAbs(rightIn), adjVal);
		SmartDashboard::PutNumber("adjVal", adjVal);
#else
		leftDriveValue = leftEndEffect * pow(leftIn, VAL_POW);
		rightDriveValue = rightEndEffect * pow(rightIn, VAL_POW);
#endif
		//Error catching
		if (enforceDoubleAbs(leftDriveValue) > 1.0)
		leftDriveValue = 0.0;
		if (enforceDoubleAbs(rightDriveValue) > 1.0)
		rightDriveValue = 0.0;

		double speed = sqrt(
				pow(leftDriveValue, 2) + pow(rightDriveValue, 2));

		/* Up? Slow down! */
		if (!limitTop.Get())//Normally 1
			CalibratedRobotDrive(0.5 * leftDriveValue, 0.5 * rightDriveValue, false);
		else
			CalibratedRobotDrive(leftDriveValue, rightDriveValue, false);

#ifdef DEBUG_BUILD
		SmartDashboard::PutNumber("debug_LEFTDRIVESPEED", leftDriveValue);
		SmartDashboard::PutNumber("debug_RIGHTDRIVESPEED", rightDriveValue);
		SmartDashboard::PutNumber("debug_leftIn", leftIn);
		SmartDashboard::PutNumber("debug_rightIn", rightIn);
		SmartDashboard::PutNumber("debug_speed", speed);
#endif
	} else if (currentTeleopState == TELEOP_SEEK) {
		if (xboxPrimary.GetBackButton()) {
			currentTeleopState = TELEOP_CONTROL;
			MRCCommand com;
			com.setType2(MRCCommand::Commands::MRCC_SETAUTOPILOT, 0);
			networkingClient.deliver(com);
		} else {
			if (trackingStatus == 1) {

				//float rotate = (trackingLocX / 640.0-0.5) * 2.0;

				float dd2;
				float dd1;

				float rotate2 = trackingLocX / 640 * 3.1415926 / 2.0;
				dd1 = sin(rotate2) * MAX_SPEED;
				dd2 = cos(rotate2) * MAX_SPEED;

				SmartDashboard::PutNumber("rotate2", rotate2);
				SmartDashboard::PutNumber("dd1", dd1);
				SmartDashboard::PutNumber("dd2", dd2);

				//dd2 = cos(rotate) * MAX_SPEED;
				//dd1 = sin(rotate) * MAX_SPEED;

				if (!blockIR.Get()) {
					CalibratedRobotDrive(dd1, dd2, false);
				} else {
					CalibratedRobotDrive(0.0, 0.0, false);
					claw0.Set(false);
					claw1.Set(true);
					currentTeleopState = TELEOP_CONTROL;
					MRCCommand com;
					com.setType2(MRCCommand::Commands::MRCC_SETAUTOPILOT,
							0);
					networkingClient.deliver(com);
				}
			}
		}
	} else if (currentTeleopState == TELEOP_SPIN) {
		if (xboxPrimary.GetBackButton()) {
			currentTeleopState = TELEOP_CONTROL;
			MRCCommand com;
			com.setType2(MRCCommand::Commands::MRCC_SETAUTOPILOT, 0);
			networkingClient.deliver(com);
		} else {
			CalibratedRobotDrive(0.2, -0.2, false);
		}

	} else if (currentTeleopState == TELEOP_SHAFT_MOVE_ASSIST) {
		point pt = getEncoders();
		if (xboxPrimary.GetBackButton() /*|| (pt.p1 > 2.8 * 270.0)*/) {
			currentTeleopState = TELEOP_CONTROL;
			MRCCommand com;
			com.setType2(MRCCommand::Commands::MRCC_SETAUTOPILOT, 0);
			networkingClient.deliver(com);
		} else {
			CalibratedRobotDrive(0.3, 0.3, false);
			/*
			 if (pt.p1 > pt.p2)
			 CalibratedRobotDrive(0.6, 0.61, false);
			 else if (pt.p2 > pt.p1)
			 CalibratedRobotDrive(0.61, 0.6, false);
			 else
			 CalibratedRobotDrive(0.6, 0.6, false);
			 */
		}

	}
}
