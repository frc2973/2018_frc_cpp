#include "../../headers/robot.h"

/* Subpart in autonomous */

void Robot::rmReset() {
	/* This section not used. */

	rm.lift = RM::LIFT::LIFT_DOWN;
	rm.liftStatus = RM::LIFTSTATUS::LIFTSTATUS_MOVING;
	rm.arm = RM::ARM::ARM_UP;//Default UP position, honestly autonomous shouldn't modify this
	rm.armStatus = RM::ARMSTATUS::ARMSTATUS_MOVING;
}

void Robot::rmUpdate() {
	/* This section not used. */

	//SmartDashboard::PutNumber("armEnc", encArm.Get());

	/* Lift RM */
	//DISABLED FOR RIGHT SWITCH AUTONOMOUS///////////////////////////////////////////////////////
	//////////@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	/*
	switch (rm.lift) {
	case RM::LIFT::LIFT_UP: {
		if (limitTop.Get()) {
			rm.liftStatus = RM::LIFTSTATUS::LIFTSTATUS_MOVING;
			liftMotor.Set(-0.65);
		} else {
			rm.liftStatus = RM::LIFTSTATUS::LIFTSTATUS_REACHED;
			liftMotor.Set(0.0);
		}
	}break;
	case RM::LIFT::LIFT_DOWN: {
		if (limitBottom.Get()) {
			rm.liftStatus = RM::LIFTSTATUS::LIFTSTATUS_MOVING;
			liftMotor.Set(0.45);
		} else {
			rm.liftStatus = RM::LIFTSTATUS::LIFTSTATUS_REACHED;
			liftMotor.Set(0.0);
		}
	}break;
	}*/

	/* Arm RM */
	/*
	switch (rm.arm) {
	case RM::ARM::ARM_UP: {
		if (limitArmTop.Get()) {
			rm.armStatus = RM::ARMSTATUS::ARMSTATUS_MOVING;
			armMotor.Set(-0.80);
		} else {
			rm.armStatus = RM::ARMSTATUS::ARMSTATUS_REACHED;
			armMotor.Set(0.0);
		}
	}break;
	case RM::ARM::ARM_DOWN: {
		rm.armStatus = RM::ARMSTATUS::ARMSTATUS_MOVING;
		armMotor.Set(0.0);
	}break;
	}*/
	//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

/*
	//Primary controls
	if (xboxPrimary.GetPOV() == 180) {

		if (limitBottom.Get())
			liftMotor.Set(0.45); //down
		else
			liftMotor.Set(0.0);
	} else if (xboxPrimary.GetPOV() == 0) {

		if (limitTop.Get())
			liftMotor.Set(-0.65); //Up is negative
		else
			liftMotor.Set(0.0); //Set it to not move when no data, could cause issues if not done
	} else {
		liftMotor.Set(0.0);
	}
	*/

}
