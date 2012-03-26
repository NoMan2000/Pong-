
#pragma once


#include "GameApplication.h"
#include "d3dFile.h"


enum APPMSGTYPE { MSG_NONE, MSGERR_APPMUSTEXIT, MSGWARN_SWITCHEDTOREF };


#define D3DAPPERR_NODIRECT3D          0x82000001
#define D3DAPPERR_NOWINDOW            0x82000002
#define D3DAPPERR_NOCOMPATIBLEDEVICES 0x82000003
#define D3DAPPERR_NOWINDOWABLEDEVICES 0x82000004
#define D3DAPPERR_NOHARDWAREDEVICE    0x82000005
#define D3DAPPERR_HALNOTCOMPATIBLE    0x82000006
#define D3DAPPERR_NOWINDOWEDHAL       0x82000007
#define D3DAPPERR_NODESKTOPHAL        0x82000008
#define D3DAPPERR_NOHALTHISMODE       0x82000009
#define D3DAPPERR_NONZEROREFCOUNT     0x8200000a
#define D3DAPPERR_MEDIANOTFOUND       0x8200000b
#define D3DAPPERR_RESIZEFAILED        0x8200000c



class D3DApplication;


class D3DModel
{
public:
	D3DModel();
	~D3DModel();
	bool Load(LPCTSTR xFileName, LPDIRECT3DDEVICE8 device);
	void SetLocation(float x, float y, float z);
	void Scale(float scale);
	void Scale(float x, float y, float z);
	void Render();
	void RestoreModel();
	void InvalidateModel();
	void Release();
private:
	std::string modelName;
	CD3DFrame frame;
	float x, y, z;
	LPDIRECT3DDEVICE8 device;
};



class D3DApplication : public GameApplication
{
public:
	D3DApplication();
	virtual ~D3DApplication();

protected:

	void UseDepthBuffer(bool d)  { m_bUseDepthBuffer = d; }

	HRESULT InitializeDirect3D();
	HRESULT ShutdownDirect3D();
	
	const D3DSURFACE_DESC& GetBackBufferDesc()  { return m_d3dsdBackBuffer; }
	HRESULT Clear3DBuffer(D3DCOLOR);
	HRESULT Present3DBuffer();
	LPDIRECT3DDEVICE8 Get3DDevice()  { return m_pd3dDevice; }
	HRESULT Reset3DEnvironment();
	HRESULT ToggleFullscreen();
	HRESULT ForceWindowed();

	virtual HRESULT InitDeviceObjects() { return S_OK; }
	virtual HRESULT RestoreDeviceObjects() { return S_OK; }
	virtual HRESULT InvalidateDeviceObjects() { return S_OK; }
	virtual HRESULT DeleteDeviceObjects() { return S_OK; }
	virtual HRESULT ConfirmDevice(D3DCAPS8*, DWORD, D3DFORMAT)   { return S_OK; }

	bool ProcessMessage(UINT msg, WPARAM wParam, LPARAM lParam);

private:

	HRESULT Initialize3DEnvironment();
	static int SortModesCallback( const VOID* arg1, const VOID* arg2 );
	HRESULT BuildDeviceList();
	BOOL FindDepthStencilFormat( UINT iAdapter, D3DDEVTYPE DeviceType,
				D3DFORMAT TargetFormat, D3DFORMAT* pDepthStencilFormat );
	HRESULT DisplayErrorMsg( HRESULT hr, DWORD dwType );
	HRESULT AdjustWindowForChange();


private:
	HWND m_hWnd;
	
	IDirect3D8* m_pD3D;
	LPDIRECT3DDEVICE8 m_pd3dDevice;
	D3DCAPS8 m_d3dCaps;
	D3DSURFACE_DESC m_d3dsdBackBuffer;
	D3DPRESENT_PARAMETERS m_d3dpp;

	RECT	m_rcWindowClient;    // Saved client area size for mode switches
	RECT	m_rcWindowBounds;    // Saved window bounds for mode switches
	DWORD	m_dwCreateFlags;     // Indicate sw or hw vertex processing
	TCHAR	m_strDeviceStats[90];// String to hold D3D device stats
	BOOL	m_bShowCursorWhenFullscreen; // Whether to show cursor when fullscreen
	BOOL	m_bActive;
	BOOL	m_bFrameMoving;
	BOOL	m_bSingleStep;
	DWORD	m_dwMinDepthBits;    // Minimum number of bits needed in depth buffer
	DWORD	m_dwMinStencilBits;  // Minimum number of bits needed in stencil buffer
	DWORD	m_dwWindowStyle;     // Saved window style for mode switches
	BOOL	m_bReady;

	UINT	m_dwNumAdapters;
	DWORD	m_dwAdapter;
	BOOL	m_bWindowed;
	BOOL	m_bUseDepthBuffer;   // Whether to autocreate depthbuffer

	struct D3DModeInfo
	{
		DWORD      Width;      // Screen width in this mode
		DWORD      Height;     // Screen height in this mode
		D3DFORMAT  Format;     // Pixel format in this mode
		DWORD      dwBehavior; // Hardware / Software / Mixed vertex processing
		D3DFORMAT  DepthStencilFormat; // Which depth/stencil format to use with this mode
	};

	struct D3DDeviceInfo
	{
		// Device data
		D3DDEVTYPE   DeviceType;      // Reference, HAL, etc.
		D3DCAPS8     d3dCaps;         // Capabilities of this device
		const TCHAR* strDesc;         // Name of this device
		BOOL         bCanDoWindowed;  // Whether this device can work in windowed mode

		// Modes for this device
		DWORD        dwNumModes;
		D3DModeInfo  modes[150];

		// Current state
		DWORD        dwCurrentMode;
		BOOL         bWindowed;
		D3DMULTISAMPLE_TYPE MultiSampleType;
	};

	struct D3DAdapterInfo
	{
		// Adapter data
		D3DADAPTER_IDENTIFIER8 d3dAdapterIdentifier;
		D3DDISPLAYMODE d3ddmDesktop;      // Desktop display mode for this adapter

		// Devices for this adapter
		DWORD          dwNumDevices;
		D3DDeviceInfo  devices[5];

		// Current state
		DWORD          dwCurrentDevice;
	};

	D3DAdapterInfo m_Adapters[10];
};



