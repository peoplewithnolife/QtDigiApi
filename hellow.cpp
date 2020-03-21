#include "DbgUtil.h"
#include "digiApi.h"
#include "digiApiCmd.h"

#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <iostream>
using namespace std;

static HANDLE hComm;

int SerialSetup(void); 
int SerialTestWrite(void);
int SerialTestRead(void);
int SerialClose(void);

DWORD WINAPI MyThread(LPVOID lpParameter)
{
	unsigned int& myCounter = *((unsigned int*)lpParameter);
    while(myCounter < 10) 
    {
        Sleep(2000);
        ++myCounter;
    }
	//while(myCounter < 0xFFFFFFFF) ++myCounter;
	return 0;
}


//int WINAPI WinMainNk(HINSTANCE hInstance,
//                   HINSTANCE hPrevInstance,
//                   LPSTR lpCmdLine,
//                   int nCmdShow) {
/*
bytearray(b'\x17\x01\x00\x13\xa2\x00@\xad\x14.\xff\xfe\x02NI')
17010013a20040ad142efffe024e49
19
bytearray(b'~\x00\x0f\x17\x01\x00}3\xa2\x00@\xad\x14.\xff\xfe\x02NIm')
7e000f1701007d33a20040ad142efffe024e496d
---------- now decode---------
17010013a20040ad142efffe024e49
bytearray(b'\x17\x01\x00\x13\xa2\x00@\xad\x14.\xff\xfe\x02NI')
Ended */
void ApiTest(void)
{
   uint8_t rawFrm[15] = {0x17,0x01,0x00,0x13,0xA2,0x00,0x40,0xAD,0x14,0x2E,0xFF,0xFE,0x02,0x4E,0x49};
   uint16_t pLen;
   uint8_t dork[64];
   uint8_t dorky[1024];
   uint8_t dorkDec[256];
   memset(dork,0,64);
   memset(dorkDec,0,256);
   pLen = ApiRawFrame(rawFrm,15,dork);
   DumpByteStr(dork,dorky,pLen);
   printf((char*)dorky);

   pLen = ApiDecodeFrame(dork,pLen,dorkDec,1024,1);
   DumpByteStr(dorkDec,dorky,pLen);
   printf((char*)dorky);
}

int HelloMain(void)
{
   if (0)
   {
      ApiTest();
//      uint16_t pLen;
//      uint8_t dork[64];
//      uint8_t dorky[1024];
//      uint8_t dorkAsc[256];
//      memset(dork,0,64);
//      memset(dorkAsc,0,256);
////      pLen = ApiGetParam((uint8_t*) "VL", 2, dork);
//      DumpByteStr(dork,dorky,pLen);
//      printf((char*)dorky);
//      DumpAsciiStr(dork,dorkAsc,pLen);
//      printf((char*)dorkAsc);
      return 0;
   }
	unsigned int myCounter = 0;
	DWORD myThreadID;

   SerialSetup();
   //HANDLE myHandle = CreateThread(0, 0, MyThread, &myCounter, 0, &myThreadID);
    //CreateThread(NULL,1024,MyThread,NULL,NULL,&id);
   //MessageBox(NULL, TEXT("Phil SUX twice"), TEXT("buhbu"), MB_OK);

   SerialTestWrite();
   SerialTestRead();
   SerialClose();

   //char myChar = ' ';
   //while(myChar != 'q') {
   //	cout << myCounter << endl;
   //   myChar = (char) getchar();
   //}
	
   //CloseHandle(myHandle);
 
   printf("And thats it....\n");

   return 0;
}

int SerialTestWrite(void)
{
   uint16_t pLen;
   uint8_t dork[64];
   BOOL status;
   DWORD dNoOFBytestoWrite;         // No of bytes to write into the port
   DWORD dNoOfBytesWritten = 0;     // No of bytes written to the port

   pLen = ApiGetParam((uint8_t*) "VL", 2, dork);

   dNoOFBytestoWrite = pLen;

   status = WriteFile(hComm,        // Handle to the Serial port
                      dork,     // Data to be written to the port
                      dNoOFBytestoWrite,  //No of bytes to write
                      &dNoOfBytesWritten, //Bytes written
                      NULL);
   if (status = true)
   {
      printf("Worte %d bytes\n\n",dNoOfBytesWritten);
   }
   else
   {
      printf(":( :( >:( FAIL");
   }
   return 0;
}

