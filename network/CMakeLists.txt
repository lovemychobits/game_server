CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
AUX_SOURCE_DIRECTORY(. LIBNET_SRC)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -Wno-unused-variable -pthread")

INCLUDE_DIRECTORIES("/usr/local/include/boost/")
INCLUDE_DIRECTORIES("/usr/local/log4cplus/include")

LINK_DIRECTORIES("/usr/local/lib")

MESSAGE(STATUS "dir src:" +${LIBNET_SRC})
SET(LIBRARY_OUTPUT_PATH "../libs")
ADD_LIBRARY(cpnet STATIC ${LIBNET_SRC})

TARGET_LINK_LIBRARIES(cpnet boost_system)
TARGET_LINK_LIBRARIES(cpnet boost_thread)
TARGET_LINK_LIBRARIES(cpnet thread)
