#include "main.h"

std::string getbindtext(size_t hex);

void bd::thread() {
	size_t selected_bind = 0;
	bool bind_break = false;
	while (1) {
		if (bd::pressed) {
			Sleep(1);
			selected_bind = 1;
			bd::selected_bind = 0;
			bool lith_bind_first_loop = false; size_t bool_counter = 0;
			while (bd::selected_bind == 0) {
				if (bool_counter < 1) { lith_bind_first_loop = true; }
				else { lith_bind_first_loop = false; }
				Sleep(2);
				for (size_t i = 3; i < 256; i++) {
					if (i != 13) {
						if (GetAsyncKeyState((i)&SHRT_MAX) && bd::selected_bind == 0) {
							if (!lith_bind_first_loop) {
								bd::selected_bind = i;
								bd::bind = getbindtext(i);
								if (i == 27) {
									bd::bind = "[NONE]";
									selected_bind = 1;
									bd::selected_bind = 0;
									bind_break = true;
								}
								Sleep(250);
							}
						}
					}
				}
				if (bind_break) { bind_break = false; break; }
				selected_bind = 0;
				bd::pressed = false;
				bool_counter++;
			}
		}
		if (GetAsyncKeyState(bd::selected_bind) & 0x8000) {
			if (!bd::enable) {
				while (GetAsyncKeyState(bd::selected_bind) & 0x8000) {
					Sleep(5);
				}
				bd::hThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)&bd::AddLike, 0, 0, 0);
				bd::enable = true;
				bd::title = u8"Í£Ö¹";
			}
			else {
				while (GetAsyncKeyState(bd::selected_bind) & 0x8000) {
					Sleep(5);
				}
				TerminateThread(bd::hThread, 0);
				bd::enable = false;
				bd::title = u8"¿ªË¢!";
			}
		}
	
		Sleep(50);
	}
}

std::string getbindtext(size_t hex) {
	const char* const lith_key_string[] = {
		"Unknown",
		"LButton",
		"RButton",
		"CANCEL",
		"MButton",
		"MButton2",
		"MButton3",
		"Unknown",
		"Back",
		"Tab",
		"Unknown",
		"Unknown",
		"Clear",
		"Return",
		"Unknown",
		"Unknown",
		"Shift",
		"Control",
		"Menu",
		"Pause",
		"Capital",
		"Unknown",
		"Unknown",
		"Unknown",
		"Final",
		"Unknown",
		"Unknown",
		"Escape",
		"Convert",
		"NonConvert",
		"Accept",
		"ModeChange",
		"Space",
		"Prior",
		"Next",
		"End",
		"Home",
		"Left",
		"Up",
		"Right",
		"Down",
		"Select",
		"Print",
		"Execute",
		"SnpShot",
		"Insert",
		"Delete",
		"Help",
		"0",
		"1",
		"2",
		"3",
		"4",
		"5",
		"6",
		"7",
		"8",
		"9",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"A",
		"B",
		"C",
		"D",
		"E",
		"F",
		"G",
		"H",
		"I",
		"J",
		"K",
		"L",
		"M",
		"N",
		"O",
		"P",
		"Q",
		"R",
		"S",
		"T",
		"U",
		"V",
		"W",
		"X",
		"Y",
		"Z",
		"Lwin",
		"Rwin",
		"Apps",
		"Unknown",
		"Sleep",
		"0",
		"1",
		"2",
		"3",
		"4",
		"5",
		"6",
		"7",
		"8",
		"9",
		"*",
		"+",
		"Separator",
		"-",
		"Decimal",
		"/",
		"F1",
		"F2",
		"F3",
		"F4",
		"F5",
		"F6",
		"F7",
		"F8",
		"F9",
		"F10",
		"F11",
		"F12",
		"F13",
		"F14",
		"F15",
		"F16",
		"F17",
		"F18",
		"F19",
		"F20",
		"F21",
		"F22",
		"F23",
		"F24",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Numlock",
		"Scroll",
		"Equal",
		"Masshou",
		"Touroku",
		"Loya",
		"Roya",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"LShift",
		"RShift",
		"Control",
		"Control",
		"Menu",
		"RMenu",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
		"Unknown",
	};
	return "[Bind: " + string(lith_key_string[hex]) + "]";
}
