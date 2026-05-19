#pragma once
#include "INC_Windows.h"


LRESULT CALLBACK NzWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

class NzWndBase
{
public:
	NzWndBase() = default;
	virtual ~NzWndBase() = default;

	bool Create(const wchar_t* className, const wchar_t* windowName, int width, int height);
	void Destory();

	void* GetHandle() const { return m_hWnd; }
	int GetWidth() const { return m_width; }
	int getHeight() const { return m_height; }

protected:
	friend LRESULT CALLBACK NzWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	virtual void OnResize(int width, int height);
	virtual void OnClose() {}

	HWND m_hWnd = HWND();
	int m_width = 0;
	int m_height = 0;

	NzWndBase(const NzWndBase&) = delete;
	NzWndBase& operator = (const NzWndBase&) = delete;
	NzWndBase(NzWndBase&&) = delete;
	NzWndBase& operator = (NzWndBase&&) = delete;
};