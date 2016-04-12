PROJECT(dataserver)
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

INCLUDE_DIRECTORIES("/usr/local/include")
INCLUDE_DIRECTORIES("/usr/local/mysql/include")
INCLUDE_DIRECTORIES("/usr/local/include/boost")
INCLUDE_DIRECTORIES("/usr/include/")

AUX_SOURCE_DIRECTORY(. DIR_SRCS)
MESSAGE(STATUS "dir src:" +${DIR_SRCS})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -Wall -pthread")

LINK_DIRECTORIES("../lib")
LINK_DIRECTORIES("/usr/local/lib")
LINK_DIRECTORIES("/usr/local/mysql/lib")
LINK_DIRECTORIES("/usr/local/protobuf/lib")

ADD_EXECUTABLE(dataserver ${DIR_SRCS})

TARGET_LINK_LIBRARIES(dataserver tcmalloc)
TARGET_LINK_LIBRARIES(dataserver cpnet)
TARGET_LINK_LIBRARIES(dataserver cputils)
TARGET_LINK_LIBRARIES(dataserver log4cplus)
TARGET_LINK_LIBRARIES(dataserver mysqlclient)
TARGET_LINK_LIBRARIES(dataserver boost_system)
TARGET_LINK_LIBRARIES(dataserver boost_thread)
TARGET_LINK_LIBRARIES(dataserver boost_date_time)
TARGET_LINK_LIBRARIES(dataserver protobuf)
TARGET_LINK_LIBRARIES(dataserver TinyXml)
