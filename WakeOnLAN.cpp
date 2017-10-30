// std includes
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>

// windows socket includes
#include <WinSock2.h>
#pragma comment(lib, "Ws2_32.lib")



void PrintUsage(char* msg)
{
	std::cout << "USAGE: " << msg << " MACAddresses.txt" << std::endl;
	std::cout << "For further information call: " << msg << " --help" << std::endl;
}

void PrintHelp()
{
	std::cout << "-- Wake On LAN --" << std::endl;
	std::cout << "For usage please create text file names 'MACAddresses.txt' with on MAC address per line.";
	std::cout << " For every MAC address a broadcast Wake-On-LAN call will be send.";
	std::cout << " A MAC address has the form 'XX:XX:XX:XX:XX:XX'." << std::endl;

	std::cout << std::endl << "-- Basic call --" << std::endl;
	std::cout << "USAGE: WakeOnLAN.exe MACAddresses.txt" << std::endl;
	
	std::cout << std::endl << "-- Optional parameters --" << std::endl;
	std::cout << "-h or --help \t Shows this information"<< std::endl;
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
			if (Address.size() > 0)
				MACAddresses.push_back(Address);
		}
		FID.close();
	}

	return MACAddresses;
}

bool SendWakeOnLAN(const std::string MACAddress, unsigned PortAddress, unsigned long BroadcastAddress)
{

	// Build message
	// 6x 0xFF followed 16x MACAddress
	std::string Message( 6, 0xFF );
	for (auto i = 0; i < 16; ++i)
	{
		Message += MACAddress;
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
		const int optval{ 1 };
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
		if (sendto(SendingSocket, (char*) Message.c_str(), 102, 0, reinterpret_cast<sockaddr*>(&LANDestination), sizeof(LANDestination)) != NO_ERROR)
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
	// Handle input arguments
	if (argc != 2)
	{
		PrintUsage(argv[0]);
		return 1;
	}

	
	for (auto i = 1; i < argc; ++i)
	{
		
		if ( !strcmp(argv[i],"--help") || !strcmp(argv[i],"-h"))
		{
			PrintHelp();
		}
			
	}

	// Read all addresses from address file
	std::string MACFileName{ "MACAddresses.txt" };
	std::vector<std::string> MACAddressList = ReadAddresses(MACFileName);

	// Sending configurations
	unsigned short PortAddress{ 9 };
	unsigned long BroadcastAddress{ 0xFFFFFFFF };


	// Sending Wake On LAN signals to all listed MAC addresses
	auto AddressIter = MACAddressList.cbegin();
	for (; AddressIter != MACAddressList.cend(); ++AddressIter)
	{
		if (SendWakeOnLAN((*AddressIter), PortAddress, BroadcastAddress))
		{
			std::cout << "Sending magic packet successful!" << std::endl;
		}
		else
		{
			std::cout << "Error occured!" << std::endl;
			return 1;
		}
	}

	
	return 0;

}