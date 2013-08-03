/*-------------------------------------------------------------------------------
Copyright 2013 Guido Ottaviani
guido@guiott.com
part of "ProtocolTester.ino"
acts as a tester exchanging data with the protocol:
http://www.guiott.com/Rino/CommandDescr/Protocol.htm
*/

/*-----------------------------------------------------------------------------*/
void TxData(char TxCmd, int TxCmdLen)
{// Send the requested parameters, with the correct handshake
    byte TxChkSum=0;
    char TxBuff[MAX_BUFF];
    int TxCount = 0;
    
    TmpBuff[0] = HEADER;
    TmpBuff[1] = ID;           // often not used, useful if many device on bus
    TmpBuff[2] = TxCmd;        // command
    TmpBuff[3] = TxCmdLen - RX_HEADER_LEN;     // lenght of the payload plus CHKSUM
    
    for (TxCount = 0; TxCount < TxCmdLen; TxCount ++)
    {// fill the buffer
      TxBuff[TxCount] = TmpBuff[TxCount];
    }

    for (int i=0; i<=TxCount - 1; i++)
    {// compute the checksum while sending
      TxChkSum += (byte)(TxBuff[i]);
      Serial.write(TxBuff[i]);
    }
   
    Serial.write(TxChkSum);
}

