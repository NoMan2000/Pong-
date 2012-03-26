#pragma once


void eiLogDestroy();
int eiLogNumEntries();
void eiLogToggleVisible();
void CDECL eiLog(LPCTSTR lpszFormat, ...);
void CDECL eiLogClr(DWORD clr, LPCTSTR lpszFormat, ...);


#ifdef EILOG
#define LOG eiLog
#define LOGC eiLogClr
#else
#define LOG  1 ? (void)0 : eiLog
#define LOGC 1 ? (void)0 : eiLogClr
#endif


const int MAX_STRING_LEN = 200;
const int FONT_PITCH = 16;


struct LogEntry
{
	char str[MAX_STRING_LEN];
	int len;
	int id;
	COLORREF clr;
};



void eiLogResetIterator();
bool eiLogNextEntry();
const LogEntry* eiLogEntry();
void eiLogLock();
void eiLogUnlock();


