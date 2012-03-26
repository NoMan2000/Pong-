
#ifndef EISAMPLEAPP_H
#define EISAMPLEAPP_H


#include "D3DApplication.h"
#include "FPS.h"
#include "d3dfile.h"


class eiInputManager;
class eiKeyboard;
class eiPointer;




class GameObject;
class Player;


class PongApp : public D3DApplication
{
public:

	PongApp();
	~PongApp();

	eiKeyboard* GetKeyboard() { assert(keyboard); return keyboard; }
	eiPointer* GetMouse()		{ assert(mouse); return mouse; }
	CD3DFont* GetBigFont()		{ assert(fontBig); return fontBig; }
	CD3DFont* GetSmallFont()	{ assert(fontSmall); return fontSmall; }
	void ResetState();

private:

	// GameApplication overrides
	virtual HBRUSH GetBackgroundBrush()  { return (HBRUSH)GetStockObject(BLACK_BRUSH); }
	virtual LPCSTR GetTitle()					{ return "Pong"; }
	bool AppBegin();
	bool AppUpdate();
	bool AppEnd();
	bool AppActivate()		{ appActive = true;  return true; }
	bool AppDeactivate()	{ appActive = false; return true; }
	bool ProcessMessage ( UINT Message, WPARAM wParam, LPARAM lParam );
	void Paint();

	// D3DApplication overrides
	HRESULT RestoreDeviceObjects();
	HRESULT InitDeviceObjects();
	HRESULT DeleteDeviceObjects();
	HRESULT InvalidateDeviceObjects();

	// helper functions
	void DrawScene(LPDIRECT3DDEVICE8, const D3DVIEWPORT8&);
	void UpdateState();
	void ShowFPS();
	void DrawLog(const D3DVIEWPORT8& viewport);

private:
	bool appActive;

	DWORD lastCycleTime;
	
	D3DModel paddleModel;
	D3DModel pongModel;

	CD3DFont* fontBig;
	CD3DFont* fontSmall;

	eiInputManager* inputMgr;
	eiKeyboard* keyboard;
	eiPointer* mouse;

	GameObject* object[3];
	Player* humanPlayer;
	Player* aiPlayer;

	FpsData fps;

//	int curX, lastX, controllerX;


};


#endif