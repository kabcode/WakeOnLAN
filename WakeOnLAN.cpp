// std includes
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>

// windows socket includes
#include <WinSock2.h>



void PrintUsage(char* msg)
{
	std::cout << "USAGE: " << msg << " MACAddresses.txt" << std::endl;
	std::cout << "For further information call: " << msg << " --help" << std::endl;
}

void PrintHelp()
{
	std::cout << "-- Wake On LAN --" << std::endl;
	std::cout << "For usage please create text file names 'MACAddresses.txt' with on MAC address per line." << std::endl;
	std::cout << "For every MAC address a broadcast Wake-On-LAN call will be send." << std::endl;
	std::cout << "A MAC address has the form 'XX:XX:XX:XX:XX:XX'." << std::endl;
	std::cout << "-- Basic call --" << std::endl;
	std::cout << "USAGE: WakeOnLAN.exe MACAddresses.txt" << std::endl;
	std::cout << "-- Optional parameters --" << std::endl;
	std::cout << "-h or --help" << std::endl;
}

std::vector<std::string> ReadAddresses(std::string AddressFileName)
{
	std::vector<std::string> MACAddresses = std::vector<std::string>();
	std::ifstream FID(AddressFileName);
	if (FID.is_open)
	{
		std::string Address;
		while (std::getline(FID,Address))
		{
			Address.erase(std::remove_if(Address.begin(), Address.end(), isspace), Address.end());
			if (Address.size() > 0)
				MACAddresses.push_back(Address);
		}
		FID.close;
	}

	return MACAddresses;
}

bool SendWakeOnLAN(const std::string MACAddress, unsigned PortAddress, unsigned long Broadcast)
{

	// Build message
	// 6x 0xFF followed 16x MACAddress
	std::string Message(6, 0xFF);
	for (auto i = 0; i < 16; ++i)
	{
		Message += MACAddress;
	}

	// Create socket
}

int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		PrintUsage(argv[0]);
		return 1;
	}

	for (auto i = 1; i < argc; ++i)
	{
		if (argv[i] == "--help" || argv[i] == "-h")
			PrintHelp();
	}


	std::string MACFileName("MACAddresses.txt");
	std::vector<std::string> MACAddressList = ReadAddresses(MACFileName);

	// Socket variables
	WSADATA WsaData;
	SOCKET SendingSocket = INVALID_SOCKET;

	// Initialize WinSock
	if (WSAStartup(MAKEWORD(2, 2), &WsaData) != 0)
	{
		std::cout << "WSA startup failed with error:" << std::endl;
		std::cout << WSAGetLastError() << std::endl;
	}
	else
	{

	}

}