PROJECT(lobbyserver)
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

INCLUDE_DIRECTORIES("/usr/local/include")
INCLUDE_DIRECTORIES("/usr/local/include/boost")
INCLUDE_DIRECTORIES("/usr/local/mysql/include")

AUX_SOURCE_DIRECTORY(. DIR_SRCS)
MESSAGE(STATUS "dir src:" +${DIR_SRCS})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -Wall -pthread")

LINK_DIRECTORIES("../lib")
LINK_DIRECTORIES("/usr/local/lib")
LINK_DIRECTORIES("/usr/local/mysql/lib")

ADD_EXECUTABLE(lobbyserver ${DIR_SRCS})

TARGET_LINK_LIBRARIES(lobbyserver tcmalloc)
TARGET_LINK_LIBRARIES(lobbyserver cpnet)
TARGET_LINK_LIBRARIES(lobbyserver cputils)
TARGET_LINK_LIBRARIES(lobbyserver log4cplus)
TARGET_LINK_LIBRARIES(lobbyserver mysqlclient)
TARGET_LINK_LIBRARIES(lobbyserver boost_system)
TARGET_LINK_LIBRARIES(lobbyserver boost_thread)
TARGET_LINK_LIBRARIES(lobbyserver boost_date_time)
TARGET_LINK_LIBRARIES(lobbyserver protobuf)
TARGET_LINK_LIBRARIES(lobbyserver TinyXml)
