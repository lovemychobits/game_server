CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
AUX_SOURCE_DIRECTORY(. LIBNET_SRC)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -Wno-unused-variable -pthread")

#SET(LIBHELLO_SRC .)

INCLUDE_DIRECTORIES("/usr/local/include/boost/")
INCLUDE_DIRECTORIES("/usr/local/include/log4cplus/")
#INCLUDE_DIRECTORIES("../include")

LINK_DIRECTORIES("/usr/local/lib")
#LINK_DIRECTORIES("/usr/lib/x86_64-linux-gnu/")
#TARGET_LINK_LIBRARIES(net boost_system)
#TARGET_LINK_LIBRARIES(net boost_thread)

MESSAGE(STATUS "dir src:" +${LIBNET_SRC})
SET(LIBRARY_OUTPUT_PATH "../lib")
ADD_LIBRARY(cpnet STATIC ${LIBNET_SRC})

TARGET_LINK_LIBRARIES(cpnet boost_system)
TARGET_LINK_LIBRARIES(cpnet boost_thread)
TARGET_LINK_LIBRARIES(cpnet thread)

