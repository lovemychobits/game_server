PROJECT(lobbyserver)
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

INCLUDE_DIRECTORIES("/usr/local/include")
INCLUDE_DIRECTORIES("/usr/local/include/boost/")
INCLUDE_DIRECTORIES("/usr/local/log4cplus/include/")
INCLUDE_DIRECTORIES("/usr/local/protobuf/include/")
INCLUDE_DIRECTORIES("/usr/include/mysql")

ADD_SUBDIRECTORY(config)
ADD_SUBDIRECTORY(handlers)
ADD_SUBDIRECTORY(gsgroup)
ADD_SUBDIRECTORY(playermng)
ADD_SUBDIRECTORY(lzSDK)
ADD_SUBDIRECTORY(jsonhelper)
ADD_SUBDIRECTORY(mysql)


AUX_SOURCE_DIRECTORY(. DIR_SRCS)
MESSAGE(STATUS "dir src:" +${DIR_SRCS})

SET(CMAKE_CXX_FLAGS_RELEASE "-std=c++11 -O2 -Wall -Wno-unused-variable -pthread")
SET(CMAKE_CXX_FLAGS_DEBUG "-std=c++11 -g -Wall -Wno-unused-variable -pthread")

LINK_DIRECTORIES("../libs")
LINK_DIRECTORIES("/usr/local/lib")
LINK_DIRECTORIES("/usr/local/log4cplus/lib")
LINK_DIRECTORIES("/usr/local/protobuf/lib")
LINK_DIRECTORIES("/usr/lib64/mysql")

SET(EXECUTABLE_OUTPUT_PATH "../bin/lobbyserver/")
ADD_EXECUTABLE(lobbyserver ${DIR_SRCS})

#TARGET_LINK_LIBRARIES(lobbyserver tcmalloc)

TARGET_LINK_LIBRARIES(lobbyserver config)
TARGET_LINK_LIBRARIES(lobbyserver handlers)
TARGET_LINK_LIBRARIES(lobbyserver gsgroup)
TARGET_LINK_LIBRARIES(lobbyserver playermng)
TARGET_LINK_LIBRARIES(lobbyserver lzsdk)
TARGET_LINK_LIBRARIES(lobbyserver jsonhelper)
TARGET_LINK_LIBRARIES(lobbyserver datamng)
TARGET_LINK_LIBRARIES(lobbyserver proto)

TARGET_LINK_LIBRARIES(lobbyserver redisclient)
TARGET_LINK_LIBRARIES(lobbyserver cpnet)
TARGET_LINK_LIBRARIES(lobbyserver cputils)

TARGET_LINK_LIBRARIES(lobbyserver TinyXml)
TARGET_LINK_LIBRARIES(lobbyserver log4cplus)

TARGET_LINK_LIBRARIES(lobbyserver boost_system)
TARGET_LINK_LIBRARIES(lobbyserver boost_timer)
TARGET_LINK_LIBRARIES(lobbyserver boost_thread)
TARGET_LINK_LIBRARIES(lobbyserver boost_date_time)
TARGET_LINK_LIBRARIES(lobbyserver protobuf)
TARGET_LINK_LIBRARIES(lobbyserver mysqlclient)
