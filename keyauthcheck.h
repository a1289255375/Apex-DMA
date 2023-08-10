#pragma once
#include "Authkey/auth.hpp"
#include "skStr.h"
using namespace KeyAuth;
#include "includes.hpp"
std::string name = "sunflower_ap_ow"; // application name. right above the blurred text aka the secret on the licenses tab among other tabs
std::string ownerid = "Tjzus4TXGU"; // ownerid, found in account settings. click your profile picture on top right of dashboard and then account settings.
std::string secret = "9b4aab24be88bb6a274d10163c0a844f77542753a36e024fb3693f8002f0ea2b"; // app secret, the blurred text on licenses tab and other tabs
std::string version = "1.0"; // leave alone unless you've changed version on website
std::string url = "https://keyauth.win/api/1.2/"; // change if you're self-hosting

api KeyAuthApp(name, ownerid, secret, version, url);
void KeyauthCheck()
{
	try {
		SetConsoleTitleA(skCrypt("Loader"));
		std::cout << skCrypt("\n\n Connecting..");
		KeyAuthApp.init();
		if (!KeyAuthApp.data.success)
		{
			std::cout << skCrypt("\n Status: ") << KeyAuthApp.data.message;
			Sleep(1500);
			exit(0);
		}

		//if (KeyAuthApp.checkblack()) {
		//	abort();
		//}

		std::cout << skCrypt("\n\n App data:");
		std::cout << skCrypt("\n Number of users: ") << KeyAuthApp.data.numUsers;
		std::cout << skCrypt("\n Number of online users: ") << KeyAuthApp.data.numOnlineUsers;
		std::cout << skCrypt("\n Number of keys: ") << KeyAuthApp.data.numKeys;
		std::cout << skCrypt("\n Application Version: ") << KeyAuthApp.data.version;
		std::cout << skCrypt("\n Customer panel link: ") << KeyAuthApp.data.customerPanelLink;
		//std::cout << skCrypt("\n Checking session validation status (remove this if causing your loader to be slow)");
		//KeyAuthApp.check();
		//std::cout << skCrypt("\n Current Session Validation Status: ") << KeyAuthApp.data.message;

		std::cout << skCrypt("\n\n [1] Login\n [2] Register\n [3] Upgrade\n [4] License key only\n\n Choose option: ");

		int option;
		std::string username;
		std::string password;
		std::string key;

		std::cin >> option;
		switch (option)
		{
		case 1:
			std::cout << skCrypt("\n\n Enter username: ");
			std::cin >> username;
			std::cout << skCrypt("\n Enter password: ");
			std::cin >> password;
			KeyAuthApp.login(username, password);
			break;
		case 2:
			std::cout << skCrypt("\n\n Enter username: ");
			std::cin >> username;
			std::cout << skCrypt("\n Enter password: ");
			std::cin >> password;
			std::cout << skCrypt("\n Enter license: ");
			std::cin >> key; 
			KeyAuthApp.regstr(username, password, key);
			break;
		case 3:
			std::cout << skCrypt("\n\n Enter username: ");
			std::cin >> username;
			std::cout << skCrypt("\n Enter license: ");
			std::cin >> key;
			KeyAuthApp.upgrade(username, key);
			break;
		case 4:
			std::cout << skCrypt("\n Enter license: ");
			std::cin >> key;
			KeyAuthApp.license(key);
			break;
		default:
			std::cout << skCrypt("\n\n Status: Failure: Invalid Selection");
			Sleep(3000);
			exit(0);
		}
		if (!KeyAuthApp.data.success)
		{
			std::cout << skCrypt("\n Status: ") << KeyAuthApp.data.message;
			Sleep(1500);
			exit(0);
		}
	}
	catch (std::exception& e) {
		std::cout << "Caught exception: " << e.what() << std::endl;
	}

	
}