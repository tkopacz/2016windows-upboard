// AaeonIo.h : main header file for the AaeonIo DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols

#define AAEONIO_API __declspec(dllexport)

// CAaeonIoApp
// See AaeonIo.cpp for the implementation of this class
//

class CAaeonIoApp : public CWinApp
{
public:
	CAaeonIoApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

extern "C" {

AAEONIO_API HANDLE aaeonioOpen();
AAEONIO_API BOOL aaeonioClose( HANDLE hInst );

AAEONIO_API BYTE aaeonioReadPortByte( HANDLE hInst, WORD wPort );
AAEONIO_API VOID aaeonioWritePortByte( HANDLE hInst, WORD wPort, BYTE bData );
AAEONIO_API WORD aaeonioReadPortWord( HANDLE hInst, WORD wPort );
AAEONIO_API VOID aaeonioWritePortWord( HANDLE hInst, WORD wPort, WORD wData );
AAEONIO_API DWORD aaeonioReadPortDWord( HANDLE hInst, WORD wPort );
AAEONIO_API VOID aaeonioWritePortDWord( HANDLE hInst, WORD wPort, DWORD dwData );

AAEONIO_API BYTE aaeonioReadMemByte( HANDLE hInst, ULONG uAddr );
AAEONIO_API VOID aaeonioWriteMemByte( HANDLE hInst, ULONG uAddr, BYTE bData );
AAEONIO_API WORD aaeonioReadMemWord( HANDLE hInst, ULONG uAddr );
AAEONIO_API VOID aaeonioWriteMemWord( HANDLE hInst, ULONG uAddr, WORD wData );
AAEONIO_API DWORD aaeonioReadMemDWord( HANDLE hInst, ULONG uAddr );
AAEONIO_API VOID aaeonioWriteMemDWord( HANDLE hInst, ULONG uAddr, DWORD dwData );

AAEONIO_API BOOL aaeonioReadMSR( HANDLE hInst, DWORD index, PDWORD eax, PDWORD edx, DWORD_PTR threadAffinityMask );
}