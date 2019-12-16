// FDSConnection.cpp: implementation of the CFDSConnection class.
//
//////////////////////////////////////////////////////////////////////

//#include "stdafx.h"
#include "windows.h"
#include "FDSConnection.h"

#define MAX_SIZE 0x7F00
#define MAX_RETRIES 5

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFDSConnection::CFDSConnection()
{
	m_FDSResultMessage = -1;
	m_FDSConnectionWnd = NULL;
	m_FDSCallMessage = -1;

	m_Atom = 0;
	m_hMap = 0;
	m_pView = NULL;
	m_pNext = NULL;
}

CFDSConnection::~CFDSConnection()
{
	Close();
}

/////////////////////////////////////////////////////////////////////
BOOL CFDSConnection::Open()
{
	char szName[MAX_PATH];
	static int nTry = 0;

	if (m_pView) {
		m_FDSResultMessage = FDS_ERROR_ALLREADYOPEN;
		return FALSE;
	}

	m_FDSConnectionWnd = FindWindow("FDSConnection", NULL);   // locate FSConnect
	if (!m_FDSConnectionWnd) {
		m_FDSConnectionWnd = FindWindow(NULL, "FDSWideConnectionClient");
		if (!m_FDSConnectionWnd) {
			m_FDSResultMessage = FDS_ERROR_CONNECTION;
			return FALSE;
		}
	}

	m_FDSCallMessage = RegisterWindowMessage(FDS_CALL_CONNECTION);
	if (m_FDSCallMessage == 0) {
		m_FDSResultMessage = FDS_ERROR_REGISTERMESSSAGE;
		return FALSE;
	}


	wsprintf(szName, FDS_CALL_CONNECTION ":%X:%X", GetCurrentProcessId(), nTry);

	m_Atom = GlobalAddAtom(szName);
	if (m_Atom == 0) {
		m_FDSResultMessage = FDS_ERROR_CREATEATOM;
		Close();
		return FALSE;
	}

	m_hMap = CreateFileMapping((HANDLE)0xFFFFFFFF,
							   NULL,
							   PAGE_READWRITE,
							   0, 
							   MAX_SIZE+256,
							   szName);

	if ((m_hMap == 0) || (GetLastError() == ERROR_ALREADY_EXISTS)) {	
		m_FDSResultMessage = FDS_ERROR_CREATEMAP;
		Close();
		return FALSE;
	}

	m_pView = (BYTE*)MapViewOfFile(m_hMap, FILE_MAP_WRITE, 0, 0, 0);
	if (m_pView == NULL) {	
		m_FDSResultMessage = FDS_ERROR_CREATEVIEW;
		Close();
		return FALSE;
	}
	m_pNext = m_pView;

	m_FDSResultMessage = FDS_OK;
	return TRUE;
}

BOOL CFDSConnection::Close()
{
	m_FDSConnectionWnd = 0;
	m_FDSCallMessage = 0;
  
	if (m_Atom) {	
		GlobalDeleteAtom(m_Atom);
		m_Atom = 0;
	}

	if (m_pView) {	
		UnmapViewOfFile((LPVOID)m_pView);
		m_pView = 0;
	}

	if (m_hMap) {	
		CloseHandle(m_hMap);
		m_hMap = 0;
	}

	m_FDSResultMessage = FDS_OK;
	return TRUE;
}

BOOL CFDSConnection::Read(DWORD offset, DWORD size, void *data, BOOL use_true_offset /*= FALSE*/)
{
	FDS_ACTION_READ_HDR* pHdr = (FDS_ACTION_READ_HDR*) m_pNext;

	if (!m_pView) {	
		m_FDSResultMessage = FDS_ERROR_CONNECTION;
		return FALSE;
	}

	if (((m_pNext - m_pView) + 4 + (size + sizeof(FDS_ACTION_READ_HDR))) > MAX_SIZE) {	
		m_FDSResultMessage = FDS_ERROR_MAXSIZEREACHED;
		return FALSE;
	}

	if (use_true_offset)
		pHdr->action = FDS_ACTION_READTRUE;
	else
		pHdr->action = FDS_ACTION_READ;
	pHdr->offset = offset;
	pHdr->size = size;
	pHdr->data = (BYTE*)data;

	if (size) ZeroMemory(&m_pNext[sizeof(FDS_ACTION_READ_HDR)], size);

	m_pNext += sizeof(FDS_ACTION_READ_HDR) + size;

	m_FDSResultMessage = FDS_OK;
	return TRUE;
}

