AUX_SOURCE_DIRECTORY(. CONFIG_SRCS)

SET(CMAKE_CXX_FLAGS_RELEASE "-O2 -Wall -Wno-unused-variable -pthread")
SET(CMAKE_CXX_FLAGS_DEBUG "-g -Wall -Wno-unused-variable -pthread")

ADD_LIBRARY ( config ${CONFIG_SRCS})
