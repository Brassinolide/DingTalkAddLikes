#pragma once
#include <string>
#include <stdio.h>
#include <d3d9.h>
#include <iostream>
#include <Windows.h>

using namespace std;

namespace bd {
	extern void thread();
	extern void AddLike();
	inline bool pressed = false;
	inline string bind = "[NONE]";
	inline bool enable = false;
	inline char* title = u8"¿ªË¢!";
	inline size_t selected_bind;
	inline HANDLE hThread;
}
