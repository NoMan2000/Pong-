
#ifndef WM_MOUSEWHEEL
#define WM_MOUSEWHEEL 0x020A
#endif



class GameApplication
{
public:
	GameApplication();
	virtual ~GameApplication();

	HINSTANCE GetAppInstance();
	HWND GetAppWindow();

protected:

	virtual HBRUSH GetBackgroundBrush()  { return (HBRUSH)(COLOR_WINDOW+1); }
	virtual void GetWindowDims(long& w, long& h )		 { w = 640; h = 480; }
	virtual LPCSTR GetTitle()					{ return "title"; }

	virtual bool AppPreBegin()  { return true; }
	virtual bool AppBegin()		{ return true; }
	virtual bool AppUpdate()	{ return true; }
	virtual bool AppEnd()		{ return true; }

	virtual bool AppActivate()	{ return false; }
	virtual bool AppDeactivate(){ return false; }

	virtual bool Paint(HWND, WPARAM, LPARAM ) { return false; }

	virtual bool MouseMove( POINT &pt, long keys )						{ return false; }
	virtual bool MouseWheelMove( POINT &pt, long keys, long WDelta )	{ return false; }
	virtual bool MouseButtonDown( POINT &pt, long keys, long button )	{ return false; }
	virtual bool MouseButtonUp( POINT &pt, long keys, long button )		{ return false; }
	virtual bool MouseDoubleClick( POINT &pt, long keys, long button )  { return false; }

	virtual bool KeyDown( long key, long data )	{ return false; }
	virtual bool KeyUp( long key, long data )	{ return false; }

	virtual bool ProcessMessage( UINT Msg, WPARAM wParam, LPARAM lParam ) {	 return false; }

private:	// implementation functions

	bool InitWindow( int showFlag );
	int MemberWinMain( HINSTANCE inst, int showFlag );
	friend int APIENTRY WinMain( HINSTANCE, HINSTANCE, TCHAR *, int );
	static LRESULT CALLBACK WndProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam );

private:	// data members

	static GameApplication* pApp;
	HINSTANCE  appInstance;
	HWND appWindow;
};


