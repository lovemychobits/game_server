CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
AUX_SOURCE_DIRECTORY(. LIBNET_SRC)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -Wno-unused-variable -pthread")

INCLUDE_DIRECTORIES("/usr/local/include/boost/")
INCLUDE_DIRECTORIES("/usr/local/include/log4cplus/")

LINK_DIRECTORIES("/usr/local/lib")

MESSAGE(STATUS "dir src:" +${LIBNET_SRC})
SET(LIBRARY_OUTPUT_PATH "../lib")
ADD_LIBRARY(cputils STATIC ${LIBNET_SRC})

TARGET_LINK_LIBRARIES(cputils boost_system)
TARGET_LINK_LIBRARIES(cputils boost_thread)
TARGET_LINK_LIBRARIES(cputils thread)

