/**
*	内部测试专用
*
*	@copyright	Copyright 2018-2021 SICK GCN. All rights reserved.
*	@author		Vision Lab, SICK GCN
*/

#include "UnitTests.h"

#ifdef UNIT_TEST

#include <opencv2\opencv.hpp>

#pragma region Protected

#define ECode2String(x) SickCam::CAM_STATUS_str(x)
#define CONDITION(x) (x)

Str
MakeUnitTestName(int& cid, int& total, const char * log)
{
	return (std::to_string(cid++) + "/" + std::to_string(total) + " " + std::string(log));
}

Str
MakeUnitTestName_int(const int cid, const int total, const char * log)
{
	return (std::to_string(cid) + "/" + std::to_string(total) + " " + std::string(log));
}

Str
MakeUnitTestName(int& cid, int& total, const std::string log)
{
	return MakeUnitTestName(cid, total, log.c_str());
}

void
print_process_Ecode(const SickCam::CAM_STATUS ec, const Str testName)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN);
	std::cout << "\n\n" << testName.c_str() << ", ec = " << SickCam::CAM_STATUS_str(ec) << " ";

	if (ec == SickCam::CAM_STATUS::All_OK)
	{
		std::cout << ECode2String(ec).c_str() << std::endl;
	}
	else
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);
		std::cout << ECode2String(ec).c_str() << std::endl;
	}
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}

void
print_process_ok(const char * funName, const char * log)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN);
	std::cout << log << ". \n---- <CHECK OK> " << funName << std::endl;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}

void
print_process_ng(const char * funName, const char * log)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);
	std::cout << log << ". \n---- <CHECK NG> " << funName << std::endl;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

}

void
print_process_check(const bool con, const char * funName, const char * log)
{
	std::cout << "\n\n";
	con ? print_process_ok(funName, log) : print_process_ng(funName, log);
}

void
print_process_check(const bool con, const std::string funName, const char * log)
{
	print_process_check(con, funName.c_str(), log);
}

void
print_process_check(const bool con, const char * funName, const std::string log)
{
	print_process_check(con, funName, log.c_str());
}

void
print_process_check(const bool con, const std::string& funName, const std::string& log)
{
	print_process_check(con, funName.c_str(), log.c_str());
}

#pragma endregion

bool protected_unit_test_status_unreachable()
{
	std::cout << "/========= Test : 相机不可达 (Unreachable，IP 不在同一网段) =========\\" << std::endl;
	std::cout << "请将相机 IP 设置为 192.168.1.5，子网掩码为 255.255.255.0 " << std::endl;
	std::cout << "请将PC   IP 设置为 192.168.0.4，子网掩码为 255.255.255.0 或者 255.255.0.0 " << std::endl;
	std::cout << "\\============================================/\n\n" << std::endl;
	std::cout << "\n\n 已经设置完毕请按任意键继续。\n\n " << std::endl;
	system("pause");

	// --- Init Ranger3 environment
	auto pR3S = std::make_shared<Ranger3Shared>("", Ranger3Shared::getPathToRanger3Producer(), false, false);

	// --- Check connected device
	std::string ip = "";
	if (pR3S->isAvaliable())
	{
		auto devices = pR3S->getConDevListIP();
		std::cout << "Found: " << std::endl;
		for (auto sub : devices)
			std::cout << "---- " << sub.first.c_str() << std::endl;
		ip = devices.begin()->first;
	}
	else
	{
		std::cout << "Error: scan camera error. \n" << std::endl;
		return false;
	}

	//  --- Init Ranger3
	auto pCam = std::make_shared<Ranger3>(pR3S, ip, true);

	// --- Check is ready
	auto err = pCam->isReady();

	// 
	{
		auto unitTestName = MakeUnitTestName_int(1, 1, "Test : 相机不可达 (Unreachable，IP 不在同一网段), < EXP : SickCam::ERROR_CAM_IS_UNREACHABLE >");
		print_process_check(
			err == SickCam::CAM_STATUS::ERROR_CAM_IS_UNREACHABLE,
			std::string(
				"\n---- Camera status = " + ECode2String(pCam->getStatus()) +
				"\n---- isReady() return : " + ECode2String(err)),
			unitTestName);
	}

	std::cout << "\n\n" << std::endl;
	std::cout << "---- 请注意，若按照以下的设置，则相机处于同一网段，相机可达。" << std::endl;
	std::cout << "请将相机 IP 设置为 192.168.1.5，子网掩码为 255.255.0.0 " << std::endl;
	std::cout << "请将PC   IP 设置为 192.168.0.4，子网掩码为 255.255.0.0 " << std::endl;
	std::cout << "\n\n" << std::endl;

	return true;
}

