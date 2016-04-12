#ifndef NETWORK_PROTOTEMPLATE_HPP
#define NETWORK_PROTOTEMPLATE_HPP

#include "../network/MessageHeader.h"
#include <string>
#include <set>

#if !defined(_MSC_VER)
	#include <sys/time.h>
	#include <unistd.h>
#else
	#include <windows.h>
	#include <time.h>
#endif


namespace cpnet
{
	template <typename T>
	string GetProtoData(T* pMsgHeader)
	{
		char* pBuf = (char*)pMsgHeader + sizeof(T);
		int uDataSize = pMsgHeader->uMsgSize - sizeof(T);
		return string(pBuf, uDataSize);
	}

	// ����protobuf��request��Ϣ
	template <typename T>
	void BuildRequestProto(const T& requestData, string& strBuffer, uint32_t uMsgCmd)
	{
		string strOutput;
		requestData.SerializeToString(&strOutput);

		MessageHeader msgHeader; 
		msgHeader.uMsgSize = sizeof(MessageHeader) + strOutput.size();
		msgHeader.uMsgCmd = uMsgCmd;
		msgHeader.uMsgReserved = 0xA1B2C3D4;
		strBuffer.append((char*)&msgHeader, sizeof(MessageHeader));
		strBuffer.append(strOutput);
		return;
	}


	// ����protobuf��response��Ϣ
	template <typename T>
	void BuildResponseProto(const T& responseData, string& strBuffer, uint32_t uMsgCmd)
	{
		string strOutput;
		responseData.SerializeToString(&strOutput);

		MessageHeader msgHeader; 
		msgHeader.uMsgSize = sizeof(MessageHeader) + strOutput.size();
		msgHeader.uMsgCmd = uMsgCmd;
		msgHeader.uMsgReserved = 0xA1B2C3D4;
		strBuffer.append((char*)&msgHeader, sizeof(MessageHeader));
		strBuffer.append(strOutput);
		return;
	}
}
#endif