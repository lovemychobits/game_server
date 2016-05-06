using UnityEngine;
using System.Collections;

public class MessageHeader
{
    [System.Serializable]
    [System.Runtime.InteropServices.StructLayout(System.Runtime.InteropServices.LayoutKind.Sequential, Pack = 4)]
    public struct Header
    {
        public int messageSize;
        public int messageCommand;
        public uint messageCheck;

        public Header(int message_size, int message_command)
        {
            messageSize = message_size;
            messageCommand = message_command;
            messageCheck = 0xA1B2C3D4;
        }

        public override string ToString()
        {
            string retValue = string.Format("messageSize = {0}, messageCommand = {1}, messageCheck = {2}", messageSize, messageCommand, messageCheck);
            return retValue;
        }
    };
}