BOOL CFDSConnection::Write(DWORD offset, DWORD size, void *data, BOOL use_true_offset /*= FALSE*/)
{
	FDS_ACTION_WRITE_HDR* pHdr = (FDS_ACTION_WRITE_HDR*) m_pNext;

	if (!m_pView) {	
		m_FDSResultMessage = FDS_ERROR_CONNECTION;
		return FALSE;
	}

	if (((m_pNext - m_pView) + 4 + (size + sizeof(FDS_ACTION_WRITE_HDR))) > MAX_SIZE) {	
		m_FDSResultMessage = FDS_ERROR_MAXSIZEREACHED;
		return FALSE;
	}

	if (use_true_offset)
		pHdr->action = FDS_ACTION_WRITETRUE;
	else
		pHdr->action = FDS_ACTION_WRITE;
	pHdr->offset = offset;
	pHdr->size = size;

	if (size) CopyMemory(&m_pNext[sizeof(FDS_ACTION_WRITE_HDR)], data, size);

	m_pNext += sizeof(FDS_ACTION_WRITE_HDR) + size;

	m_FDSResultMessage = FDS_OK;
	return TRUE;
}

BOOL CFDSConnection::ReadToken(DWORD token, void *data)
{
	FDS_ACTION_READTOKEN_HDR* pHdr = (FDS_ACTION_READTOKEN_HDR*) m_pNext;

	if (!m_pView) {	
		m_FDSResultMessage = FDS_ERROR_CONNECTION;
		return FALSE;
	}

	if (((m_pNext - m_pView) + 4 + (8 + sizeof(FDS_ACTION_READTOKEN_HDR))) > MAX_SIZE) {	
		m_FDSResultMessage = FDS_ERROR_MAXSIZEREACHED;
		return FALSE;
	}

	pHdr->action = FDS_ACTION_READTOKEN;
	pHdr->token = token;
	pHdr->data = (BYTE*)data;

	ZeroMemory(&m_pNext[sizeof(FDS_ACTION_READTOKEN_HDR)], 8);

	m_pNext += sizeof(FDS_ACTION_READTOKEN_HDR) + 8;

	m_FDSResultMessage = FDS_OK;
	return TRUE;
}

BOOL CFDSConnection::WriteToken(DWORD token, void *data)
{
	FDS_ACTION_WRITETOKEN_HDR* pHdr = (FDS_ACTION_WRITETOKEN_HDR*) m_pNext;

	if (!m_pView) {	
		m_FDSResultMessage = FDS_ERROR_CONNECTION;
		return FALSE;
	}

	if (((m_pNext - m_pView) + 4 + (8 + sizeof(FDS_ACTION_WRITETOKEN_HDR))) > MAX_SIZE) {	
		m_FDSResultMessage = FDS_ERROR_MAXSIZEREACHED;
		return FALSE;
	}

	pHdr->action = FDS_ACTION_WRITETOKEN;
	pHdr->token = token;

	CopyMemory(&m_pNext[sizeof(FDS_ACTION_WRITETOKEN_HDR)], data, 8);

	m_pNext += sizeof(FDS_ACTION_WRITETOKEN_HDR) + 8;

	m_FDSResultMessage = FDS_OK;
	return TRUE;
}


