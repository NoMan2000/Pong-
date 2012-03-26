#include "StdAfx.h"
#include "GameApplication.h"


GameApplication* GameApplication::pApp  = NULL;


GameApplication::GameApplication()
{
	assert( pApp == 0 );
	pApp = this;

	appWindow = 0;
	appInstance = 0;
}

GameApplication::~GameApplication()
{
	pApp = 0;
	appWindow = 0;
	appInstance = 0;
}

int GameApplication::MemberWinMain(HINSTANCE hInstance, int showFlag)
{
	appInstance = hInstance;

	if ( ! AppPreBegin() )
		return 0;

	if ( !InitWindow ( showFlag ) )
		return 0;

	if ( !AppBegin () )
		return 0;

	MSG msg;
	bool done = false;

	while ( ! done )
	{
		if ( !AppUpdate () )
			done = true;

		while ( PeekMessage ( &msg, NULL, 0, 0, PM_NOREMOVE ) )
		{
			if ( GetMessage ( &msg, NULL, 0, 0 ) > 0 )
				DispatchMessage ( &msg );
			else
				done = true;
		}
	}

	AppEnd();

	return msg.wParam;
}

bool GameApplication::InitWindow(int showFlag)
{
	const TCHAR *CLASSNAME = TEXT ( "EIAPPCLASS" );

	WNDCLASS wc;
	ZeroMemory( &wc, sizeof(wc) );
	wc.style         = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc   = WndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = GetAppInstance ();
	wc.hCursor       = LoadCursor( NULL, IDC_ARROW );
	wc.hbrBackground = GetBackgroundBrush();
	wc.lpszClassName = CLASSNAME;

	ATOM rc = RegisterClass ( &wc );
	assert(rc);

	long WinStyle, WinStyleEx;

	WinStyleEx = 0;
	WinStyle   = WS_MINIMIZEBOX | WS_POPUPWINDOW | WS_CAPTION;

	
	RECT rect;

	rect.left   = rect.top = 0;

	long w, h;
	GetWindowDims( w, h );
	rect.right  = w;
	rect.bottom = h;

	AdjustWindowRectEx ( &rect, WinStyle, false, WinStyleEx );

	long width  = rect.right  - rect.left;
	long height = rect.bottom - rect.top;

	appWindow = CreateWindowEx (
				WinStyleEx,
				CLASSNAME,
				GetTitle(),
				WinStyle,
				0,
				0,
				width,
				height,
				NULL,
				0,
				GetAppInstance (),
				NULL );

	if ( !appWindow )
		return false;

	long left = ( GetSystemMetrics ( SM_CXSCREEN ) - width  ) / 2;
	long top  = ( GetSystemMetrics ( SM_CYSCREEN ) - height ) / 2;

	MoveWindow ( appWindow, left, top, width, height, TRUE );

	ShowWindow   ( appWindow, showFlag );
	UpdateWindow ( appWindow );

	return true;
}

HINSTANCE GameApplication::GetAppInstance()
{
	return appInstance;
}

HWND GameApplication::GetAppWindow()
{
	return appWindow;
}


