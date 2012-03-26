#include "stdafx.h"


#pragma comment (lib, "winmm.lib")

#pragma comment (lib,"dinput8.lib")

#pragma comment (lib,"dplayx.lib")

#pragma comment (lib, "d3d8.lib")
#pragma comment (lib, "d3dx8.lib")
#pragma comment (lib, "d3dxof.lib")

#pragma comment (lib, "dxguid.lib")
#pragma comment (lib, "dxerr8.lib")

#pragma comment (lib, "eisdk.lib")



void ReportDirectXError(const char* title, HRESULT r, const char* file, int line, const char* str )
{
	char tstr[1024];
	char* fail = FAILED(r) ? "failure" : "(no failure)";
	const char* hrStr = DXGetErrorString8( r );
	sprintf( tstr, "******** %s %s ********\n%s\nreturn code: '%s'\n%s (line %d)\n********************************\n", title, fail, str, hrStr, file, line );
	OutputDebugString( tstr );

	if (FAILED(r))
	{
		__asm { int 3 }
	}
}