bool protected_unit_test_status_occupied()
{
	std::cout << "/========= Test : 相机被占用 (Occupied，已经被其他软件打开) =========\\" << std::endl;
	std::cout << "请将相机 IP 设置为 192.168.1.5，子网掩码为 255.255.255.0 " << std::endl;
	std::cout << "请将PC   IP 设置为 192.168.1.4，子网掩码为 255.255.255.0 " << std::endl;
	std::cout << "请使用 Ranger3Studio 连接相机，并保持连接。" << std::endl;
	std::cout << "\\============================================/\n\n" << std::endl;
	std::cout << "\n\n 已经设置完毕请按任意键继续。\n\n " << std::endl;
	system("pause");

	// --- Init Ranger3 environment
	auto pR3S = std::make_shared<Ranger3Shared>("", Ranger3Shared::getPathToRanger3Producer(), false, false);

	// --- Check connected device
	std::string ip = "";
	if (pR3S->isAvaliable())
	{
		auto devices = pR3S->getConDevListIP();
		std::cout << "Found: " << std::endl;
		for (auto sub : devices)
			std::cout << "---- " << sub.first.c_str() << std::endl;
		ip = devices.begin()->first;
	}
	else
	{
		std::cout << "Error: scan camera error. \n" << std::endl;
		return false;
	}

	//  --- Init Ranger3
	auto pCam = std::make_shared<Ranger3>(pR3S, ip, true);

	// --- Check is ready
	auto err = pCam->isReady();

	// 
	{
		auto unitTestName = MakeUnitTestName_int(1, 1, "Test : 相机被占用 (Occupied，已经被其他软件打开), < EXP : SickCam::ERROR_CAM_IS_OCCUPIED >");
		print_process_check(
			err == SickCam::CAM_STATUS::ERROR_CAM_IS_OCCUPIED,
			std::string(
				"\n---- Camera status = " + ECode2String(pCam->getStatus()) +
				"\n---- isReady() return : " + ECode2String(err)),
			unitTestName);
	}

	return true;
}

