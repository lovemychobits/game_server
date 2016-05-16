set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wextra -Wall -pthread")

INCLUDE_DIRECTORIES("/usr/local/log4cplus/include/")
INCLUDE_DIRECTORIES("/usr/local/protobuf/include/")

AUX_SOURCE_DIRECTORY(. SLITHER_SRCS)
ADD_LIBRARY ( slither ${SLITHER_SRCS})
