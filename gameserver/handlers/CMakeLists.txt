AUX_SOURCE_DIRECTORY(. HANDLERS_SRCS)

SET(CMAKE_CXX_FLAGS_RELEASE "-O2 -Wall -Wno-unused-variable -pthread")
SET(CMAKE_CXX_FLAGS_DEBUG "-g -Wall -Wno-unused-variable -pthread")

ADD_LIBRARY ( handlers ${HANDLERS_SRCS})
