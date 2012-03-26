#pragma once


#pragma warning( disable : 4100 )	// unreference formal parameter
#pragma warning( disable : 4201 )	// nameless unions

// Win32
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>

// standard libs
#include <assert.h>
#include <stdio.h>
#include <string.h>

// STL
#pragma warning( disable : 4786 )  // disable browser symbol truncation warning (stl templates)
#include <list>

// DirectX
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <ddraw.h>
#include <d3d8.h>
#include <d3dx8.h>
#include <DXerr8.h>

// multimedia
#include <mmsystem.h>

// DX SDK
#include "D3DFont.h"
#include "D3DUtil.h"
#include "dxUtil.h"

// EISDK
#define TRACE DXUtil_Trace



#define EILOG
#include "eiLog.h"




void ReportDirectXError(const char* title, HRESULT r, const char* file, int line, const char* str );