bool protected_unit_test_ip_setting()
{
	std::cout << "/========= Test : 相机 IP 设置 =========\\" << std::endl;
	std::cout << "请将相机 IP 设置为 192.168.1.5，子网掩码为 255.255.255.0 " << std::endl;
	std::cout << "请将PC   IP 设置为 192.168.1.4，子网掩码为 255.255.255.0 " << std::endl;
	std::cout << "关闭所有连接到相机的软件。" << std::endl;
	std::cout << "\\============================================/\n\n" << std::endl;
	std::cout << "\n\n 已经设置完毕请按任意键继续。\n\n " << std::endl;
	system("pause");


	// --- Init Ranger3 environment
	auto pR3S = std::make_shared<Ranger3Shared>("",	Ranger3Shared::getPathToRanger3Producer(), false, false);

	// --- Check connected device
	std::string ip = "";
	if (pR3S->isAvaliable())
	{
		auto devices = pR3S->getConDevListIP();
		std::cout << "Found: " << std::endl;
		for (auto sub : devices)
			std::cout << "---- " << sub.first.c_str() << std::endl;
		ip = devices.begin()->first;
	}
	else
	{
		std::cout << "Error: scan camera error. \n" << std::endl;
		return false;
	}

	//  --- Init Ranger3
	auto pCam = std::make_shared<Ranger3>(pR3S, ip, true);

	// --- Check is ready
	auto err = pCam->isReady();
	if (err != SickCam::CAM_STATUS::All_OK)
	{
		if (err == SickCam::CAM_STATUS::ERROR_CAM_IS_OCCUPIED)			std::cout << "Error: camera is occupied! \n" << std::endl;
		if (err == SickCam::CAM_STATUS::ERROR_CAM_IS_UNREACHABLE)		std::cout << "Error: camera is unreachable! \n" << std::endl;
		
		return false;
	}

	// --- Check status
	auto status = pCam->getStatus();
	std::cout << "camera status is " << ECode2String(status) << "!" << std::endl;

	std::string nnnnnnnnnnnnn("\n");
	int cId(1), total(6);

	///////////////////// get ///////////////////////////

	std::cout << nnnnnnnnnnnnn;
	{
		auto unitTestName = MakeUnitTestName(cId, total, "Test: disconnectCamera() + getIp(false) + getIp(true) , < EXP : OK >");

		pCam->disconnectCamera();

		auto strTempIP = pCam->getIp(false);
		auto strPersistentIP = pCam->getIp(true);

		print_process_check(
			!strTempIP.empty() && strPersistentIP.empty(),
			std::string(
				"\n---- Camera status = " + ECode2String(pCam->getStatus()) +
				"\n---- temporary(current) ip = " + strTempIP + 
				"\n---- persistent ip = <" + strPersistentIP + "> (It should be empty! Can only be get when connected!!)"),
			unitTestName);
	}

	std::cout << nnnnnnnnnnnnn;
	{
		auto unitTestName = MakeUnitTestName(cId, total, "Test: connectCamera() + getIp(false) + getIp(true), < EXP : OK >");

		pCam->disconnectCamera();
		pCam->connectCamera();

		auto strTempIP = pCam->getIp(false);
		auto strPersistentIP = pCam->getIp(true);

		print_process_check(
			!strTempIP.empty() && !strPersistentIP.empty(),
			std::string(
				"\n---- Camera status = " + ECode2String(pCam->getStatus()) +
				"\n---- temporary(current) ip = " + strTempIP +
				"\n---- persistent ip = " + strPersistentIP
			),
			unitTestName);
	}

	////////////////////// set //////////////////////////

	std::cout << nnnnnnnnnnnnn;
	{
		auto unitTestName = MakeUnitTestName(cId, total, "Test : disconnectCamera() + setIp(fasle) : Force IP, < EXP : OK >");
		std::string targetIP("192.168.1.15");
		pCam->disconnectCamera();		auto strTempIP = pCam->getIp(false);		auto strPersistentIP = pCam->getIp(true);

		auto op_err = pCam->setIp(targetIP, false);
		print_process_check(
			CONDITION(
				pCam->getIp(false) == targetIP &&
				pCam->getIp(true) == strPersistentIP &&
				op_err == SickCam::CAM_STATUS::All_OK
			),
			std::string(
				"\n---- Camera status = " + ECode2String(pCam->getStatus()) +
				"\n---- temporary(current) ip = " + strTempIP +
				"\n---- persistent ip = " + strPersistentIP +
				"\n---- Do Force IP = " + targetIP + ", return " + ECode2String(op_err) +
				"\n---- temporary(current) ip = " + pCam->getIp(false) +
				"\n---- persistent ip = " + pCam->getIp(true)
			),
			unitTestName);
	}
	std::cout << nnnnnnnnnnnnn;
	{
		auto unitTestName = MakeUnitTestName(cId, total, "Test : disconnectCamera() +  setIp(true), < EXP NG >");
		std::string targetIP("192.168.1.25");
		pCam->disconnectCamera();		auto strTempIP = pCam->getIp(false);		auto strPersistentIP = pCam->getIp(true);

		auto op_err = pCam->setIp(targetIP, true);
		print_process_check(
			CONDITION(
				pCam->getIp(false) == strTempIP &&
				pCam->getIp(true) == strPersistentIP &&
				pCam->getIp(true) != targetIP &&
				op_err == SickCam::CAM_STATUS::ERROR_CAM_IS_DISCONNECTED
			), 
			std::string(
				"\n---- Camera status = " + ECode2String(pCam->getStatus()) +
				"\n---- temporary(current) ip = " + strTempIP +
				"\n---- persistent ip = " + strPersistentIP +
				"\n---- Do Set Persistent IP = " + targetIP + ", return " + ECode2String(op_err) +
				"\n---- temporary(current) ip = " + pCam->getIp(false) +
				"\n---- persistent ip = " + pCam->getIp(true)
			),
			unitTestName);

	}
	std::cout << nnnnnnnnnnnnn;
	{
		auto unitTestName = MakeUnitTestName(cId, total, "Test : connectCamera() + setIp(fasle) : Force IP, < EXP NG >");
		std::string targetIP("192.168.1.35");

		pCam->disconnectCamera();
		pCam->connectCamera();
		
		auto strTempIP = pCam->getIp(false);		auto strPersistentIP = pCam->getIp(true);

		auto op_err = pCam->setIp(targetIP, false);
		print_process_check(
			CONDITION(
				pCam->getIp(false) == strTempIP &&
				pCam->getIp(true) == strPersistentIP &&
				pCam->getIp(false) != targetIP &&
				op_err == SickCam::CAM_STATUS::ERROR_CAM_IS_CONNECTED
			),
			std::string(
				"\n---- Camera status = " + ECode2String(pCam->getStatus()) +
				"\n---- temporary(current) ip = " + strTempIP +
				"\n---- persistent ip = " + strPersistentIP +
				"\n---- Do Force IP = " + targetIP + ", return " + ECode2String(op_err) +
				"\n---- temporary(current) ip = " + pCam->getIp(false) +
				"\n---- persistent ip = " + pCam->getIp(true)
			),
			unitTestName);
	}
	std::cout << nnnnnnnnnnnnn;
	{
		auto unitTestName = MakeUnitTestName(cId, total, "Test : connectCamera() +  setIp(true), < EXP OK >");
		auto targetIP("192.168.1.45");

		pCam->disconnectCamera();
		pCam->connectCamera();
		pCam->setIp("192.168.1.75", true);

		auto strTempIP = pCam->getIp(false);
		auto strPersistentIP = pCam->getIp(true);

		auto op_err = pCam->setIp(targetIP, true);
		print_process_check(
			//targetIP == pCam->getIp(true),
			CONDITION(
				pCam->getIp(false) == targetIP &&
				pCam->getIp(true) != strPersistentIP &&
				pCam->getIp(true) == targetIP &&
				op_err == SickCam::CAM_STATUS::All_OK
			),
			std::string(
				"\n---- Camera status = " + ECode2String(pCam->getStatus()) +
				"\n---- temporary(current) ip = " + strTempIP +
				"\n---- persistent ip = " + strPersistentIP +
				"\n---- Do Set Persistent IP = " + targetIP + ", return " + ECode2String(op_err) +
				"\n---- temporary(current) ip = " + pCam->getIp(false) +
				"\n---- persistent ip = " + pCam->getIp(true)
			),
			unitTestName);
	}

	return true;
}

