#pragma once
#include <dinput.h>

class InputManager
{
private:
	IDirectInput8*	m_direct_input;
	IDirectInputDevice8* m_keyboard_device;
	IDirectInputDevice8* m_mouse_device;
	HINSTANCE*	m_hInst;
	HWND*		m_hWnd;
	unsigned char m_keyboard_keys_state[256];
	unsigned char m_previous_keyboard_keys_state[256];
	DIMOUSESTATE m_mouse_state;

public:
	InputManager(HINSTANCE* g_hInst, HWND* g_hWnd);
	~InputManager();

	HRESULT InitialiseInput();
	void ReadInputStates();
	bool IsKeyPressed(unsigned char DI_keycode);
	bool IsKeyBeganPressed(unsigned char DI_keycode);
	DIMOUSESTATE GetMouseState();
};

