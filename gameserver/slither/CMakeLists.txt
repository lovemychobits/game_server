INCLUDE_DIRECTORIES("/usr/local/log4cplus/include/")
INCLUDE_DIRECTORIES("/usr/local/protobuf/include/")

AUX_SOURCE_DIRECTORY(. SLITHER_SRCS)

SET(CMAKE_CXX_FLAGS_RELEASE "-O2 -Wall -Wno-unused-variable -pthread")
SET(CMAKE_CXX_FLAGS_DEBUG "-g -Wall -Wno-unused-variable -pthread")

ADD_LIBRARY ( slither ${SLITHER_SRCS})