bool protected_unit_test_ip_many_times()
{
	std::cout << "/========= Test : 连续设置多次，相机固定 IP  =========\\" << std::endl;
	std::cout << "请确保相机可以被正常连接。 " << std::endl;
	std::cout << "\\============================================/\n\n" << std::endl;
	std::cout << "\n\n 已经设置完毕请按任意键继续。\n\n " << std::endl;
	system("pause");

	// --- Init Ranger3 environment
	auto pR3S = std::make_shared<Ranger3Shared>("", Ranger3Shared::getPathToRanger3Producer(), false, false);

	// --- Check connected device
	std::string ip = "";
	if (pR3S->isAvaliable())
	{
		auto devices = pR3S->getConDevListIP();
		std::cout << "Found: " << std::endl;
		for (auto sub : devices)
			std::cout << "---- " << sub.first.c_str() << std::endl;
		ip = devices.begin()->first;
	}
	else
	{
		std::cout << "Error: scan camera error. \n" << std::endl;
		return false;
	}

	//  --- Init Ranger3
	auto pCam = std::make_shared<Ranger3>(pR3S, ip, true);

	// --- Check is ready
	auto err = pCam->isReady();
	if (err != SickCam::CAM_STATUS::All_OK)
	{
		if (err == SickCam::CAM_STATUS::ERROR_CAM_IS_OCCUPIED)			std::cout << "Error: camera is occupied! \n" << std::endl;
		if (err == SickCam::CAM_STATUS::ERROR_CAM_IS_UNREACHABLE)		std::cout << "Error: camera is unreachable! \n" << std::endl;

		return false;
	}
	pCam->connectCamera();

	int cId(1), total(7);
	{
		std::string IP("192.168.1.61");
		auto unitTestName = MakeUnitTestName(cId, total, IP + ", < EXP : OK >");
		err = pCam->setIp(IP, true);

		print_process_check(
			CONDITION(
				err == SickCam::CAM_STATUS::All_OK &&
				pCam->getIp(true) == IP
			) ,
			std::string(
				"\n---- Camera status = " + ECode2String(pCam->getStatus()) +
				"\n---- temporary(current) ip = " + pCam->getIp(false) +
				"\n---- persistent ip = " + pCam->getIp(true) + 
				"\n---- ErrorCode: " + SickCam::CAM_STATUS_str(err)
			),
			unitTestName);
	}

	{
		std::string IP("192.168.1.62");
		auto unitTestName = MakeUnitTestName(cId, total, IP + ", < EXP : OK >");
		err = pCam->setIp(IP, true);

		print_process_check(
			CONDITION(
				err == SickCam::CAM_STATUS::All_OK &&
				pCam->getIp(true) == IP
			),
			std::string(
				"\n---- Camera status = " + ECode2String(pCam->getStatus()) +
				"\n---- temporary(current) ip = " + pCam->getIp(false) +
				"\n---- persistent ip = " + pCam->getIp(true) +
				"\n---- ErrorCode: " + SickCam::CAM_STATUS_str(err)
			),
			unitTestName);
	}

	{
		std::string IP("192.168.1.63");
		auto unitTestName = MakeUnitTestName(cId, total, IP + ", < EXP : OK >");
		err = pCam->setIp(IP, true);

		print_process_check(
			CONDITION(
				err == SickCam::CAM_STATUS::All_OK &&
				pCam->getIp(true) == IP
			),
			std::string(
				"\n---- Camera status = " + ECode2String(pCam->getStatus()) +
				"\n---- temporary(current) ip = " + pCam->getIp(false) +
				"\n---- persistent ip = " + pCam->getIp(true) +
				"\n---- ErrorCode: " + SickCam::CAM_STATUS_str(err)
			),
			unitTestName);
	}

	{
		std::string IP("192.168.1.64");
		auto unitTestName = MakeUnitTestName(cId, total, IP + ", < EXP : OK >");
		err = pCam->setIp(IP, true);

		print_process_check(
			CONDITION(
				err == SickCam::CAM_STATUS::All_OK &&
				pCam->getIp(true) == IP
			),
			std::string(
				"\n---- Camera status = " + ECode2String(pCam->getStatus()) +
				"\n---- temporary(current) ip = " + pCam->getIp(false) +
				"\n---- persistent ip = " + pCam->getIp(true) +
				"\n---- ErrorCode: " + SickCam::CAM_STATUS_str(err)
			),
			unitTestName);
	} 
	
	{
		std::string IP("192.168.1.65");
		auto unitTestName = MakeUnitTestName(cId, total, IP + ", < EXP : OK >");
		err = pCam->setIp(IP, true);

		print_process_check(
			CONDITION(
				err == SickCam::CAM_STATUS::All_OK &&
				pCam->getIp(true) == IP
			),
			std::string(
				"\n---- Camera status = " + ECode2String(pCam->getStatus()) +
				"\n---- temporary(current) ip = " + pCam->getIp(false) +
				"\n---- persistent ip = " + pCam->getIp(true) +
				"\n---- ErrorCode: " + SickCam::CAM_STATUS_str(err)
			),
			unitTestName);
	}

	{
		std::string IP("192.168.1.66");
		auto unitTestName = MakeUnitTestName(cId, total, IP + ", < EXP : OK >");
		err = pCam->setIp(IP, true);

		print_process_check(
			CONDITION(
				err == SickCam::CAM_STATUS::All_OK &&
				pCam->getIp(true) == IP
			),
			std::string(
				"\n---- Camera status = " + ECode2String(pCam->getStatus()) +
				"\n---- temporary(current) ip = " + pCam->getIp(false) +
				"\n---- persistent ip = " + pCam->getIp(true) +
				"\n---- ErrorCode: " + SickCam::CAM_STATUS_str(err)
			),
			unitTestName);
	}

	{
		std::string IP("192.168.1.67");
		auto unitTestName = MakeUnitTestName(cId, total, IP + ", < EXP : OK >");
		err = pCam->setIp(IP, true);

		print_process_check(
			CONDITION(
				err == SickCam::CAM_STATUS::All_OK &&
				pCam->getIp(true) == IP
			),
			std::string(
				"\n---- Camera status = " + ECode2String(pCam->getStatus()) +
				"\n---- temporary(current) ip = " + pCam->getIp(false) +
				"\n---- persistent ip = " + pCam->getIp(true) +
				"\n---- ErrorCode: " + SickCam::CAM_STATUS_str(err)
			),
			unitTestName);
	}

	pCam->disconnectCamera();

	return true;
}

