#include "InputManager.h"
#include <algorithm>
#include <iterator>


InputManager::InputManager(HINSTANCE * g_hInst, HWND* g_hWnd)
{
	m_hInst = g_hInst;
	m_hWnd = g_hWnd;
}

InputManager::~InputManager()
{
	if (m_keyboard_device) {
		m_keyboard_device->Unacquire();
		m_keyboard_device->Release();
	}
	if (m_mouse_device) {
		m_mouse_device->Unacquire();
		m_mouse_device->Release();
	}
	if (m_direct_input) m_direct_input->Release();
}

HRESULT InputManager::InitialiseInput()
{
	HRESULT hr;
	ZeroMemory(m_keyboard_keys_state, sizeof(m_keyboard_keys_state));

	hr = DirectInput8Create(*m_hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_direct_input, NULL);
	if (FAILED(hr)) return hr;

	hr = m_direct_input->CreateDevice(GUID_SysKeyboard, &m_keyboard_device, NULL);
	if (FAILED(hr)) return hr;

	hr = m_keyboard_device->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(hr)) return hr;

	hr = m_keyboard_device->SetCooperativeLevel(*m_hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(hr)) return hr;

	hr = m_keyboard_device->Acquire();
	if (FAILED(hr)) return hr;

	hr = m_direct_input->CreateDevice(GUID_SysMouse, &m_mouse_device, NULL);
	if (FAILED(hr)) return hr;

	hr = m_mouse_device->SetDataFormat(&c_dfDIMouse);
	if (FAILED(hr)) return hr;

	hr = m_mouse_device->SetCooperativeLevel(*m_hWnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
	if (FAILED(hr)) return hr;

	hr = m_mouse_device->Acquire();

	return S_OK;
}

void InputManager::ReadInputStates()
{
	std::copy(std::begin(m_keyboard_keys_state), std::end(m_keyboard_keys_state), std::begin(m_previous_keyboard_keys_state));

	HRESULT hr;
	hr = m_keyboard_device->GetDeviceState(sizeof(m_keyboard_keys_state), (LPVOID)&m_keyboard_keys_state);
	if (FAILED(hr)) {
		if ((hr == DIERR_INPUTLOST) || (hr == DIERR_NOTACQUIRED)) {
			m_keyboard_device->Acquire();
		}
	}

	hr = m_mouse_device->GetDeviceState(sizeof(m_mouse_state), (LPVOID)&m_mouse_state);
	if (FAILED(hr)) {
		if ((hr == DIERR_INPUTLOST) || (hr == DIERR_NOTACQUIRED)) {
			m_mouse_device->Acquire();
		}
	}
}

bool InputManager::IsKeyPressed(unsigned char DI_keycode)
{
	return m_keyboard_keys_state[DI_keycode] & 0x80;
}

bool InputManager::IsKeyBeganPressed(unsigned char DI_keycode)
{
	bool lastFrame = !(m_previous_keyboard_keys_state[DI_keycode] & 0x80);
	bool thisFrame = m_keyboard_keys_state[DI_keycode] & 0x80;
	return lastFrame && thisFrame;
}

DIMOUSESTATE InputManager::GetMouseState()
{
	return m_mouse_state;
}
