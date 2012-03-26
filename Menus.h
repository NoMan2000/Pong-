#pragma once


#include "eiMenu.h"


class MenuBase : public eiMenu
{
public:
	MenuBase() : eiMenu()
	{
	}
	MenuBase(LPCTSTR title) : eiMenu(title)
	{
	}
private:
	bool HandleInput(UINT_PTR action, DWORD data);
};


class GameOverMenu : public MenuBase
{
public:
	GameOverMenu();
	int Push(int humanScore, int aiScore);
	int Pop();
	void DrawMenu(float x, float y);
	void ItemSelected(int index);
};

class CountdownMenu : public MenuBase
{
public:
	CountdownMenu();
	int Push();
	void DrawMenu(float x, float y);
	void SetCount(int count);
private:
	DWORD lastTime;
	int count;
};


class QuitMenu : public MenuBase
{
public:
	QuitMenu();
	int Push();
	void DrawMenu(float x, float y);
	void ItemSelected(int index);
};