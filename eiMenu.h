
#pragma once


const int MENU_MAX_ENTRIES = 20;
const int MENU_MAX_NAME_LEN = 80;

class eiMenu;


#include <list>



class eiLayer
{
public:

	static bool StackIsEmpty();
	static int StackDraw(DWORD width, DWORD height);
	static bool StackUpdate();
	static bool StackInput(UINT_PTR, DWORD);
	static int StackPopAll();

	void SetPlacement(float x, float y)  { xPos=x; yPos=y; }

	virtual int Push();
	virtual int Pop();

	const char* GetTitle()	{ return title.data(); }
	void SetTitle(LPCTSTR t)  { title = t; }

protected:

	eiLayer(const char* name);
	eiLayer();

	virtual ~eiLayer();

	virtual void DrawMenu(float x, float y) = 0;
	virtual bool HandleInput(UINT_PTR, DWORD) = 0;
	virtual void Update()  { }

private:

	std::string title;
	float xPos, yPos;

	typedef std::list<eiLayer*> LayerList;

	static LayerList list;
	static CRITICAL_SECTION critSect;
};


class eiMenu : public eiLayer
{
protected:

	eiMenu(const char* n);
	eiMenu();
	~eiMenu();

public:

	int AddEntry(const char*);
	bool SetData(int index, void* d, size_t size = 0);
	void* GetData(int index);
	int AddDivider();
	int ClearEntries();

	int GetNumEntries()		{ return entryCount; }
	const char* GetEntry(int index);
	int GetCurEntry()		{ return curEntry; }
	bool IsLastEntry(int index) const		{ return entryCount == index; }
	bool IsSelected(const char* menuText)   { return strcmp( menuText, entry[curEntry].string ) == 0; }
	void SetCurEntry(int index)		{ assert(index>=0 && index<entryCount); curEntry=index; }

protected:

	void Up();
	void Down();
	void Select()			{ ItemSelected(curEntry); }
	virtual void ItemSelected(int index)   { }

private:

	struct Entry
	{
		bool divider;
		char string[100];
		void* data;
		int dataSize;
	};

	Entry entry[MENU_MAX_ENTRIES];
	int entryCount;
	int curEntry;

};

