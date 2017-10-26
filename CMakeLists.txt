##############################
# CMakeLists for Wake-On-LAN #
##############################
cmake_minimum_required(VERSION 3.0)

project(WakeOnLAN)

set(SRC
	WakeOnLAN.cpp
)


add_executable(WakeOnLAN ${SRC})