LRESULT CALLBACK GameApplication::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (hwnd!=GameApplication::pApp->GetAppWindow())
		return  DefWindowProc ( hwnd, msg, wParam, lParam );

	if ( GameApplication::pApp == NULL )
		return DefWindowProc ( hwnd, msg, wParam, lParam );

	POINT pt;
	LONG delta;
	USHORT active, minimized;

	switch ( msg )
	{
	case WM_PAINT:
		if ( !GameApplication::pApp->Paint( hwnd, wParam, lParam ) )
			if ( !GameApplication::pApp->ProcessMessage ( msg, wParam, lParam ) )
				return DefWindowProc ( hwnd, msg, wParam, lParam );
		break;
	case WM_KEYDOWN:
		if ( !GameApplication::pApp->KeyDown ( wParam, lParam ) )
			if ( !GameApplication::pApp->ProcessMessage ( msg, wParam, lParam ) )
				return DefWindowProc ( hwnd, msg, wParam, lParam ); 
		break;
	case WM_KEYUP:
		if ( !GameApplication::pApp->KeyUp ( wParam, lParam ) )
			if ( !GameApplication::pApp->ProcessMessage ( msg, wParam, lParam ) )
				return DefWindowProc ( hwnd, msg, wParam, lParam ); 
		break;
	case WM_MOUSEMOVE:
		pt.x = ( short ) LOWORD ( lParam );
		pt.y = ( short ) HIWORD ( lParam );
		if ( !GameApplication::pApp->MouseMove ( pt, LOWORD( wParam) ) )
			if ( !GameApplication::pApp->ProcessMessage ( msg, wParam, lParam ) )
				return DefWindowProc ( hwnd, msg, wParam, lParam );
		break;
	case WM_MOUSEWHEEL:
		delta = ( short ) HIWORD ( wParam );
		pt.x   = ( short ) LOWORD ( lParam );
		pt.y   = ( short ) HIWORD ( lParam );
		if ( !GameApplication::pApp->MouseWheelMove ( pt, LOWORD ( wParam ), delta ) )
			if ( !GameApplication::pApp->ProcessMessage ( msg, wParam, lParam ) )
				return DefWindowProc ( hwnd, msg, wParam, lParam );
		break;
	case WM_LBUTTONDOWN:
		pt.x   = ( short ) LOWORD ( lParam );
		pt.y   = ( short ) HIWORD ( lParam );
		if ( !GameApplication::pApp->MouseButtonDown ( pt, LOWORD ( wParam ), 0 ) )
			if ( !GameApplication::pApp->ProcessMessage ( msg, wParam, lParam ) )
				return DefWindowProc ( hwnd, msg, wParam, lParam ); 
		break;
	case WM_LBUTTONDBLCLK:
		pt.x   = ( short ) LOWORD ( lParam );
		pt.y   = ( short ) HIWORD ( lParam );
		if ( !GameApplication::pApp->MouseDoubleClick ( pt, LOWORD ( wParam ), 0 ) )
			if ( !GameApplication::pApp->ProcessMessage ( msg, wParam, lParam ) )
				return DefWindowProc ( hwnd, msg, wParam, lParam ); 
		break;
	case WM_LBUTTONUP:
		pt.x   = ( short ) LOWORD ( lParam );
		pt.y   = ( short ) HIWORD ( lParam );
		if ( !GameApplication::pApp->MouseButtonUp ( pt, LOWORD ( wParam ), 0 ) )
			if ( !GameApplication::pApp->ProcessMessage ( msg, wParam, lParam ) )
				return DefWindowProc ( hwnd, msg, wParam, lParam ); 
		break;
	case WM_RBUTTONDOWN:
		pt.x   = ( short ) LOWORD ( lParam );
		pt.y   = ( short ) HIWORD ( lParam );
		if ( !GameApplication::pApp->MouseButtonDown ( pt, LOWORD ( wParam ), 1 ) )
			if ( !GameApplication::pApp->ProcessMessage ( msg, wParam, lParam ) )
				return DefWindowProc ( hwnd, msg, wParam, lParam ); 
		break;
	case WM_RBUTTONDBLCLK:
		pt.x   = ( short ) LOWORD ( lParam );
		pt.y   = ( short ) HIWORD ( lParam );
		if ( !GameApplication::pApp->MouseDoubleClick ( pt, LOWORD ( wParam ), 1 ) )
			if ( !GameApplication::pApp->ProcessMessage ( msg, wParam, lParam ) )
				return DefWindowProc ( hwnd, msg, wParam, lParam ); 
		break;
	case WM_RBUTTONUP:
		pt.x   = ( short ) LOWORD ( lParam );
		pt.y   = ( short ) HIWORD ( lParam );
		if ( !GameApplication::pApp->MouseButtonUp ( pt, LOWORD ( wParam ), 1 ) )
			if ( !GameApplication::pApp->ProcessMessage ( msg, wParam, lParam ) )
				return DefWindowProc ( hwnd, msg, wParam, lParam ); 
		break;
	case WM_MBUTTONDOWN:
		pt.x   = ( short ) LOWORD ( lParam );
		pt.y   = ( short ) HIWORD ( lParam );
		if ( !GameApplication::pApp->MouseButtonDown ( pt, LOWORD ( wParam ), 2 ) )
			if ( !GameApplication::pApp->ProcessMessage ( msg, wParam, lParam ) )
				return DefWindowProc ( hwnd, msg, wParam, lParam ); 
		break;
	case WM_MBUTTONDBLCLK:
		pt.x   = ( short ) LOWORD ( lParam );
		pt.y   = ( short ) HIWORD ( lParam );
		if ( !GameApplication::pApp->MouseDoubleClick ( pt, LOWORD ( wParam ), 2 ) )
			if ( !GameApplication::pApp->ProcessMessage ( msg, wParam, lParam ) )
				return DefWindowProc ( hwnd, msg, wParam, lParam ); 
		break;
	case WM_MBUTTONUP:
		pt.x   = ( short ) LOWORD ( lParam );
		pt.y   = ( short ) HIWORD ( lParam );
		if ( !GameApplication::pApp->MouseButtonUp ( pt, LOWORD ( wParam ), 2 ) )
			if ( !GameApplication::pApp->ProcessMessage ( msg, wParam, lParam ) )
				return DefWindowProc ( hwnd, msg, wParam, lParam ); 
		break;
	case WM_DESTROY:
			if ( !GameApplication::pApp->ProcessMessage ( msg, wParam, lParam ) )
				PostQuitMessage( 0 );
			break;
	case WM_ACTIVATE:
		active    = LOWORD ( wParam );
		minimized = HIWORD ( wParam );
		if ( ( active == WA_ACTIVE ) || ( active == WA_CLICKACTIVE ) )
		{
			if ( minimized ) // Needed???
				ShowWindow ( hwnd, SW_RESTORE );
		if ( !GameApplication::pApp->AppActivate () )
			if ( !GameApplication::pApp->ProcessMessage ( msg, wParam, lParam ) )
				return DefWindowProc ( hwnd, msg, wParam, lParam ); 
		}
		else
		{
			if ( !GameApplication::pApp->AppDeactivate () )
				if ( !GameApplication::pApp->ProcessMessage ( msg, wParam, lParam ) )
					return DefWindowProc ( hwnd, msg, wParam, lParam ); 
		}
		break;
	default: 
		if ( !GameApplication::pApp->ProcessMessage ( msg, wParam, lParam ) )
			return DefWindowProc ( hwnd, msg, wParam, lParam );
	}
	return 0;
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE, TCHAR*, int showFlag)
{
	// global instance of GameApplication-derived class must be present!
	assert( GameApplication::pApp );

	return GameApplication::pApp->MemberWinMain( hInstance, showFlag );
}