#pragma region HighSpeedTest
/////////////////////////////////////////////////////////////////////////////////
uint64_t g_linerate = 20000; // max 30000
uint64_t g_grabTimeS = 60;
uint64_t profileN(g_linerate * g_grabTimeS), scanHeight(10), timestampBeg(0), timestampEnd(0);

int g_buffercount = round(300.0*1024*1024 / (2560*4* scanHeight)); // keep 200 MB

long long g_beg(0), g_end(0); // microseconds
uint64_t g_numTotal(profileN / scanHeight), g_count(0), g_cid(0)/*current image id*/;
double g_rate_gt(double(g_linerate) / scanHeight);
std::ofstream g_fileID, g_fileTS;


void SICK_CALLBACK
on_callback_grabbing_rate(SickCam::ImgT * pImg, void * any)
{
	auto time = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	g_cid = pImg->get_ID();
	printf("\rcallback time= %lld, id = %lld", time, g_cid);

	if (1 <= pImg->get_ID() && pImg->get_ID() <= g_numTotal)
	{
		g_fileID << pImg->get_ID() << ",";
		g_fileTS << pImg->get_ChunkInfo()[0].timestamp << ",";
		++g_count;

		if (pImg->get_ID() == 1){
			g_beg = time;
			timestampBeg = pImg->get_ChunkInfo()[0].timestamp;
			std::cout << "**** start " << pImg->get_ID() << ", " << time << " ms." << std::endl;
		}else if (pImg->get_ID() == g_numTotal){
			g_end = time;
			timestampEnd = pImg->get_ChunkInfo()[scanHeight - 1].timestamp;
			std::cout << "**** end " << pImg->get_ID() << ", " << time << " ms." << std::endl;
		}


		//std::ofstream file;
		//file.open("D:\\ts\\test" + std::to_string(pImg->get_ID()) + ".txt");
		//for(auto sub : pImg->get_ChunkInfo())
		//	file << sub.timestamp << ",";
	}
}
bool protected_unit_test_grabbing_rate()
{
	std::cout << "/========= Test : Grabbing rate  =========\\" << std::endl;
	std::cout << "            Please ensure the camera ip setting is correct! " << std::endl;
	std::cout << "\\============================================/\n\n" << std::endl;
	std::cout << "\n\n Press any key to continue the following test. \n\n " << std::endl;
	///system("pause");

	// --- Init Ranger3 environment
	auto pR3S = std::make_shared<Ranger3Shared>("", Ranger3Shared::getPathToRanger3Producer(), true, false);

	// --- Check connected device
	std::string ip = "";
	if (pR3S->isAvaliable()) {
		auto devices = pR3S->getConDevListIP();
		std::cout << "Found: " << std::endl;
		for (auto sub : devices)
			std::cout << "---- " << sub.first.c_str() << std::endl;
		ip = devices.begin()->first;
	}
	else { std::cout << "Error: scan camera error. \n" << std::endl;	return false; }

	//  --- Init Ranger3
	auto pCam = std::make_shared<Ranger3>(pR3S, ip, true);

	// --- Check is ready
	auto err = pCam->isReady();
	if (err != SickCam::CAM_STATUS::All_OK)
	{
		if (err == SickCam::CAM_STATUS::ERROR_CAM_IS_OCCUPIED)			std::cout << "Error: camera is occupied! \n" << std::endl;
		if (err == SickCam::CAM_STATUS::ERROR_CAM_IS_UNREACHABLE)		std::cout << "Error: camera is unreachable! \n" << std::endl;

		return false;
	}
	pCam->connectCamera();

	pCam->setBufferCount(g_buffercount);
	pCam->setParameterValue("DeviceScanType", "Linescan3D");
	pCam->setParameterValue("ExposureTime_RegionSelector_Region1", "1");
	pCam->setParameterValue("ComponentEnable_ComponentSelector_Reflectance_RegionSelector_Scan3dExtraction1", "0");
	pCam->setParameterValue("Width_RegionSelector_Region1", "2560");
	pCam->setParameterValue("Height_RegionSelector_Region1", "24");//400
	pCam->setParameterValue("TriggerMode_TriggerSelector_LineStart", "Off");
	pCam->setParameterValue("TriggerMode_TriggerSelector_FrameStart", "Off");
	pCam->setParameterValue("AcquisitionLineRate", std::to_string(g_linerate));
	pCam->setParameterValue("Height_RegionSelector_Scan3dExtraction1", std::to_string(scanHeight));
	pCam->setParameterValue("Scan3dOutputMode_Scan3dExtractionSelector_Scan3dExtraction1", "UncalibratedC"); // RectifiedC UncalibratedC
	pCam->setParameterValue("Scan3dRectificationWidth_Scan3dExtractionSelector_Scan3dExtraction1", "2560");
	pCam->setParameterValue("ChunkModeActive", "1");


	std::cout << "\n\n Check parameters and press any key to continue if they are as expected.  \n\n " << std::endl;
	std::cout <<
		"\nbuffercount                             (n   )= " << g_buffercount <<
		"\nDeviceScanType                          (Linescan3D)= " << pCam->getParameterValue("DeviceScanType") <<
		"\nExposureTime_RegionSelector_Region1     (1   )= " << pCam->getParameterValue("ExposureTime_RegionSelector_Region1") <<
		"\nComponentEnable_ComponentSelector_Reflectance_RegionSelector_Scan3dExtraction1 = (1)" << pCam->getParameterValue("ComponentEnable_ComponentSelector_Reflectance_RegionSelector_Scan3dExtraction1") <<
		"\nWidth_RegionSelector_Region1            (2560)= " << pCam->getParameterValue("Width_RegionSelector_Region1") <<
		"\nHeight_RegionSelector_Region1           (400 )= " << pCam->getParameterValue("Height_RegionSelector_Region1") <<
		"\nTriggerMode_TriggerSelector_LineStart   (Off )= " << pCam->getParameterValue("TriggerMode_TriggerSelector_LineStart") <<
		"\nTriggerMode_TriggerSelector_FrameStart  (Off )= " << pCam->getParameterValue("TriggerMode_TriggerSelector_FrameStart") <<
		"\nAcquisitionLineRate                     (XXXX)= " << pCam->getParameterValue("AcquisitionLineRate") <<
		"\nHeight_RegionSelector_Scan3dExtraction1 (XXXX)= " << pCam->getParameterValue("Height_RegionSelector_Scan3dExtraction1") <<
		"\nScan3dOutputMode_Scan3dExtractionSelector_Scan3dExtraction1 (RectifiedC)= " << pCam->getParameterValue("Scan3dOutputMode_Scan3dExtractionSelector_Scan3dExtraction1") <<
		"\nChunkModeActive                         (1   )= " << pCam->getParameterValue("ChunkModeActive") <<
		"\n\n\n";
	///system("pause");
	g_fileID.open("D:\\testID.txt");
	g_fileTS.open("D:\\testTS.txt");

	//  --- Start Ranger3
	{	auto err = pCam->startCamera(on_callback_grabbing_rate);
	if (err != SickCam::CAM_STATUS::All_OK) {
		std::cout << SickCam::CAM_STATUS_str(err) << std::endl;
		return false;
	}
	}

	//for (int i=0; i<35; ++i)
	//{
	//	__sleep1MS(1000);
	//}
	printf("Estimate total time: %lld second\n", g_grabTimeS);
	while (g_end == 0)
	{
		auto rate = int(1.0*g_count / g_numTotal * 100);
		printf("\r%d%%(Time left: %f second), g_count=%llu, c_id=%llu(gt=%llu), lost=%llu ", rate, g_grabTimeS - rate/100.0*g_grabTimeS, g_count, g_cid, g_numTotal, g_cid - g_count);
		if (g_cid > g_numTotal)
			break;
	}
	printf("pCam->stopCamera();");
	pCam->stopCamera();

	double rate = (g_count-1)*1E6 / (g_end - g_beg); // 实际测的时间是从1得到之后，到n得到之后，所以其实算是 n-1 的时间
	print_process_check(
		abs(rate - g_rate_gt) < 1.0,
		std::string(
			"\n---- Grabbing rate ：" + std::to_string(rate) + " Frames/Second， GT = " + std::to_string(g_rate_gt)
		),
		"Test: speed");

	print_process_check(
		CONDITION(g_count == g_numTotal
		),
		std::string(
			"\n---- Received Frames ：" + std::to_string(g_count) + " Frames（GT=" + std::to_string(g_numTotal) + "）" +
			"\n---- Received Profiles：" + std::to_string(g_count * scanHeight) + " Profiles（GT=" + std::to_string(profileN) + "）"
		),
		"test: frame lost, counting by Image ID");

	auto lineRate = (g_count * scanHeight - 1) * 1E9 / (timestampEnd - timestampBeg); // timestamp unit is 1 ns
	print_process_check(
		CONDITION(abs(lineRate - g_linerate) < 0.1
		),
		std::string(
			"\n---- Real LineRate：" + std::to_string(lineRate) + " Hz（GT=" + std::to_string(g_linerate) + "）"
			"\n---- Real timestampBeg=" + std::to_string(timestampBeg) + " ns, Real timestampEnd=" + std::to_string(timestampEnd) + " ns"
			"\n---- Real timestamp Interval =" + std::to_string(1.0 / lineRate ) + " second"
			"\n---- GT   timestamp Interval =" + std::to_string(1.0 / g_linerate) + " second"
		),
		"test: frame lost, counting by timestamp");

	pCam->disconnectCamera();

	{
		auto rate = int(1.0*g_count / g_numTotal * 100);
		printf("\n\n\n\n%d%%(FINAL : Time left: %f second), g_count=%llu, c_id=%llu(gt=%llu), lost=%llu ", rate, g_grabTimeS - rate / 100.0*g_grabTimeS, g_count, g_cid, g_numTotal, g_cid - g_count);
	}

	// lost 1606
	cout << "\n\n\n======== TEST: 02 ============" << endl;

	return true;
}

