#pragma once


void ReportLastError();
void ReportDirectXError(const char* title, HRESULT r, const char* file, int line, const char* str );



#ifdef _DEBUG

#define DXVERIFY(exp) \
	do \
	{ \
		HRESULT hr = exp; \
		if (hr != DPN_OK) \
			{ \
				ReportDirectXError( "DX", hr, __FILE__, __LINE__, #exp ); \
			} \
	} while (0) \

#else

#define DXVERIFY(exp)           ((void)(exp))

#endif





