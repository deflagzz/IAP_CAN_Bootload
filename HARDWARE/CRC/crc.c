#include "crc.h"

//查表
unsigned long CRC32Table[256];

void CRC32TableCreate(void)
{
    unsigned int c;
    unsigned int i, j;

    for (i = 0; i < 256; i++) {
        c = (unsigned int)i;
        for (j = 0; j < 8; j++) {
            if (c & 1)
                c = 0xedb88320L ^ (c >> 1);
            else
                c = c >> 1;
        }
        CRC32Table[i] = c;
    }

}

unsigned int CRC32Calculate(u8 *pBuf ,u16 pBufSize)
{
    unsigned int retCRCValue=0xffffffff;
    unsigned char *pData;
    pData=(unsigned char *)pBuf;
     while(pBufSize--)
     {
         retCRCValue=CRC32Table[(retCRCValue ^ *pData++) & 0xFF]^ (retCRCValue >> 8);
     }
     return retCRCValue^0xffffffff;
}






//一直算
uint16_t CalcCrc(unsigned char *chData, unsigned short uNo)
{
	uint16_t crc = 0xffff;
	uint16_t i, j;
	for(i = 0; i < uNo; i++)
	{
		crc ^= chData[i];
		for(j = 0; j < 8; j++)
		{
			if(crc & 1)
			{
				crc >>= 1;
				crc ^= 0xA001;
			}
			else
				crc >>= 1;
		}
	}
	return (crc);
}





