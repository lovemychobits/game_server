AUX_SOURCE_DIRECTORY(. JSONHELPER_SRCS)

SET(CMAKE_CXX_FLAGS_RELEASE "-std=c++11 -O2 -Wall -Wno-unused-variable -pthread")
SET(CMAKE_CXX_FLAGS_DEBUG "-std=c++11 -g -Wall -Wno-unused-variable -pthread")

ADD_LIBRARY ( jsonhelper ${JSONHELPER_SRCS})
