INCLUDE_DIRECTORIES("/usr/local/log4cplus/include/")
INCLUDE_DIRECTORIES("/usr/local/protobuf/include/")

AUX_SOURCE_DIRECTORY(. PLANE_SRCS)
ADD_LIBRARY (plane ${PLANE_SRCS})

