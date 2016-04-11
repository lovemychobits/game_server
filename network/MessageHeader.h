#ifndef NETWORK_MESSAGEHEADER_H
#define NETWORK_MESSAGEHEADER_H

#include <stdint.h>

namespace cpnet
{
	struct MessageHeader
	{
		uint32_t uMsgSize;
		uint32_t uMsgCmd;
		uint32_t uMsgReserved;
	};
}

#endif