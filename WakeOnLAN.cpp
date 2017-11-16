// std includes
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <stdio.h>

// windows socket includes
#include <WinSock2.h>
#include <Windows.h>
#pragma comment(lib, "Ws2_32.lib")

void PrintUsage(char* msg)
{
	std::cout << "USAGE: " << msg << " MACAddresses.txt" << std::endl;
	std::cout << "For further information call: " << msg << " --help" << std::endl;
}

void PrintHelp()
{
	std::cout << std::endl;
	std::cout << "-- Wake On LAN --" << std::endl;
	std::cout << "For usage please create text file names 'MACAddresses.txt' with on MAC address per line.";
	std::cout << " For every MAC address a broadcast Wake-On-LAN call will be send.";
	std::cout << " A MAC address has the form 'XX:XX:XX:XX:XX:XX'." << std::endl;
	std::cout << " Every line beginning with '#' will be ignored." << std::endl;

	std::cout << std::endl << "-- Basic call --" << std::endl;
	std::cout << "WakeOnLAN.exe [options]" << std::endl;
	
	std::cout << std::endl << "-- Options --" << std::endl;
	std::cout << "-h or --help \t Shows this information"<< std::endl;
	std::cout << "-f or --file [filename] \t Set other MAC address file (default: MACAddresses.txt)" << std::endl;
	std::cout << "-p or --port [portnumber]\t Set other port file (default: 9)" << std::endl;
	std::cout << std::endl;
}

std::vector<std::string> ReadAddresses(std::string AddressFileName)
{
	std::vector<std::string> MACAddresses = std::vector<std::string>();
	std::ifstream FID(AddressFileName);
	if (FID.is_open())
	{
		std::string Address;
		while (std::getline(FID,Address))
		{
			Address.erase(std::remove_if(Address.begin(), Address.end(), isspace), Address.end());
			if (Address[0] == '#') continue;
			if (Address.size() > 0)
				MACAddresses.push_back(Address);
			std::cout << Address << std::endl;
		}
		FID.close();
	}
	else
	{
		std::cout << "File not found!" << std::endl;
	}

	if (MACAddresses.size() == 0)
	{
		std::cout << "No valid addresses in file!" << std::endl;
	}

	return MACAddresses;
}

void RemoveCharsFromString(std::string &Str, char* CharsToRemove) {
	for (unsigned int i = 0; i < strlen(CharsToRemove); ++i) {
		Str.erase(std::remove(Str.begin(), Str.end(), CharsToRemove[i]), Str.end());
	}
}

bool SendWakeOnLAN(std::string MACAddress, unsigned PortAddress, unsigned long BroadcastAddress)
{

	// Build message = magic packet
	// 6x 0xFF followed 16x MACAddress
	unsigned char Message[102];
	for (auto i = 0; i < 6; ++i)
		Message[i] = 0xFF;

	unsigned char MAC[6];
	std::string StrMAC = MACAddress;
	RemoveCharsFromString(StrMAC, ":-");
	std::cout << StrMAC << std::endl;


	for (auto i = 0; i < 6; ++i)
	{
		MAC[i] = static_cast<unsigned char>(std::stoul(StrMAC.substr(i * 2, 2), nullptr, 16));
	}
	
	for (auto i = 1; i <= 16; ++i)
	{
		memcpy(&Message[i * 6], &MAC, 6 * sizeof(unsigned char));
	}
	
	// Create socket
	// Socket variables
	WSADATA WSAData;
	SOCKET SendingSocket = INVALID_SOCKET;
	struct sockaddr_in LANDestination {};

	// Initialize WinSock
	if (WSAStartup(MAKEWORD(2, 2), &WSAData) != NO_ERROR)
	{
		std::cout << "WSA startup failed with error:" << std::endl;
		std::cout << WSAGetLastError() << std::endl;
		return false;
	}
	else
	{
		// Initialize socket with protocol properties (internet protocol, datagram-based protocol (size limited to 512bytes))
		SendingSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

		if (SendingSocket == INVALID_SOCKET)
		{
			std::cout << "Socket is not initialized:" << std::endl;
			std::cout << WSAGetLastError() << std::endl;
			return false;
		}

		// Set socket options (broadcast)
		const bool optval = TRUE;
		if (setsockopt(SendingSocket, SOL_SOCKET, SO_BROADCAST, (char*)&optval, sizeof(optval)) != NO_ERROR)
		{
			std::cout << "Socket startup failed with error:" << std::endl;
			std::cout << WSAGetLastError() << std::endl;
			return false;
		}

		LANDestination.sin_family = AF_INET;
		LANDestination.sin_port = htons(PortAddress);
		LANDestination.sin_addr.s_addr = BroadcastAddress;

		// send Wake On LAN packet
		if (sendto(SendingSocket, (char*) Message, 102, 0, reinterpret_cast<sockaddr*>(&LANDestination), sizeof(LANDestination)) == ERROR)
		{
			std::cout << "Sending magic packet fails with error:" << std::endl;
			std::cout << WSAGetLastError() << std::endl;
			return false;
		}

	}

	return true;
}

int main(int argc, char* argv[])
{

	
	// Default MAC address file name
	std::string MACFileName{ "MACAddresses.txt" };
	// Default port on receiving device
	unsigned short PortAddress{ 9 };
	unsigned long BroadcastAddress{ 0xFFFFFFFF };

	// Handle input arguments
	for (auto i = 1; i < argc; ++i)
	{
		
		if ( !strcmp(argv[i],"--help") || !strcmp(argv[i],"-h"))
		{
			PrintHelp();
			return 1;
		}
		if (!strcmp(argv[i], "--file") || !strcmp(argv[i], "-f"))
		{
			MACFileName = argv[i + 1];
			std::cout << "MACFileName changed to " << argv[i+1] << std::endl;
			++i;
		}
		if (!strcmp(argv[i], "--port") || !strcmp(argv[i], "-p"))
		{
			PortAddress = static_cast<unsigned short>(std::stoi(argv[i + 1]));
			std::cout << "Wake On LAN port changed to " << argv[i + 1] << std::endl;
			++i;
		}
			
	}

	// Read all addresses from address file
	std::vector<std::string> MACAddressList = ReadAddresses(MACFileName);

	std::cout << "Broadcast address: " << BroadcastAddress << std::endl;


	// Sending Wake On LAN signals to all listed MAC addresses
	auto AddressIter = MACAddressList.cbegin();
	for (; AddressIter != MACAddressList.cend(); ++AddressIter)
	{
		if (SendWakeOnLAN((*AddressIter), PortAddress, BroadcastAddress))
		{
			std::cout << "Magic packet sent successful to : "<< *AddressIter <<":"<< PortAddress << std::endl;
		}
		else
		{
			return 1;
		}
	}

	return 0;

}