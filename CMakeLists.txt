cmake_minimum_required(VERSION 3.10.2)
project(TicketSystem_2020)
set(CMAKE_CXX_STANDARD 14)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}   -g")

include_directories(
	${PROJECT_SOURCE_DIR}/src/
	${PROJECT_SOURCE_DIR}/BPlusTree/
	${PROJECT_SOURCE_DIR}/include/
)

AUX_SOURCE_DIRECTORY(src DIR_SRCS)

add_executable(TicketSystem_2020 ${DIR_SRCS})