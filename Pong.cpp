
#include "stdafx.h"
#include "d3dHelp.h"
#include "eiInput.h"
#include "eiLog.h"
#include "menus.h"
#include "GameObjects.h"
#include "Pong.h"
#include "constants.h"



const int TimerFreq = 60;
const int TimerInterval = 1000/TimerFreq;
const int TimerRes = 10;

const char* PaddleModelFile	= "paddle.x";
const char* PongModelFile	= "pong.x";


PongApp theApp;
GameOverMenu gameOverMenu;
CountdownMenu countdownMenu;
QuitMenu quitMenu;


PongApp::PongApp()
	:	D3DApplication(),
		appActive(false),
		fontBig(0),
		fontSmall(0),
		inputMgr(0),
		keyboard(0),
		mouse(0),
		humanPlayer(0),
		aiPlayer(0)
{
	ZeroMemory(object, sizeof(object) );

	fontSmall = new CD3DFont( "arial", 16 );
	fontBig = new CD3DFont( "arial", 36 );

	//UseDepthBuffer( false );
}

PongApp::~PongApp()
{
	delete fontBig;
	delete fontSmall;
}



bool PongApp::AppBegin() 
{
	// initialize Direct3D
	InitializeDirect3D();

	ToggleFullscreen();

	// initialize DirectInput - system keyboard and mouse
	inputMgr = new eiInputManager( GetAppWindow() );

	keyboard = new eiKeyboard();
	keyboard->Attach( 0, 100 );

	mouse = new eiPointer();
	mouse->Attach( 0, 512 );

	// initialize player objects
	ResetState();
	countdownMenu.Push();

	// initialize frames per second object
	fps.Reset();

	return true;
}

void PongApp::ResetState()
{
	for (int i=0; i<3; i++)
	{
		delete object[i];
	}

	humanPlayer = new HumanPlayer( &paddleModel );
	aiPlayer = new ArtificialPlayer( &paddleModel ); 
	Ball* ball = new Ball( &pongModel );

	humanPlayer->SetBall( ball );
	aiPlayer->SetBall( ball );
	ball->SetPlayers( humanPlayer, aiPlayer );

	object[0] = ball;
	object[1] = humanPlayer;
	object[2] = aiPlayer;

	srand( timeGetTime() );
}

void PongApp::UpdateState()
{
	if ( ! appActive)
		return;

	// if a menu is being displayed, pump keyboard input to the menu system
	DIDEVICEOBJECTDATA event;
	while (keyboard->GetEvent( event ))
	{
		if ( ! eiMenu::StackIsEmpty() )
			eiMenu::StackInput( event.dwOfs, event.dwData );
		else
		{
			if (event.dwOfs == DIK_ESCAPE && event.dwData & 0x80)
				quitMenu.Push();
		}
	}

	// update game objects
	if ( eiMenu::StackIsEmpty() )
	{
		for (int i=0; i<3; i++)
		{
			object[i]->Update();
		}
	}

	// check for end-of-game
	int humanScore = humanPlayer->GetScore();
	int aiScore = aiPlayer->GetScore();

	if (humanScore >= WinningScore || aiScore >= WinningScore)
	{
		if (eiMenu::StackIsEmpty())
			gameOverMenu.Push( humanScore, aiScore);
	}
}

void PongApp::DrawScene(LPDIRECT3DDEVICE8 device, const D3DVIEWPORT8& viewport)
{
	for (int i=0; i<3; i++)
	{
		object[i]->Render();
	}
}


bool PongApp::AppUpdate ()
{
	DWORD timeNow = timeGetTime();
	
	if (timeNow >= lastCycleTime + TimerInterval)
	{
		UpdateState();
		lastCycleTime = timeNow;
	}

	LPDIRECT3DDEVICE8 device = Get3DDevice();

	HRESULT hr;
	if( FAILED( hr = device->TestCooperativeLevel() ) )
	{
		if( D3DERR_DEVICELOST == hr )
			return true;

		if( D3DERR_DEVICENOTRESET == hr )
		{
			if( FAILED( hr = Reset3DEnvironment() ) )
				return true;
		}
		return true;
	}

	Clear3DBuffer(D3DCOLOR_RGBA(0, 15, 0, 255));

	if (SUCCEEDED(device->BeginScene()))
	{
		D3DVIEWPORT8 viewport;
		device->GetViewport( &viewport );

		DrawScene( device, viewport );

		ShowFPS();
		DrawLog( viewport );

		eiMenu::StackDraw( viewport.Width, viewport.Height );

		device->EndScene();

		Present3DBuffer();

		fps.NewFrame( timeNow );
	}

	return true;
}

void PongApp::DrawLog(const D3DVIEWPORT8& viewport)
{
	eiLogLock();

	int maxVisible = (viewport.Height - viewport.Y) / FONT_PITCH - 1;
	int numItems = eiLogNumEntries();
	int drawCount = min( maxVisible, numItems );
	int output_y = FONT_PITCH * (drawCount-1);

	eiLogResetIterator();

	for (int i = 0; i < drawCount; i++)
	{
		const LogEntry* p = eiLogEntry();

		DWORD color;
		if (p->clr == 0)
			color = 0xaaaaaaaa;
		else
			color = p->clr;

		Draw2DText( fontSmall, viewport.Width/2, output_y, color, p->str );

		output_y -= FONT_PITCH;
		eiLogNextEntry();
	}

	eiLogUnlock();
}


