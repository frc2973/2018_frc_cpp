#include "../../headers/robot.h"



void Robot::stepAutoStage() {
	point encs = getEncoders();
	SmartDashboard::PutNumber("endLeftAutoActionEnd", encs.p1);
	SmartDashboard::PutNumber("endRightAutoActionEnd", encs.p2);
	autoAction = AUTOACTION_STOP;
	encReset();
	switch (autoPlan) {
	case AUTOPLAN_BOTHSWITCH: {
		switch (autoStage) {
		case -1:
			autoAction = AUTOACTION_LIFT0d5S;
			waitUntil = clock() + CLOCKS_PER_SEC * 50.0 / 1000.0;
			break;
		case 0:
			//Rotate towards target
			switch (gSM[0])
			{
			case 'L':
				waitUntil = clock() + CLOCKS_PER_SEC * 300.0 / 1000.0;
				autoAction = AUTOACTION_RUNRIGHTDRIVE;
				break;
			case 'R':
				waitUntil = clock() + CLOCKS_PER_SEC * 180.0 / 1000.0;
				autoAction = AUTOACTION_RUNLEFTDRIVE;
				break;
			}
			break;
		case 1:
			//Drive
			timeApproachTrack = time(NULL) + 4;
			autoAction = AUTOACTION_TIMEAPPROACH;
			break;
		case 2://Reached destination
			claw0.Set(true);
			claw1.Set(false);
			autoAction=AUTOACTION_STOP;
			break;

		default:
			autoAction=AUTOACTION_STOP;
			break;
		}
	}break;
	case AUTOPLAN_FALLBACKFORWARDNOSHAFT: {
		switch (autoStage){
		case -1:
			timeApproachTrack = time(NULL) + 4;
			autoAction = AUTOACTION_TIMEAPPROACH;
			break;
		case 0:
			autoAction = AUTOACTION_STOP;
			break;

		}
	}break;
	case AUTOPLAN_FALLBACKLEFT: {

		switch (autoStage) {
		case -1:
			autoAction = AUTOACTION_LIFT0d5S;
			waitUntil = clock() + CLOCKS_PER_SEC * 50.0 / 1000.0;
			break;
		case 0:
			timeApproachTrack = time(NULL) + 4;
			autoAction = AUTOACTION_TIMEAPPROACH;

			SmartDashboard::PutNumber("HERE3",clock());
			break;
		case 1:
			if (gSM[0] == 'L') {
				claw0.Set(true);
				claw1.Set(false);
			}
			else {
				autoAction = AUTOACTION_STOP;
			}
			break;
		default:

			autoAction = AUTOACTION_STOP;
			break;
		}

	}break;
	case AUTOPLAN_FALLBACKRIGHT: {

		switch (autoStage) {
		case -1:
			autoAction = AUTOACTION_LIFT0d5S;
			waitUntil = clock() + CLOCKS_PER_SEC * 50.0 / 1000.0;
			break;
		case 0:
			timeApproachTrack = time(NULL) + 4;
			autoAction = AUTOACTION_TIMEAPPROACH;
			SmartDashboard::PutNumber("HERE3",clock());
			break;
		case 1:
			if (gSM[0] == 'R') {
				claw0.Set(true);
				claw1.Set(false);
			}
			else {
				autoAction = AUTOACTION_STOP;
			}
			break;
		default:

			autoAction = AUTOACTION_STOP;
			break;
		}

	}break;
	case AUTOPLAN_FALLBACKLEFTSHAFTASSIST: {

			switch (autoStage) {
			case -1:
				autoAction = AUTOACTION_LIFT0d5S;
				waitUntil = clock() + CLOCKS_PER_SEC * 50.0 / 1000.0;
				break;
			case 0:
				timeApproachTrack = time(NULL) + 3;
				autoAction = AUTOACTION_SHAFTASSISTTIMEAPPROACH;

				SmartDashboard::PutNumber("HERE3",clock());
				break;
			case 1:
				if (gSM[0] == 'L') {
					claw0.Set(true);
					claw1.Set(false);
				}
				else {
					autoAction = AUTOACTION_STOP;
				}
				break;
			default:

				autoAction = AUTOACTION_STOP;
				break;
			}

		}break;
		case AUTOPLAN_FALLBACKRIGHTSHAFTASSIST: {

			switch (autoStage) {
			case -1:
				autoAction = AUTOACTION_LIFT0d5S;
				waitUntil = clock() + CLOCKS_PER_SEC * 50.0 / 1000.0;
				break;
			case 0:
				timeApproachTrack = time(NULL) + 3;
				autoAction = AUTOACTION_SHAFTASSISTTIMEAPPROACH;
				SmartDashboard::PutNumber("HERE3",clock());
				break;
			case 1:
				if (gSM[0] == 'R') {
					claw0.Set(true);
					claw1.Set(false);
				}
				else {
					autoAction = AUTOACTION_STOP;
				}
				break;
			default:

				autoAction = AUTOACTION_STOP;
				break;
			}

		}break;

	case AUTOPLAN_RIGHTSWITCH: {
		switch (autoStage) {
		/*case -1: {
			autoAction = AUTOACTION_WAIT;
			waitUntil = clock() + CLOCKS_PER_SEC * 3100.0 / 1000.0;
			MRCCommand::MRCCommandBodyBase::ADATA aData;
			aData.T2.i1 = 1;
			networkingClient.deliver(2, MRCCommand::Commands::MRCC_SOUND, aData);
		}break;*/
		case -1:
			autoAction = AUTOACTION_SHAFTASSISTDISTANCE;
			shaftAssistDistance = 5500 * 3 / 4;
			break;
		case 0:
			autoAction = AUTOACTION_WAIT;
			waitUntil = clock() + CLOCKS_PER_SEC * 100 / 1000;
			break;
		case 1:
			autoAction = AUTOACTION_SHAFTASSISTLEFT90;
			break;
		case 2:
			autoAction = AUTOACTION_SHAFTOVERCOMPENSATELEFT;
			break;
		case 3:
			//Drive up0d5sfrombase
			autoAction = AUTOACTION_LIFT0d5S;
			waitUntil = clock() + CLOCKS_PER_SEC * 50.0 / 1000.0;
			break;
		case 4:
			//Drive until lir
			autoAction = AUTOACTION_APPROACHUNTILLEFTANTENNA;
			break;
		case 5:
			//Open claws
			claw0.Set(true);
			claw1.Set(false);
			break;
		default:
			autoAction = AUTOACTION_STOP;
			break;
		}
	}
		break;
	case AUTOPLAN_DEV: {
		switch (autoStage) {
		case -1:
			autoAction = AUTOACTION_SHAFTASSISTDISTANCE;
			shaftAssistDistance = 5900;
			break;
		default:

			autoAction = AUTOACTION_STOP;
			break;
			/*
			 case -1:
			 autoAction = AUTOACTION_DEV_ENDLESSLEFT;
			 break;
			 case 2:
			 autoAction=AUTOACTION_STOP;
			 break;*/
		}
	}
		break;
	case AUTOPLAN_CROSSSWITCHTOLEFT: {
		switch (autoStage) {
		case -1:
			autoAction = AUTOACTION_SHAFTASSISTDISTANCE;
			shaftAssistDistance = 5900;
			break;
		case 0:
			autoAction = AUTOACTION_WAIT;
			waitUntil = clock() + CLOCKS_PER_SEC * 100 / 1000;
		case 1:
			autoAction = AUTOACTION_SHAFTASSISTLEFT90;
			break;
		case 2:
			autoAction = AUTOACTION_SHAFTOVERCOMPENSATELEFT;
			//waitUntil = clock() + CLOCKS_PER_SEC * 50 / 1000;
			break;
		case 3:
			autoAction = AUTOACTION_SHAFTASSISTDISTANCE;
			shaftAssistDistance = 6500;
			break;
		case 4:
			autoAction = AUTOACTION_SHAFTASSISTLEFT90;
			break;
		case 5:
			autoAction = AUTOACTION_SHAFTASSISTDISTANCE;
			shaftAssistDistance = 1500;
			break;
		case 6:
			autoAction = AUTOACTION_SHAFTASSISTLEFT90;
			break;
		case 7:
			//Drive up0d5sfrombase
			autoAction = AUTOACTION_LIFT0d5S;
			waitUntil = clock() + CLOCKS_PER_SEC * 50 / 1000;
			break;
		case 8:
			autoAction = AUTOACTION_APPROACHUNTILLEFTANTENNA;
			break;
		case 9:
			//Open claws
			claw0.Set(true);
			claw1.Set(false);
			break;
		default:
			autoAction = AUTOACTION_STOP;
			break;
		}
	}
		break;
	case AUTOPLAN_CORNERTRACKPLACERIGHT: {
		switch (autoStage) {
		case -1:
			//Drive up0d5sfrombase
			autoAction = AUTOACTION_LIFT0d5S;
			waitUntil = clock() + CLOCKS_PER_SEC * 50.0 / 1000.0;
			break;
		case 0:
			timeApproachTrack = time(NULL) + 5;
			autoAction = AUTOACTION_TIMEAPPROACHTRACK;
			break;
		case 1:
			if (gSM[0] == 'R') {
				claw0.Set(true);
				claw1.Set(false);
			}
			else {
				autoAction = AUTOACTION_STOP;
			}
			break;
		default:
			autoAction = AUTOACTION_STOP;
			break;
		}
	}
		break;
	case AUTOPLAN_CROSSAUTONOMOUSLINE: {
		switch (autoStage) {
		case -1:
			timeApproachTrack = time(NULL) + 5;
			autoAction = AUTOACTION_TIMEAPPROACHTRACK;
			break;
		default:
			autoAction = AUTOACTION_STOP;
			break;
		}
	}
		break;
	case AUTOPLAN_CORNERTRACKPLACELEFT: {
		switch (autoStage) {
		case -1:
			//Drive up0d5sfrombase
			autoAction = AUTOACTION_LIFT0d5S;
			waitUntil = clock() + CLOCKS_PER_SEC * 50.0 / 1000.0;
			break;
		case 0:
			timeApproachTrack = time(NULL) + 5;
			autoAction = AUTOACTION_TIMEAPPROACHTRACK;
			break;
		case 1:
			if (gSM[0] == 'L') {
				claw0.Set(true);
				claw1.Set(false);
			}
			else {
				autoAction = AUTOACTION_STOP;
			}
			break;
		default:
			autoAction = AUTOACTION_STOP;
			break;
		}
	}
		break;
	case AUTOPLAN_NONE: {
		autoAction = AUTOACTION_STOP;
	}
		break;
	default: {
		autoAction = AUTOACTION_STOP;
	}
		break;
	}
	SmartDashboard::PutNumber("Current Stage", autoStage);
	autoStage++;
}

