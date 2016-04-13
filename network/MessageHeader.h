#ifndef NETWORK_MESSAGEHEADER_H
#define NETWORK_MESSAGEHEADER_H

#include <stdint.h>

namespace cpnet
{
#define MAGIC_CODE 0xA1B2C3D4
	struct MessageHeader
	{
		uint32_t uMsgSize;
		uint32_t uMsgCmd;
		uint32_t uMsgReserved;
	};
}

#endif