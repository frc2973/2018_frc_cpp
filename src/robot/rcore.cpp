/* For robot core functionality that isn't updated much */

#include "../../headers/robot.h"

void Robot::postStatus() {
	MRCCommand::MRCCommandBodyBase::ADATA aData;
	aData.T2.i1 = status;
	networkingClient.deliver(2, MRCCommand::Commands::MRCC_ROBOT_STATUS, aData);
}

bool Robot::handleMessage(NetworkingClient* networkingClient,
		MRCCommand& recCom) {
	switch (recCom.mrch.command) {
	case MRCCommand::MRCC_WELCOME: {
		postStatus(); //Default function comes before so the server will accept it

		MRCCommand::MRCCommandBodyBase::ADATA aData;
		aData.T2.i1 = 0;
		networkingClient->deliver(2, MRCCommand::Commands::MRCC_SETAUTODATA,
				aData);
		hasAutoData = false;
	}
		break;
	case MRCCommand::MRCC_ROBOT_STATUS: {
		if (recCom.mrcb.aData.T2.i1 == -1) {
			postStatus();
		} else {
			//Future usage: @@@@@@@@@@@@@@@@@
			//status = read_msg_.mrcb.aData.T2.i1;
			postStatus();
		}
	}
		break;
	case MRCCommand::MRCC_TRACKINGSTATUS: {
		trackingStatus = recCom.mrcb.aData.T2.i1;
	}
		break;
	case MRCCommand::MRCC_TRACKINGAABB: {
		trackingStatus = 1;
		trackingLocX = recCom.mrcb.aData.T3.f1 + recCom.mrcb.aData.T3.f3 / 2;
		trackingLocY = recCom.mrcb.aData.T3.f2 + recCom.mrcb.aData.T3.f4 / 2;
		trackingWidth = recCom.mrcb.aData.T3.f3;
	}
		break;
	case MRCCommand::MRCC_BATTERY: {
		MRCCommand::MRCCommandBodyBase::ADATA aData;
		aData.T1.f1 = frc::DriverStation::GetInstance().GetBatteryVoltage();
		networkingClient->deliver(1, MRCCommand::Commands::MRCC_BATTERY, aData);
	}
		break;
	case MRCCommand::MRCC_AUTODATA: {
		MRCCommand::MRCCommandBodyBase::ADATA aData;
		aData.T2.i1 = 1;
		networkingClient->deliver(2, MRCCommand::Commands::MRCC_SETAUTODATA,
				aData);
		autoData = recCom.mrcb.aData.T4;
		hasAutoData = true;
		SmartDashboard::PutNumber("AutoData i1", recCom.mrcb.aData.T4.i1);
		SmartDashboard::PutNumber("AutoData i2", recCom.mrcb.aData.T4.i2);
		SmartDashboard::PutNumber("AutoData i3", recCom.mrcb.aData.T4.i3);
		SmartDashboard::PutNumber("AutoData i4", recCom.mrcb.aData.T4.i4);

		//TEMPORARY@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
		if (autoData.i4 == 1)
			compressor.SetClosedLoopControl(true);
		if (autoData.i4 == 0)
			compressor.SetClosedLoopControl(false);

	}
		break;
	}
	return true;
}

void Robot::batLoop() {
	while (!terminateThreadsLocal) {
		usleep(1000000);
		if (networkingClient.getNetworkStatus() == NS_CONNECTED) {
			MRCCommand::MRCCommandBodyBase::ADATA aData;
			aData.T3.f1 = frc::DriverStation::GetInstance().GetBatteryVoltage();
			networkingClient.deliver(3, MRCCommand::Commands::MRCC_BATTERY,
					aData);
		}
	}
}

void Robot::batLoop_Static(Robot* _this) {
	_this->batLoop();
}
