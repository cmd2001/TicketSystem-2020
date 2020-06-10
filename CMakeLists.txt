cmake_minimum_required(VERSION 3.10.2)
project(TicketSystem_2020)
set(CMAKE_CXX_STANDARD 14)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}   -O3")

include_directories(
	${PROJECT_SOURCE_DIR}/src/
	${PROJECT_SOURCE_DIR}/BPlusTree/
	${PROJECT_SOURCE_DIR}/include/
)

AUX_SOURCE_DIRECTORY(src DIR_SRCS)

add_executable(code ${DIR_SRCS})