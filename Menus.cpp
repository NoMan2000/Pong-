#include "stdafx.h"
#include "d3dhelp.h"
#include "menus.h"
#include "pong.h"
#include "constants.h"


const char* Winner = "You Win!";
const char* Loser = "You Lose";
const char* PlayAgain = "Play Again";
const char* Quit = "Quit";
const char* YesText = "Yes";
const char* NoText = "No";


extern PongApp theApp;
extern CountdownMenu countdownMenu;


/////////////////////
// Menu base class //
/////////////////////
//
// basic keyboard event translation
//
/////////////////////

bool MenuBase::HandleInput(UINT_PTR action, DWORD data)
{
	if (action==DIK_UP && (data & 0x80))
	{
		Up();
		return true;
	}
	else if (action==DIK_DOWN && (data & 0x80))
	{
		Down();
		return true;
	}
	else if (action==DIK_RETURN && (data & 0x80))
	{
		Select();
		return true;
	}
	return false;
}



///////////////////
// GameOver Menu //
///////////////////
//
// displays "you win" or "you lose"
// provides two options: "play again" or "quit"
// if "play again" is selected, the countdown menu is launched
//
///////////////////

GameOverMenu::GameOverMenu() 
	:	MenuBase()
{
	SetPlacement( 0.5f, 0.25f );
	AddEntry("Play Again");
	AddEntry("Quit");
}

int GameOverMenu::Push(int humanScore, int aiScore)
{
	if (humanScore > aiScore)
		SetTitle(Winner);
	else
		SetTitle(Loser);


	return eiMenu::Push();
}

int GameOverMenu::Pop()
{
	return eiMenu::Pop();
}

void GameOverMenu::DrawMenu(float x, float y)
{
	CD3DFont* font = theApp.GetBigFont();

	SIZE s;
	font->GetTextExtent( const_cast<char*>(GetTitle()), &s );
	Draw2DText( font, x-(float)s.cx/2.0f, y, 0xffffffff, GetTitle() );

	int cur = GetCurEntry();
	int num = GetNumEntries();

	font = theApp.GetSmallFont();

	for (int i=0; i<num; i++)
	{
		font->GetTextExtent( const_cast<char*>(GetEntry(i)), &s );

		float ex = x-(float)s.cx/2.0f;
		float ey = y + i * 20.f + 60.0f;
		DWORD color = (i==cur) ? 0xffaaffaa : 0xaaaaaaaa;

		Draw2DText( font, ex, ey+(i+1)*20, color, GetEntry(i) );
	}
}

void GameOverMenu::ItemSelected(int index)
{
	if (IsSelected(Quit))
	{
		PostMessage( theApp.GetAppWindow(), WM_CLOSE, 0, 0 );
	}
	else if (IsSelected(PlayAgain))
	{
		Pop();					// remove this menu
		countdownMenu.Push();	// go to countdown menu
	}
}

////////////////////
// Countdown Menu //
////////////////////
//
// displays a countdown before launching the game
//
////////////////////


CountdownMenu::CountdownMenu()
	:	MenuBase()
{
	SetPlacement( 0.6f, 0.43f );
}

int CountdownMenu::Push()
{
	theApp.ResetState();

	lastTime = timeGetTime();
	count = 3;
	SetCount( count );

	return MenuBase::Push();
}

void CountdownMenu::SetCount(int count)
{
	char str[3];
	sprintf( str, "%d", count);
	SetTitle( str );
}

void CountdownMenu::DrawMenu(float x, float y)
{
	CD3DFont* font = theApp.GetBigFont();

	SIZE s;
	font->GetTextExtent( const_cast<char*>(GetTitle()), &s );
	Draw2DText( font, x-(float)s.cx/2.0f, y, 0xffffffff, GetTitle() );

	DWORD timeNow = timeGetTime();
	if (timeNow - lastTime >= 1000)
	{
		count--;
		if (count > 0)
		{
			SetCount( count );
			lastTime = timeNow;
		}
		else
		{
			Pop();	// remove this menu
		}
	}
}

///////////////
// Quit Menu //
///////////////
//
// Asks user if he's sure he wants to quit
//
////////////////////

QuitMenu::QuitMenu() 
	:	MenuBase("Quit?")
{
	SetPlacement( 0.5f, 0.25f );
	AddEntry( YesText );
	AddEntry( NoText );
}

int QuitMenu::Push()
{
	SetCurEntry( 1 );

	return MenuBase::Push();
}

void QuitMenu::DrawMenu(float x, float y)
{
	CD3DFont* font = theApp.GetBigFont();

	SIZE s;
	font->GetTextExtent( const_cast<char*>(GetTitle()), &s );
	Draw2DText( font, x-(float)s.cx/2.0f, y, 0xffffffff, GetTitle() );

	int cur = GetCurEntry();
	int num = GetNumEntries();

	font = theApp.GetSmallFont();

	for (int i=0; i<num; i++)
	{
		font->GetTextExtent( const_cast<char*>(GetEntry(i)), &s );

		float ex = x-(float)s.cx/2.0f;
		float ey = y + i * 20.f + 60.0f;
		DWORD color = (i==cur) ? 0xffaaffaa : 0xaaaaaaaa;

		Draw2DText( font, ex, ey+(i+1)*20, color, GetEntry(i) );
	}
}

void QuitMenu::ItemSelected(int index)
{
	if (IsSelected( YesText ))
	{
		PostMessage( theApp.GetAppWindow(), WM_CLOSE, 0, 0 );
	}
	else if (IsSelected( NoText ))
	{
		Pop();	// remove this menu (return to game)
	}
}
