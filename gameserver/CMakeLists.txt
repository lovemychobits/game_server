PROJECT(gameserver)
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

INCLUDE_DIRECTORIES("/usr/local/include")
INCLUDE_DIRECTORIES("/usr/local/include/boost/")
INCLUDE_DIRECTORIES("/usr/local/log4cplus/include/")
INCLUDE_DIRECTORIES("/usr/local/protobuf/include/")

ADD_SUBDIRECTORY(config)
ADD_SUBDIRECTORY(fsm)
ADD_SUBDIRECTORY(handlers)
ADD_SUBDIRECTORY(slither)

AUX_SOURCE_DIRECTORY(. DIR_SRCS)
MESSAGE(STATUS "dir src:" +${DIR_SRCS})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wextra -Wall -pthread")

LINK_DIRECTORIES("../libs")
LINK_DIRECTORIES("/usr/local/lib")
LINK_DIRECTORIES("/usr/local/log4cplus/lib")
LINK_DIRECTORIES("/usr/local/protobuf/lib")

SET(EXECUTABLE_OUTPUT_PATH "../bin/gameserver/")
ADD_EXECUTABLE(gameserver ${DIR_SRCS})

#TARGET_LINK_LIBRARIES(gameserver tcmalloc)

TARGET_LINK_LIBRARIES(gameserver config)
TARGET_LINK_LIBRARIES(gameserver fsm)
TARGET_LINK_LIBRARIES(gameserver handlers)
TARGET_LINK_LIBRARIES(gameserver slither)
TARGET_LINK_LIBRARIES(gameserver proto)

TARGET_LINK_LIBRARIES(gameserver redisclient)
TARGET_LINK_LIBRARIES(gameserver cpnet)
TARGET_LINK_LIBRARIES(gameserver cputils)

TARGET_LINK_LIBRARIES(gameserver TinyXml)
TARGET_LINK_LIBRARIES(gameserver log4cplus)

TARGET_LINK_LIBRARIES(gameserver boost_system)
TARGET_LINK_LIBRARIES(gameserver boost_timer)
TARGET_LINK_LIBRARIES(gameserver boost_thread)
TARGET_LINK_LIBRARIES(gameserver boost_date_time)
TARGET_LINK_LIBRARIES(gameserver protobuf)
