// std includes
#include <iostream>


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
}


int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		PrintUsage(argv[0]);
		return 1;
	}
}