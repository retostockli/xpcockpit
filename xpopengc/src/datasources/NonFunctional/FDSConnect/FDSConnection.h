// FDSConnection.h: interface for the CFDSConnection class.
//
//////////////////////////////////////////////////////////////////////

#ifndef FDSCONNECTION_H
#define FDSCONNECTION_H

#include "windows.h"

#define FDS_CALL_CONNECTION			"FDSConnection:Call"

#define FDS_ACTION_READ				1
#define FDS_ACTION_WRITE			2
#define FDS_ACTION_READTRUE			3
#define FDS_ACTION_WRITETRUE		4
#define FDS_ACTION_READTOKEN		5
#define FDS_ACTION_WRITETOKEN		6
#define FDS_ACTION_SEARCH			7

#define FDS_RETURN_FAILURE			0
#define FDS_RETURN_SUCCESS			1

#define FDS_OK						0
#define FDS_ERROR_CONNECTION		1
#define FDS_ERROR_REGISTERMESSSAGE	2
#define FDS_ERROR_SENDMESSSAGE		3
#define FDS_ERROR_ALLREADYOPEN		4
#define FDS_ERROR_TIMEOUT			5
#define FDS_ERROR_CREATEATOM		6
#define FDS_ERROR_CREATEMAP			7
#define FDS_ERROR_CREATEVIEW		8
#define FDS_ERROR_MAXSIZEREACHED	9
#define FDS_ERROR_DATA				10


typedef struct tagFDS_ACTION_READ_HDR {
	DWORD action;
	DWORD offset;
	DWORD size;
	void* data;
} FDS_ACTION_READ_HDR;

typedef struct tagFDS_ACTION_WRITE_HDR {
	DWORD action;
	DWORD offset;
	DWORD size;
} FDS_ACTION_WRITE_HDR;

typedef struct tagFDS_ACTION_READTOKEN_HDR {
	DWORD action;
	DWORD token;
	void* data;
} FDS_ACTION_READTOKEN_HDR;

typedef struct tagFDS_ACTION_WRITETOKEN_HDR {
	DWORD action;
	DWORD token;
} FDS_ACTION_WRITETOKEN_HDR;

typedef struct tagFDS_ACTION_SEARCH_HDR {
	DWORD action;
	DWORD offset_from;
	DWORD offset_to;
	DWORD size;
} FDS_ACTION_SEARCH_HDR;

class CFDSConnection  
{
public:
	CFDSConnection();
	virtual ~CFDSConnection();

	BOOL Open();
	BOOL Close();

	BOOL Read(DWORD offset, DWORD size, void *data, BOOL use_true_offset = FALSE);
	BOOL Write(DWORD offset, DWORD size, void *data, BOOL use_true_offset = FALSE);
	BOOL ReadToken(DWORD token, void *data);
	BOOL WriteToken(DWORD token, void *data);
	BOOL Process();

	BOOL Search(DWORD offset_from, DWORD offset_to, DWORD size, void *data);

	DWORD GetResultMessage();

private:
	DWORD m_FDSResultMessage;
	HWND m_FDSConnectionWnd;
	UINT m_FDSCallMessage;

	ATOM m_Atom;
	HANDLE m_hMap;
	BYTE* m_pView;
	BYTE* m_pNext;
};

#endif // !defined FDSCONNECTION_H
