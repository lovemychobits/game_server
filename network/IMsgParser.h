#ifndef NETWORK_IMSGPARSER_H
#define NETWORK_IMSGPARSER_H

#include "header.h"

/*
	the user should to check whether there is at least one completion msg
*/

namespace cpnet
{
	class IMsgParser
	{
	public:
		// check the message header is completion
		virtual size_t CheckMsgHeader(void* pMsg, size_t nBytesTransfered) = 0;
		// check the message body is completion
		virtual size_t CheckMsgBody(void* pMsg, size_t nBytesTransfered) = 0;
	};
}

#endif