void Robot::AutonomousInit() {
	status = 3;
	postStatus();

	encReset();

	gSM = frc::DriverStation::GetInstance().GetGameSpecificMessage();
	SmartDashboard::PutString("gSM", gSM);

	autoPlan = AUTOPLAN_NONE;

	if (hasAutoData && gSM.length() == 3) {
		int i1 = autoData.i1, i2 = autoData.i2, i3 = autoData.i3, i4 =
				autoData.i4;

		//i1 is unused
		//i2 is unused
		//i3 is auto plan
		//i4 is the compressor

		switch (i3) {
		case 0: // NONE
			autoPlan = AUTOPLAN_NONE;
			break;
		case 1: // RIGHT SWITCH TRACK PLACE
			autoPlan = AUTOPLAN_CORNERTRACKPLACERIGHT;
			break;
		case 2: // AUTOPLAN_CORNERTRACKPLACELEFT
			autoPlan = AUTOPLAN_CORNERTRACKPLACELEFT;
			break;
			//DO NOT UPDATE ANY PREVIOUS NUMBERS, ONLY ADD. BACKWARDS COMPATIBILITY
		default: // NONE
			autoPlan = AUTOPLAN_NONE;
		}

	} else if (SmartDashboard::GetNumber("DB/Slider 0",0.0) != 0.0) {
		switch (int(SmartDashboard::GetNumber("DB/Slider 0",0.0))) {
		case 0:
			autoPlan = AUTOPLAN_NONE;
			break;
		case 1:
			autoPlan = AUTOPLAN_FALLBACKLEFT;
			break;
		case 2:
			autoPlan = AUTOPLAN_FALLBACKRIGHT;
			break;
		case 3:
			autoPlan = AUTOPLAN_FALLBACKFORWARDNOSHAFT;
			break;
		case 4:
			autoPlan = AUTOPLAN_FALLBACKLEFTSHAFTASSIST;
			break;
		case 5:
			autoPlan = AUTOPLAN_FALLBACKRIGHTSHAFTASSIST;
			break;
		default:
			autoPlan = AUTOPLAN_NONE;

		}
	}

	//Override
	autoPlan = AUTOPLAN_FALLBACKFORWARDNOSHAFT;////////

	autoStage = -1;
	stepAutoStage();

	rmReset();

	winch.Set(0.0);
}