int SerialTestRead(void)
{
   char TempChar; //Temporary character used for reading
   char SerialBuffer[256];//Buffer for storing Rxed Data
   uint8_t dorkV[1024];
   uint8_t dorkMsgV[256];
   DWORD NoBytesRead;
   int i = 0;
   uint16_t decodeLen;

   BOOL status;
   status = SetCommMask(hComm, EV_RXCHAR);
   DWORD dwEventMask;
   status = WaitCommEvent(hComm, &dwEventMask, NULL);

   do
    {
      ReadFile( hComm,           //Handle of the Serial port
                &TempChar,       //Temporary character
                sizeof(TempChar),//Size of TempChar
                &NoBytesRead,    //Number of bytes read
                NULL);

      SerialBuffer[i] = TempChar;// Store Tempchar into buffer
      i++;
     } while (NoBytesRead > 0);
   DumpByteStr((uint8_t*)SerialBuffer,dorkV,i);
   printf((char*)dorkV);
   DumpAsciiStr((uint8_t*)SerialBuffer,dorkV,i);
   printf((char*)dorkV);

   printf("Trying to decode ....\n");
   decodeLen = ApiDecodeFrame((uint8_t*)SerialBuffer,i,dorkMsgV,256,1);
   printf("decoded? ....\n");
   DumpByteStr((uint8_t*)dorkMsgV,dorkV,decodeLen);
   printf((char*)dorkV);
   DumpAsciiStr((uint8_t*)dorkMsgV,dorkV,decodeLen);
   printf((char*)dorkV);
   return 0;
}

int SerialClose(void)
{
   CloseHandle(hComm);
   return 0;
}

int SerialSetup(void)
{
    BOOL status;
    DCB dcbSerialParams;
    COMMTIMEOUTS timeouts;

    memset(&dcbSerialParams,0,sizeof(dcbSerialParams));
    memset(&timeouts, 0, sizeof(timeouts));

    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

    printf("Dork\n");
    //CreateFile();
    hComm = CreateFile(TEXT("\\\\.\\COM21"),                //port name
                        GENERIC_READ | GENERIC_WRITE, //Read/Write
                        0,                            // No Sharing
                        NULL,                         // No Security
                        OPEN_EXISTING,// Open existing port only
                        0,            // Non Overlapped I/O
                        NULL);        // Null for Comm Devices

    if (hComm == INVALID_HANDLE_VALUE)
    {
        printf("Error in opening serial port");
    }
    else
    {
        printf("opening serial port successful");
        status = GetCommState(hComm, &dcbSerialParams);
        dcbSerialParams.BaudRate = CBR_115200;  // Setting BaudRate = 9600
        dcbSerialParams.ByteSize = 8;         // Setting ByteSize = 8
        dcbSerialParams.StopBits = ONESTOPBIT;// Setting StopBits = 1
        dcbSerialParams.Parity   = NOPARITY;  // Setting Parity = None
        SetCommState(hComm, &dcbSerialParams);
        status = GetCommState(hComm, &dcbSerialParams);
        timeouts.ReadIntervalTimeout         = 50; // in milliseconds
        timeouts.ReadTotalTimeoutConstant    = 50; // in milliseconds
        timeouts.ReadTotalTimeoutMultiplier  = 10; // in milliseconds
        timeouts.WriteTotalTimeoutConstant   = 50; // in milliseconds
        timeouts.WriteTotalTimeoutMultiplier = 10; // in milliseconds
        SetCommTimeouts(hComm,&timeouts);
    }

    //CloseHandle(hComm);//Closing the Serial Port

    return 0;

}

/*
int main(int argc, char* argv[])
{
    HANDLE hComm;
    char ch;

    for (int i = 0; i < argc; i++)
        printf("%s\n", argv[i]);

    hComm = CreateFileA( argv[1],GENERIC_READ | GENERIC_WRITE,
                        0,
                        0,
                        OPEN_EXISTING,
                        NULL,
                        0);

    if (hComm == INVALID_HANDLE_VALUE)
    {
        printf("Cannot open %s\n", argv[1]);        //error occured alert user about error
        return -1;
    }

    printf("Press the following keys:\n");
    printf("1: Set DTR\n");
    printf("2: Clear DTR\n");
    printf("3: Set RTS\n");
    printf("4: Clear RTS\n");
    printf("q: End Program\n");

    do
    {
        ch = _getch();
        switch (ch)
        {
        case '1':   if (EscapeCommFunction(hComm,SETDTR) == 0)
                        printf ("Error Setting DTR\n");
                    break;
        case '2':   if (EscapeCommFunction(hComm,CLRDTR) == 0)
                        printf ("Error Clearing DTR\n");
                    break;
        case '3':   if (EscapeCommFunction(hComm,SETRTS) == 0)
                        printf ("Error Setting CTS\n");
                    break;
        case '4':   if (EscapeCommFunction(hComm,CLRRTS) == 0)
                        printf ("Error Clearing CTS\n");
                    break;
        }
    }   while (ch != 'q');


    return 0;
}
 */