/////////////////////////////////////////////////////////////////////////////////
#pragma endregion

bool protected_unit_test_chunk_data()
{
	cv::Mat mark, mark1, mark2;

	SickCam::ImgT img;
	img.loadFromIconFile("H:\\delete\\chunk_data_verify\\r3s5459", 1.0);
	if (img.has(SickCam::DataNames::MAR))
		mark = cv::Mat(img.get_rows(), CHUNK_DATA_WIDTH, CV_32SC1, (uint8_t*)img.getData(SickCam::DataNames::MAR)); // 编码器信息

	SickCam::ImgT img1;
	img1.loadFromIconFile("H:\\delete\\chunk_data_verify\\r3s", 1.0);
	if (img1.has(SickCam::DataNames::MAR))
		mark1 = cv::Mat(img1.get_rows(), CHUNK_DATA_WIDTH, CV_32SC1, (uint8_t*)img1.getData(SickCam::DataNames::MAR)); // 编码器信息

	SickCam::ImgT img2;
	img2.loadFromIconFile("H:\\delete\\chunk_data_verify\\ss", 1.0);
	if (img2.has(SickCam::DataNames::MAR))
		mark2 = cv::Mat(img2.get_rows(), CHUNK_DATA_WIDTH, CV_32SC1, (uint8_t*)img2.getData(SickCam::DataNames::MAR)); // 编码器信息

	return true;
}