BOOL CFDSConnection::Process()
{

	DWORD error = 0;
	DWORD* pdw = NULL;
	FDS_ACTION_READ_HDR* pHdrR = NULL;
	FDS_ACTION_WRITE_HDR* pHdrW = NULL;
	FDS_ACTION_READTOKEN_HDR* pHdrRT = NULL;
	FDS_ACTION_WRITETOKEN_HDR* pHdrWT = NULL;
	FDS_ACTION_SEARCH_HDR* pHdrS = NULL;
	int i = 0;

	if (!m_pView) {
		m_FDSResultMessage = FDS_ERROR_CONNECTION;
		return FALSE;
	}
	if (m_pView == m_pNext) {
		m_FDSResultMessage = FDS_ERROR_DATA;
		return FALSE;
	}

	ZeroMemory(m_pNext, 4);
	m_pNext = m_pView;

	while ((++i < MAX_RETRIES) && !SendMessageTimeout(
			m_FDSConnectionWnd,
			m_FDSCallMessage,
			m_Atom,
			0,
			SMTO_BLOCK,
			2000,
			&error))
		Sleep(100);

	if (i >= MAX_RETRIES) {	
		m_FDSResultMessage = GetLastError() == 0 ? FDS_ERROR_TIMEOUT : FDS_ERROR_SENDMESSSAGE;
		return FALSE;
	}
	if (error != FDS_RETURN_SUCCESS) {	
		m_FDSResultMessage = FDS_ERROR_DATA;
		return FALSE;
	}

	pdw = (DWORD*)m_pView;

	while (*pdw) {
		switch (*pdw) {
			case FDS_ACTION_READ:
			case FDS_ACTION_READTRUE:
				pHdrR = (FDS_ACTION_READ_HDR*)pdw;
				m_pNext += sizeof(FDS_ACTION_READ_HDR);
				if (pHdrR->data && pHdrR->size)
					CopyMemory(pHdrR->data, m_pNext, pHdrR->size);
				m_pNext += pHdrR->size;
				pdw = (DWORD*)m_pNext;
				break;
			case FDS_ACTION_WRITE:
			case FDS_ACTION_WRITETRUE:
				pHdrW = (FDS_ACTION_WRITE_HDR*)pdw;
				m_pNext += sizeof(FDS_ACTION_WRITE_HDR) + pHdrW->size;
				pdw = (DWORD*)m_pNext;
				break;
			case FDS_ACTION_READTOKEN:
				pHdrRT = (FDS_ACTION_READTOKEN_HDR*)pdw;
				m_pNext += sizeof(FDS_ACTION_READTOKEN_HDR);
				if (pHdrRT->data)
					CopyMemory(pHdrRT->data, m_pNext, 8);
				m_pNext += 8;
				pdw = (DWORD*)m_pNext;
				break;
			case FDS_ACTION_WRITETOKEN:
				pHdrWT = (FDS_ACTION_WRITETOKEN_HDR*)pdw;
				m_pNext += sizeof(FDS_ACTION_WRITETOKEN_HDR) + 8;
				pdw = (DWORD*)m_pNext;
				break;
			case FDS_ACTION_SEARCH:
				pHdrS = (FDS_ACTION_SEARCH_HDR*)pdw;
				m_pNext += sizeof(FDS_ACTION_SEARCH_HDR) + pHdrS->size;
				pdw = (DWORD*)m_pNext;
				break;
			default:
				*pdw = 0;
				break;
		}
	}

	m_pNext = m_pView;

	m_FDSResultMessage = FDS_OK;
	return TRUE;
}

BOOL CFDSConnection::Search(DWORD offset_from, DWORD offset_to, DWORD size, void *data)
{
	FDS_ACTION_SEARCH_HDR* pHdr = (FDS_ACTION_SEARCH_HDR*) m_pNext;

	if (!m_pView) {	
		m_FDSResultMessage = FDS_ERROR_CONNECTION;
		return FALSE;
	}

	if (((m_pNext - m_pView) + 4 + (size + sizeof(FDS_ACTION_SEARCH_HDR))) > MAX_SIZE) {	
		m_FDSResultMessage = FDS_ERROR_MAXSIZEREACHED;
		return FALSE;
	}

	pHdr->action = FDS_ACTION_SEARCH;
	pHdr->offset_from = offset_from;
	pHdr->offset_to = offset_to;
	pHdr->size = size;

	if (size) CopyMemory(&m_pNext[sizeof(FDS_ACTION_SEARCH_HDR)], data, size);

	m_pNext += sizeof(FDS_ACTION_SEARCH_HDR) + size;

	m_FDSResultMessage = FDS_OK;
	return TRUE;
}


DWORD CFDSConnection::GetResultMessage()
{
	return m_FDSResultMessage;
}