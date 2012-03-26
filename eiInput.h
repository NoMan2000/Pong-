#pragma once


#include <dinput.h>
#include <d3d8.h>
#include <list>
#include "eiUtil.h"



#ifdef _DEBUG


#define DIVERIFY(exp) \
	do \
	{ \
		HRESULT hr = exp; \
		if (hr != DI_OK) \
			{ \
				ReportDirectXError( "DI", hr, __FILE__, __LINE__, #exp ); \
			} \
	} while (0) \

#else

#define DIVERIFY(exp)           ((void)(exp))

#endif

const char* GetDeviceTypeString(DWORD type);
int GetDeviceFlagStrings(DWORD flags, char**);
const char* DikGetName(DWORD dik, bool complete = false);

typedef DIJOYSTATE GAMECTRLSTATE;



enum HatState
{
	HS_CENTERED,
	HS_NORTH,
	HS_NORTHEAST,
	HS_EAST,
	HS_SOUTHEAST,
	HS_SOUTH,
	HS_SOUTHWEST,
	HS_WEST,
	HS_NORTHWEST,
};



// OBJECTITERATOR

typedef std::list<DIDEVICEOBJECTINSTANCE> ObjectList;


class eiInputObjectIterator : public ObjectList::iterator
{
	friend class eiInputDevice;
private:
	eiInputObjectIterator::eiInputObjectIterator(ObjectList& list)
		:	ObjectList::iterator(list.begin()),
		deviceList(list)  { }
public:
	void First()		{ *static_cast<ObjectList::iterator*>(this) = deviceList.begin(); }
	bool Begin()	{ return *this == deviceList.begin(); }
	bool End()		{ return *this == deviceList.end(); }

	const char* GetName()	{ return operator*().tszName; }
	const GUID* GetID()	{ return &(operator*()).guidType; }
	DWORD GetType()    { return operator*().dwType; }

private:
	ObjectList& deviceList;
};


/////////////////
// INPUTDEVICE //
/////////////////

class eiInputDevice
{
	friend class eiInputManager;
public:
	static eiInputManager* inputMgr;
	operator LPDIRECTINPUTDEVICE8()			{ return device; }
	LPDIRECTINPUTDEVICE8 GetDXinterface()	{ return device; }

	DWORD GetDeviceTypeRaw()	{ return caps.dwDevType; }
	DWORD GetDeviceType()		{ return GET_DIDEVICE_TYPE(caps.dwDevType); }
	DWORD GetDeviceSubType()	{ return GET_DIDEVICE_SUBTYPE(caps.dwDevType); }
	DWORD GetCapsFlags()		{ return caps.dwFlags; }
	DWORD GetNumButtons()		{ return caps.dwButtons; }
	DWORD GetNumAxes()			{ return caps.dwAxes; }
	DWORD GetNumHats()			{ return caps.dwPOVs; }
	DWORD GetNumPovs()			{ return caps.dwPOVs; }
	DWORD GetFFSamplePeriod()	{ return caps.dwFFSamplePeriod; }
	DWORD GetFFMinTimeResolution() { return caps.dwFFMinTimeResolution; }
	DWORD GetFirmwareRevision()	{ return caps.dwFirmwareRevision; }
	DWORD GetHardwareRevision()	{ return caps.dwHardwareRevision; }
	DWORD GetFFDriverVersion()	{ return caps.dwFFDriverVersion; }

	bool IsPolledDevice()		{ return (caps.dwFlags & DIDC_POLLEDDEVICE) != 0; }
	bool IsForceFeedback()		{ return (caps.dwFlags & DIDC_FORCEFEEDBACK) != 0; }
	bool IsAcquired();

	GUID* GetInstanceGuid()		{ return &inst.guidInstance; }
	GUID* GetProductGuid()		{ return &inst.guidProduct; }
	GUID* GetFFDriverGuid()		{ return &inst.guidFFDriver; }
	DWORD GetDeviceTypeInst()	{ return inst.dwDevType; }
	const char* GetInstanceName()	{ return inst.tszInstanceName; }
	const char* GetProductName()	{ return inst.tszProductName; }
	WORD GetUsagePage()			{ return inst.wUsagePage; }
	WORD GetUsage()				{ return inst.wUsage; }


	HRESULT SetAxisMinMax(long min, long max);

	virtual bool RefreshState();  // retrieve device state
	virtual bool DeliverEvents(); // retrieve (and deliver) device events

	bool EnumObjects(DWORD enumFlags = DIDFT_ALL);
	eiInputObjectIterator GetObjectIterator();

	DWORD GetEventCount();
	bool PeekEvent(DIDEVICEOBJECTDATA&);
	bool GetEvent(DIDEVICEOBJECTDATA&);
	void FlushEvents();


	void Unacquire();
	void ReleaseDevice();

	bool UpdateActionMap(DIACTIONFORMAT*);

	virtual ~eiInputDevice();
protected:
	eiInputDevice(void*, DWORD); // force use by derivation with protected constructor
	
	
	virtual HRESULT Attach(const GUID* pGuid = 0, DWORD bufSize = 0, DIACTIONFORMAT* format = 0) = 0;
	virtual void InspectMappings(DIACTIONFORMAT*) { }

	const void* GetStateBuffer()		{ return stateData; }
	const void* GetPrevStateBuffer()	{ return oldStateData; }

	void SetBufferSize(DWORD);
	DWORD GetBufferSize();

	void GetDeviceCaps();

	bool SetActionMapping(bool am)  { bool r = actionMapped; actionMapped = am; return r; }
	bool IsActionMapped()			{ return actionMapped; }

private:
	virtual void DispatchEvent(const DIDEVICEOBJECTDATA&) = 0;
	virtual void MappedEvent(const DIDEVICEOBJECTDATA&)  { }

	static BOOL CALLBACK EnumObjectsCallback(LPCDIDEVICEOBJECTINSTANCE object, void* p);

protected:
	LPDIRECTINPUTDEVICE8 device;
private:

	DIDEVICEINSTANCE inst;
	DIDEVCAPS caps;

	void* stateData;
	void* oldStateData;
	DWORD stateDataSize;	

	bool actionMapped;

	ObjectList objectList;

};

//////////////
// KEYBOARD //
//////////////

class eiKeyboard : public eiInputDevice
{
	friend class eiInputManager;
public:
	eiKeyboard();
	HRESULT Attach(const GUID* guid = 0, DWORD bufSize = 0, DIACTIONFORMAT* format = 0);

	// polling-based device state query functions
	bool KeyIsDown(int dik_code);
	BYTE GetKeyState(int dik_code);

private:
	// buffered event notification override-ables
	virtual void KeyDown(DWORD) { };
	virtual void KeyUp(DWORD)  { };

	void DispatchEvent(const DIDEVICEOBJECTDATA&);
private:

	BYTE keyArray[256];
};

/////////////////////
// POINTER (mouse) //
/////////////////////

class eiPointer : public eiInputDevice
{
	friend class eiInputManager;
public:
	// mouse init takes two steps: the contructor, and call to Attach()...
	eiPointer();
	HRESULT Attach(const GUID* pGuid=0, DWORD bufferCount=0, DIACTIONFORMAT* format = 0);

	// polling-based device state query functions
	BYTE GetButtonState(int button);
	bool ButtonIsDown(int button);
	long GetX()  { return pointerState.lX; }
	long GetY()  { return pointerState.lY; }
	void GetMousePos(long& x, long& y)  { x=pointerState.lX; y=pointerState.lY; }
	long GetScrollState()  { return pointerState.lZ; }
	long GetZ()  { return pointerState.lZ; }

private:
	// buffered event notification override-ables
	virtual void ButtonDown(DWORD) { }
	virtual void ButtonUp(DWORD) { }
	virtual void AxisX(DWORD) { }
	virtual void AxisY(DWORD) { }
	virtual void AxisZ(DWORD) { }

	void DispatchEvent(const DIDEVICEOBJECTDATA&);
private:
	DIMOUSESTATE2 pointerState;
};

/////////////////////
// GAME CONTROLLER //
/////////////////////

class eiGameController : public eiInputDevice
{
	friend class eiInputManager;
public:
	// joystick init takes two steps: the contructor, and call to Attach()...
	eiGameController();
	HRESULT Attach(const GUID* pGuid=0, DWORD bufferCount=0, DIACTIONFORMAT* format = 0);

	// overridden for event emulation on polled devices
	virtual bool DeliverEvents();

	// polling-based device state query functions
	bool ButtonIsDown(int button);
	BYTE GetButtonState(int button);
	long GetX()  { return controllerState.lX; }
	long GetY()  { return controllerState.lY; }
	long GetZ()  { return controllerState.lZ; }
	long GetXR()  { return controllerState.lRx; }
	long GetYR()  { return controllerState.lRy; }
	long GetZR()  { return controllerState.lRz; }
	HatState GetHatState(int hat);
	DWORD GetHatSwitchState(int hat);

private:
	// buffered event notification override-ables
	virtual void ButtonDown(DWORD) { }
	virtual void ButtonUp(DWORD) { }
	virtual void AxisX(DWORD) { }
	virtual void AxisY(DWORD) { }
	virtual void AxisZ(DWORD) { }
	virtual void AxisRX(DWORD) { }
	virtual void AxisRY(DWORD) { }
	virtual void AxisRZ(DWORD) { }
	virtual void Slider(int s, DWORD) { }
	virtual void Hat(int h, HatState) { }

	void DispatchEvent(const DIDEVICEOBJECTDATA&);
private:
	GAMECTRLSTATE controllerState;
};


////////////////////
// DEVICEITERATOR // 
////////////////////


typedef std::list<DIDEVICEINSTANCE> DeviceDescList;
typedef std::list<eiInputDevice*> ActiveDeviceList;


class eiInputDeviceIterator : public DeviceDescList::iterator
{
	friend class eiInputManager;

private:

	eiInputDeviceIterator::eiInputDeviceIterator(DeviceDescList& list)
		:	DeviceDescList::iterator(list.begin()),
		deviceDescList(list)  { }

public:

	void First()	{ *static_cast<DeviceDescList::iterator*>(this) = deviceDescList.begin(); }
	void Reset()	{ *static_cast<DeviceDescList::iterator*>(this) = deviceDescList.begin(); }
	bool Begin()	{ return *this == deviceDescList.begin(); }
	bool End()		{ return *this == deviceDescList.end(); }

	const char* GetDeviceName()		{ return operator*().tszInstanceName; }
	const char* GetProductName()	{ return operator*().tszProductName; }
	const GUID* GetDeviceID()		{ return &(operator*()).guidInstance; }
	DWORD GetDeviceType()			{ return operator*().dwDevType; }
	operator const GUID*()			{ return &(operator*().guidInstance); }

	bool IsKeyboard()	{ return (GET_DIDEVICE_TYPE( (operator*()).dwDevType) == DI8DEVTYPE_KEYBOARD); }	
	bool IsMouse()	{ return (GET_DIDEVICE_TYPE( (operator*()).dwDevType) == DI8DEVTYPE_MOUSE); }	
	bool IsPointer()	{ return (GET_DIDEVICE_TYPE( (operator*()).dwDevType) == DI8DEVTYPE_MOUSE); }	
	bool IsJoystick()	{ return (GET_DIDEVICE_TYPE( (operator*()).dwDevType) > DI8DEVTYPE_KEYBOARD); }
	bool IsGameController()	{ return (GET_DIDEVICE_TYPE( (operator*()).dwDevType) > DI8DEVTYPE_KEYBOARD); }

private:

	DeviceDescList& deviceDescList;
};

//////////////////
// INPUTMANAGER // 
//////////////////

LPDIRECTINPUT8 GetDIinterface();


class eiInputManager
{
	friend LPDIRECTINPUT8 GetDIinterface();
	friend class eiInputDevice;
	friend class eiKeyboard;
	friend class eiPointer;
	friend class eiGameController;
	friend class eiGameControllerFF;

public:
	eiInputManager(HWND);
	virtual ~eiInputManager();
	void Terminate();
	HWND GetHwnd() { return hwnd; }
	LPDIRECTINPUT8 GetDirectInput()  { return directInput; }

	void EnumDevices(DWORD devType = DI8DEVCLASS_ALL, DWORD enumFlags = DIEDFL_ATTACHEDONLY);
	void EnumDevicesWithMapping(const char* userName, LPDIACTIONFORMAT, DWORD enumFlags = 0/*DIEDFL_ATTACHEDONLY*/);

	void SetActionMap(DIACTIONFORMAT*);

	eiInputDeviceIterator GetDeviceIterator();

	void DeliverEvents();

	void AddDevice(eiInputDevice*);
	void DelDevice(eiInputDevice*);

	HRESULT ShowConfig(DIACTION*, int, DIACTIONFORMAT*, DWORD flags);

	void UnacquireDevices();

private:
	static BOOL CALLBACK EnumDeviceCallback(const DIDEVICEINSTANCE* pDesc, void* p );
	static BOOL CALLBACK EnumDeviceMappingCallback(const DIDEVICEINSTANCE* desc, LPDIRECTINPUTDEVICE8 dev, DWORD flags, DWORD remaining, void* pvRef);
	static BOOL CALLBACK EnumDeviceReAttachCallback(const DIDEVICEINSTANCE* deviceDesc, LPDIRECTINPUTDEVICE8 dev, DWORD flags, DWORD remaining, void* p);

private:
	LPDIRECTINPUT8 directInput;

	HWND hwnd;
	DeviceDescList deviceDescList;
	ActiveDeviceList activeDeviceList;

	DIACTIONFORMAT* actionFormat;


	bool cleanUp;
};