void PongApp::ShowFPS()
{
	if (fps.Valid())
	{
		SIZE size;
		char fpsStr[20];
		sprintf( fpsStr, "%.2f fps", fps.LastReading());
		fontSmall->GetTextExtent(fpsStr, &size);
		fontSmall->DrawText( 2, 1, 0xffbbbbbb, fpsStr, 0L );
	}
}


bool PongApp::AppEnd ()
{
	for (int i=0; i<3; i++)
		delete object[i];
	
	// release model data
	paddleModel.Release();
	pongModel.Release();
	
	// terminate Direct3D
	ShutdownDirect3D();

	// terminate input (inputmgr will clean up forgotten devices)
	delete keyboard;
	delete mouse;
	delete inputMgr;

	// final log file flush
	eiLogDestroy();

	return true;
}



bool PongApp::ProcessMessage(UINT Message, WPARAM wParam, LPARAM lParam)
{
	switch (Message)
	{
		case WM_SYSCOMMAND:
			// Prevent moving/sizing and power loss
			switch( wParam )
			{
			case SC_MOVE:
			case SC_SIZE:
			case SC_MAXIMIZE:
			case SC_KEYMENU:
			case SC_MONITORPOWER:
			return true;
			}
			break;
		case WM_SYSKEYDOWN:
			if( VK_RETURN == wParam )
			{
				ToggleFullscreen();
				fps.Reset();
			}
			break;
	};

	return false;
}

HRESULT PongApp::InitDeviceObjects()
{
	char errStr[100];

	LPDIRECT3DDEVICE8 device = Get3DDevice();
	ShowCursor( false );

	if (paddleModel.Load( PaddleModelFile, device ) == false)
	{
		sprintf( errStr, "can't find %s", PaddleModelFile );
		MessageBox( GetAppWindow(), errStr, "Play demo", MB_OK );
		return E_FAIL;
	}
	paddleModel.Scale( 0.8f, 0.5f, 0.5f );
	
	if (pongModel.Load( PongModelFile, device ) == false)
	{
		sprintf( errStr, "can't find %s", PongModelFile );
		MessageBox( GetAppWindow(), errStr, "Pong", MB_OK );
		return E_FAIL;
	}

	fontSmall->InitDeviceObjects( device );
	fontBig->InitDeviceObjects( device );

	return S_OK;
}

HRESULT PongApp::RestoreDeviceObjects()
{
	paddleModel.RestoreModel();
	pongModel.RestoreModel();

	fontSmall->RestoreDeviceObjects();
	fontBig->RestoreDeviceObjects();

	const D3DSURFACE_DESC& backBuf = GetBackBufferDesc();
	LPDIRECT3DDEVICE8 device = Get3DDevice();

    device->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    device->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
    device->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    device->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    device->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
	device->SetRenderState( D3DRS_SHADEMODE, D3DSHADE_FLAT );
    device->SetRenderState( D3DRS_DITHERENABLE, TRUE );
    device->SetRenderState( D3DRS_SPECULARENABLE, TRUE );
    device->SetRenderState( D3DRS_AMBIENT,  0x00ffffff );
    device->SetRenderState( D3DRS_ZENABLE,        TRUE );

	D3DXVECTOR3 vEyePt    = D3DXVECTOR3( 0.0f, 0.0f,-1500.0f );
    D3DXVECTOR3 vLookatPt = D3DXVECTOR3( 0.0f, 0.0f,  0.0f );
    D3DXVECTOR3 vUpVec    = D3DXVECTOR3( 0.0f, 1.0f,  0.0f );
    D3DXMATRIX  matWorld, matView, matProj;

    D3DXMatrixIdentity( &matWorld );
    D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );
	FLOAT fAspect = ((FLOAT)backBuf.Width) / backBuf.Height;
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, fAspect, 1.0f, 4000.0f );

    device->SetTransform( D3DTS_WORLD,      &matWorld );
    device->SetTransform( D3DTS_VIEW,       &matView );
    device->SetTransform( D3DTS_PROJECTION, &matProj );
	
    // Set up the lighting states
    D3DLIGHT8 light;
	D3DUtil_InitLight( light, D3DLIGHT_DIRECTIONAL, 0.0f, -1.0f, 1.0f );
	device->SetLight( 0, &light );
    device->LightEnable( 0, TRUE );

    
	device->SetRenderState( D3DRS_LIGHTING, TRUE );
    device->SetRenderState( D3DRS_AMBIENT, 0x44444444 );

	return S_OK;
}

HRESULT PongApp::DeleteDeviceObjects()
{
	paddleModel.Release();
	pongModel.Release();

    fontBig->DeleteDeviceObjects();
    fontSmall->DeleteDeviceObjects();

	return S_OK;
}

HRESULT PongApp::InvalidateDeviceObjects()
{
	paddleModel.InvalidateModel();
	pongModel.InvalidateModel();

    fontBig->InvalidateDeviceObjects();
    fontSmall->InvalidateDeviceObjects();

    return S_OK;
}



