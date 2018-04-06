/*
 * client.h
 *
 *  Created on: Jan 10, 2018
 *      Author: egame
 */

#ifndef HEADERS_NETWORKING_CLIENT_H_
#define HEADERS_NETWORKING_CLIENT_H_

#include "message.h"

#include <thread>
#include <string>
#include <cstdlib>
#include <cmath>
#include <ctime>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>


class NetworkingClient;
class NetworkingEventHandler;

class NetworkingEventHandler {
public:
	virtual bool handleMessage(NetworkingClient* networkingClient,
			MRCCommand& recCom) {
		return true;
	}
};


enum NetworkStatus {
	NS_CONNECTED,
	NS_NOTCONNECTED,
	NS_ATTEMPTINGTOCONNECT,
	NS_NOINIT,
	NS_INIT
};

//Don't inherit from this, inherit from the NetworkingEventHandler and set it.
class NetworkingClient {
private:
	NetworkStatus networkStatus = NS_NOINIT;
	int networkingClientID = 0;
	int untrustworthy_ROBOTID = -1; //Holds what the server calls this NETWORKING CLIENT.
	NetworkingEventHandler* linkedNetworkingEventHandler = NULL;
	bool networkingIsInit = false;
	bool networkingIsConnected = false;
	bool terminateNetwork = false;
	std::string networkStatusToString(NetworkStatus _networkStatus) {
		switch (_networkStatus) {
		case NS_CONNECTED:
			return "NS_CONNECTED";
			break;
		case NS_NOTCONNECTED:
			return "NS_NOTCONNECTED";
			break;
		case NS_ATTEMPTINGTOCONNECT:
			return "NS_ATTEMPTINGTOCONNECT";
			break;
		case NS_NOINIT:
			return "NS_NOINIT";
			break;
		case NS_INIT:
			return "NS_INIT";
			break;
		default:
			return "Unknown (ENUM" + std::to_string(_networkStatus) + ")";
		};
	}
	std::thread* networkingThread;
	std::string ip;
	std::string port;
	int socketID = -1;
	void setNetworkStatus(NetworkStatus status) {
		networkStatus = status;
		SmartDashboard::PutString("NETSTS " + ip + ":" + port,
				networkStatusToString(networkStatus) + " (" +std::to_string(clock())+ ")");
	}
	void putNetworkError(std::string error) {
		SmartDashboard::PutString("NETERR " + ip + ":" + port,
				error + " (" +std::to_string(clock())+ ")");
	}
	bool defMsgHandle(MRCCommand& recCom) {
		switch (recCom.mrch.command) {
		case MRCCommand::MRCC_WELCOME: {
			untrustworthy_ROBOTID = recCom.mrch.to;
			//Redo this style
			MRCCommand welcomeMessage;
			welcomeMessage.mrch.type = (unsigned char) 2;
			welcomeMessage.mrch.from = recCom.mrch.to;
			welcomeMessage.mrch.to = 0;
			welcomeMessage.mrch.command = MRCCommand::MRCC_WELCOME;
			welcomeMessage.mrcb.type = 2;
			welcomeMessage.mrcb.aData.T2.i1 = recCom.mrcb.aData.T2.i1;
			welcomeMessage.prepHeader();
			if (!deliver(welcomeMessage))
				return false;
		}
			break;
		}
		return true;
	}
	bool handleMessage(int socketID, MRCCommand& recCom) {
		if (!defMsgHandle(recCom))
			return false;
		if (linkedNetworkingEventHandler) {
			if (!linkedNetworkingEventHandler->handleMessage(this, recCom))
				return false;
		}
		return true;
	}
	void networkMessageLoop() {
		MRCCommand recCom;
		int n;
		while (!terminateNetwork) {
			n = read(socketID, &recCom.mrch,
					sizeof(MRCCommand::MRCCommandHeader));
			if (n < 0) {
				putNetworkError("n<0 at read mrch");
				return;
			}
			n = read(socketID, recCom.mrcb.getBody(),
					recCom.mrch.bodySizeInBytes);
			if (n < 0) {
				putNetworkError("n<0 at read mrcb");
				return;
			}
			if (!handleMessage(socketID, recCom))
				return;
		}
	}
	bool networkingConnect() {
		if (socketID != -1) {
			close(socketID);
			socketID = -1;
		}

		struct sockaddr_in serv_addr;
		struct hostent* server;
		int portno = stoi(port);
		socketID = socket(AF_INET, SOCK_STREAM, 0);

		server = gethostbyname(ip.c_str());

		bzero((char*) &serv_addr, sizeof(serv_addr));
		serv_addr.sin_family = AF_INET;
		bcopy((char*) server->h_addr,
		(char*)&serv_addr.sin_addr.s_addr,
		server->h_length);
		serv_addr.sin_port = htons(portno);
		if (connect(socketID, (struct sockaddr*) &serv_addr, sizeof(serv_addr))
				< 0) {
			putNetworkError("connect()<0");
			if (socketID != -1) {
				close(socketID);
				socketID = -1;
			}
			return false;
		} else {
			return true;
		}
	}
	void networkingThreadMainObject() {
		while (!terminateNetwork) {
			setNetworkStatus(NS_ATTEMPTINGTOCONNECT);
			if (networkingConnect()) { //Connected
				networkingIsConnected = true;
				setNetworkStatus(NS_CONNECTED);
				networkMessageLoop();
				networkingIsConnected = false;
				if (socketID != -1) {
					close(socketID);
					socketID = -1;
				}
			}

			setNetworkStatus(NS_NOTCONNECTED);
			if (terminateNetwork)
				break;
			usleep(50000);
		}
		//Shutdown is handled in the ThreadMain, not the ThreadMainObject. This allows for ThreadMainObject to return on error.
	}
	static void networkingThreadMain(NetworkingClient* networkingBase) {
		networkingBase->networkingThreadMainObject();
		networkingBase->networkingShutdown();
	}
public:
	NetworkStatus getNetworkStatus() {
		return	networkStatus;
	}
	bool deliver(MRCCommand& sendCom) { //THIS FUNCTION NOW OWNS THE DATA

		int n = 0;
		if (networkingIsConnected)
			n = write(socketID, sendCom.data(), sendCom.totalDataSize());
		if (n < 0) {
			putNetworkError("n<0 at deliver");
			return false;
		}
		return true;
	}
	bool deliver(unsigned char type, MRCCommand::Commands command,
			MRCCommand::MRCCommandBodyBase::ADATA aData) {

		MRCCommand quickDeliver;
		quickDeliver.mrch.type = type;
		quickDeliver.mrch.from = untrustworthy_ROBOTID;
		quickDeliver.mrch.to = 0;
		quickDeliver.mrch.command = command;
		quickDeliver.mrcb.type = type;
		quickDeliver.mrcb.aData = aData;
		quickDeliver.prepHeader();
		return deliver(quickDeliver);
	}
	int getNetworkingClientID() {
		return networkingClientID;
	}
	void setMessageCallback(NetworkingEventHandler* _nEH) {
		linkedNetworkingEventHandler = _nEH;
	}
	//Ip. Port. Some random ID (this doesn't really get used)... . And then the callback, which can be set later through setMessageCallback if you pass NULL during the init.
	bool networkingInit(std::string _ip, std::string _port,
			int _networkingClientID, NetworkingEventHandler* _nEH) {
		if (networkingIsInit)
			if (!networkingShutdown())
				return false;
		networkingIsInit = true;
		networkingIsConnected = false;
		terminateNetwork = false;
		networkingClientID = _networkingClientID;
		ip = _ip;
		port = _port;
		socketID = -1;
		setMessageCallback(_nEH);
		untrustworthy_ROBOTID = -1;
		setNetworkStatus(NS_INIT);
		networkingThread = new std::thread(&networkingThreadMain, this);
		//networkingThread->detach();//DONT DETACH!!!@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@MAY CAUSE PROBLEMS AFTER REMOVAL
		return true;
	}
	bool networkingShutdown() {
		if (networkingIsInit) {
			terminateNetwork = true;
			networkingThread->join();
			terminateNetwork = false;
			delete networkingThread;

		}
		if (socketID != -1) {
			close(socketID);
			socketID = -1;
		}
		networkingIsInit = false;
		networkingClientID = 0;
		networkingIsConnected = false;
		untrustworthy_ROBOTID = -1;
		setNetworkStatus(NS_NOINIT);
		linkedNetworkingEventHandler = NULL;
		return true;
	}
	~NetworkingClient() {
		networkingShutdown();
	}
};

#endif /* HEADERS_NETWORKING_CLIENT_H_ */