void SICK_CALLBACK
on_callback_in_device_calibration(SickCam::ImgT * pImg, void * any)
{
	std::cout << pImg->collectInfo() << std::endl;
	bool is_grabbed = pImg->has(SickCam::DN::RAN_CAL);
	print_process_check(
		is_grabbed,
		std::string("\n---- get calibration data. " ) + (is_grabbed ? "Yes" : "No"),
		"in-device calibration");
}
bool protected_unit_test_in_PC_calibration()
{
#ifndef DISABLE_CAL_IN_PC
	std::cout << "/========= Test : in-pc 标定 =========\\" << std::endl;
	std::cout << "\n\n 已经设置完毕请按任意键继续。\n\n " << std::endl;
	system("pause");

	std::string unitTestName("in-pc calibration");

	// --- Init Ranger3 environment
	auto pR3S = std::make_shared<Ranger3Shared>("", Ranger3Shared::getPathToRanger3Producer(), false, false);

	// --- Check connected device
	std::string ip = "";
	if (pR3S->isAvaliable()){
		auto devices = pR3S->getConDevListIP();
		std::cout << "Found: " << std::endl;
		for (auto sub : devices)
			std::cout << "---- " << sub.first.c_str() << std::endl;
		ip = devices.begin()->first;
	}else{
		std::cout << "Error: scan camera error. \n" << std::endl;
		return false;
	}

	//  --- Init Ranger3
	auto pCam = std::make_shared<Ranger3>(pR3S, ip, true);
	auto ec = pCam->setCalibration_inPC("D:\\UserFile.json", true, 2560, 4);
	if (SickCam::CAM_STATUS::All_OK == ec)
	{
		getchar();
		return true;
	}
	else
	{
		print_process_check(
			false,
			std::string("\n---- setCalibration_inDevice faile, error code = " + ECode2String(ec)),
			unitTestName);
	}
#endif
	return false;
}