void Robot::AutonomousPeriodic() {

	point encs = getEncoders();
	SmartDashboard::PutNumber("endLeft", encs.p1);
	SmartDashboard::PutNumber("endRight", encs.p2);
	SmartDashboard::PutNumber("h2", autoAction);
	switch (autoAction) {
	case AUTOACTION_WAIT: {

		CalibratedRobotDrive(0.0, 0.0, false);
		if (clock() > waitUntil)
			stepAutoStage();
	}
		break;
	case AUTOACTION_APPROACHUNTILLEFTANTENNA: {

		CalibratedRobotDrive(0.3, 0.3, false);

		if (leftAntenna.Get()) {
			stepAutoStage();
		}
	}
		break;
	case AUTOACTION_DEV_ENDLESSLEFT: {
		if (leftAntenna.Get()) {
			CalibratedRobotDrive(0.0, -0.4, false);
		} else {

			CalibratedRobotDrive(0.2, 0.4, false);
		}
	}
		break;
	case AUTOACTION_SHAFTASSISTDISTANCE: {

		point pt = getEncoders();
		double l = 0.0, r = 0.0;
		if (pt.p1 < shaftAssistDistance)
			l = 0.4;
		else
			l = -0.4;
		if (pt.p2 < shaftAssistDistance)
			r = 0.4;
		else
			r = -0.4;
		bool lN = false, rN = false;
		if (abs(shaftAssistDistance - pt.p1) < 50)
			l *= 0.5;
		if (abs(shaftAssistDistance - pt.p2) < 50)
			r *= 0.5;
		if (abs(shaftAssistDistance - pt.p1) < 10)
			lN = true;
		if (abs(shaftAssistDistance - pt.p2) < 10)
			rN = true;

		//Active Calibration
		//Ahhhh this fixes everything
		if (pt.p1 > pt.p2)
			r *= 1.1;
		else if (pt.p2 > pt.p1)
			l *= 1.1;

		if (lN)
			l = 0.0;
		if (rN)
			r = 0.0;
		if (leftAntenna.Get())
			r = -0.2;
		CalibratedRobotDrive(l, r, false);
		if (lN && rN)
			stepAutoStage();

	}
		break;
	case AUTOACTION_SHAFTASSISTRIGHT90: {

		point pt = getEncoders();
		double lT = 535, rT = -559;
		double l = 0.0, r = 0.0;
		if (pt.p1 < lT)
			l = 0.3;
		else
			l = -0.3;
		if (pt.p2 > rT)
			r = -0.3;
		else
			r = 0.3;
		bool lN = false, rN = false;
		if (abs(lT - pt.p1) < 50)
			l *= 0.5;
		if (abs(rT - pt.p2) < 50)
			r *= 0.5;
		if (abs(lT - pt.p1) < 5)
			lN = true;
		if (abs(rT - pt.p2) < 5)
			rN = true;
		if (lN)
			l = 0.0;
		if (rN)
			r = 0.0;
		CalibratedRobotDrive(l, r, false);
		SmartDashboard::PutNumber("h", l);
		if (lN && rN)		//try making it reverse if not close enough
			stepAutoStage();

	}
		break;
	case AUTOACTION_SHAFTOVERCOMPENSATELEFT: {

		point pt = getEncoders();
		CalibratedRobotDrive(0.0, 0.2, false);
		if (pt.p2 > 20)		//try making it reverse if not close enough
			stepAutoStage();

	}
		break;
	case AUTOACTION_SHAFTASSISTLEFT90: {

		point pt = getEncoders();
		double lT = -FEET(18.5/12.0), rT = FEET(18.5/12.0);
		double l = 0.0, r = 0.0;
		if (pt.p1 > lT)
			l = -0.3;
		else
			l = 0.3;
		if (pt.p2 < rT)
			r = 0.3;
		else
			r = -0.3;
		bool lN = false, rN = false;
		if (abs(lT - pt.p1) < 100)
			l *= 0.8;
		if (abs(rT - pt.p2) < 100)
			r *= 0.8;
		if (abs(lT - pt.p1) < 5)
			lN = true;
		if (abs(rT - pt.p2) < 5)
			rN = true;
		if (lN)
			l = 0.0;
		if (rN)
			r = 0.0;
		CalibratedRobotDrive(l, r, false);
		if (lN && rN)		//try making it reverse if not close enough
			stepAutoStage();

	}
		break;
	case AUTOACTION_APPROACHTRACK: {
		if (trackingStatus == 1) {
			float rotate = trackingLocX / 640.0 * 3.1415926 / 2.0;
			float dd2 = cos(rotate) * MAX_SPEED;
			float dd1 = sin(rotate) * MAX_SPEED;

			if (dd1 > MAX_SPEED)
				dd1 = MAX_SPEED;
			if (dd2 > MAX_SPEED)
				dd2 = MAX_SPEED;

			if (!leftAntenna.Get())
				CalibratedRobotDrive(dd1, dd2, false);
			else {
				stepAutoStage();
			}
		}
	}
		break;
	case AUTOACTION_APPROACHTRACKGRAB: {
		if (trackingStatus == 1) {
			float rotate = trackingLocX / 640.0 * 3.1415926 / 2.0;
			float dd2 = cos(rotate) * MAX_SPEED;
			float dd1 = sin(rotate) * MAX_SPEED;

			if (dd1 > MAX_SPEED)
				dd1 = MAX_SPEED;
			if (dd2 > MAX_SPEED)
				dd2 = MAX_SPEED;

			if (!frontIR.Get())
				CalibratedRobotDrive(dd1, dd2 + .10, false);
			else {
				claw0.Set(true);
				claw1.Set(false);
				stepAutoStage();
			}
		}
	}
		break;
	case AUTOACTION_TIMEAPPROACHTRACK: {
		if (trackingStatus == 1) {
			float rotate = trackingLocX / 640.0 * 3.1415926 / 2.0;
			float dd2 = cos(rotate) * MAX_SPEED;
			float dd1 = sin(rotate) * MAX_SPEED;

			if (dd1 > MAX_SPEED)
				dd1 = MAX_SPEED;
			if (dd2 > MAX_SPEED)
				dd2 = MAX_SPEED;

			if (!frontIR.Get() && time(NULL) < timeApproachTrack)
				CalibratedRobotDrive(dd1, dd2, false);
			else {
				stepAutoStage();
			}
		}
	}
		break;
	case AUTOACTION_TIMEAPPROACH: {
		if (/*!leftAntenna.Get() && */time(NULL) < timeApproachTrack){
			CalibratedRobotDrive(0.31, 0.3, false);
		}
		else {
			stepAutoStage();
			SmartDashboard::PutNumber("FAIL0",0);
			SmartDashboard::PutNumber("FAIL1",0);
			if (leftAntenna.Get())
				SmartDashboard::PutNumber("FAIL0",clock());
			if (!time(NULL) < timeApproachTrack)
				SmartDashboard::PutNumber("FAIL1",clock());
		}
	}break;
	case AUTOACTION_SHAFTASSISTTIMEAPPROACH: {
		if (!leftAntenna.Get() && time(NULL) < timeApproachTrack){

			point pt = getEncoders();
			double l = 0.0, r = 0.0;
			l = 0.31;
			r = 0.3;

			//Active Calibration
			//Ahhhh this fixes everything
			if (pt.p1 > pt.p2)
				r *= 1.1;
			else if (pt.p2 > pt.p1)
				l *= 1.1;

			CalibratedRobotDrive(l, r, false);
		}
		else {
			stepAutoStage();
		}
	}break;
	case AUTOACTION_STOP: {
		CalibratedRobotDrive(0.0, 0.0, false);
	}
		break;
	case AUTOACTION_LIFT0d5S: {
		//Lift
		liftMotor.Set(-0.80);
		CalibratedRobotDrive(0.0, 0.0, false);
		if (clock() > waitUntil || !limitTop.Get()) {
			liftMotor.Set(0.0);
			stepAutoStage();
		}
	}break;
	case AUTOACTION_PLACECUBESWITCH: {
		CalibratedRobotDrive(-0.2, -0.2, false);
	}
		break;
	case AUTOACTION_RUNRIGHTDRIVE: {
		CalibratedRobotDrive(0.0, 0.3, false);

		if (clock() > waitUntil)
			stepAutoStage();
	}break;
	case AUTOACTION_RUNLEFTDRIVE: {
		CalibratedRobotDrive(0.3, 0.0, false);

		if (clock() > waitUntil)
			stepAutoStage();
	}break;
	}

	/* RM (/RUS) */
	//rmUpdate();
	armMotor.Set(0.0);
}