bool protected_unit_test_in_device_calibration()
{
	std::cout << "/========= Test : in-device 标定 =========\\" << std::endl;
	std::cout << "\n\n 已经设置完毕请按任意键继续。\n\n " << std::endl;
	system("pause");

	std::string unitTestName("in-device calibration");

	// --- Init Ranger3 environment
	auto pR3S = std::make_shared<Ranger3Shared>("", Ranger3Shared::getPathToRanger3Producer(), false, false);

	// --- Check connected device
	std::string ip = "";
	if (pR3S->isAvaliable()) {
		auto devices = pR3S->getConDevListIP();
		std::cout << "Found: " << std::endl;
		for (auto sub : devices)
			std::cout << "---- " << sub.first.c_str() << std::endl;
		ip = devices.begin()->first;
	}
	else {
		std::cout << "Error: scan camera error. \n" << std::endl;
		return false;
	}

	//  --- Init Ranger3
	auto pCam = std::make_shared<Ranger3>(pR3S, ip, true);
	auto ec = pCam->setCalibration_inDevice();
	if (SickCam::CAM_STATUS::All_OK == ec)
	{
		getchar();
		return true;
	}
	else
	{
		print_process_check(
			false,
			std::string("\n---- setCalibration_inDevice faile, error code = " + ECode2String(ec)),
			unitTestName);
	}
	return false;
}

#endif // UNIT_TEST