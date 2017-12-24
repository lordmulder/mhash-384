#define WIN32_LEAN_AND_MEAN 1
#include <Windows.h>
#include <ShellAPI.h>
#include <Shlwapi.h>

#include "win32_prng.h"

#define BUFF_SIZE 1024U

static void echo(const HANDLE hStream, const char *const text)
{
	DWORD written;
	WriteFile(hStream, text, sizeof(char) * lstrlenA(text), &written, NULL);
}

static BOOL parse_arguments(ULONGLONG *const count)
{
	int nArgs;
	LPWSTR *szArglist;
	LONGLONG temp;
	BOOL ret = TRUE;

	szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);
	if (szArglist != NULL)
	{
		if (nArgs > 1)
		{
			ret = FALSE;
			if (StrToInt64ExW(szArglist[1], STIF_DEFAULT, &temp))
			{
				if (temp > 0)
				{
					*count = temp;
					ret = TRUE;
				}
			}
		}
		LocalFree(szArglist);
	}

	return ret;
}

void rand_main(void)
{
	BYTE buffer[BUFF_SIZE];
	ULONGLONG count = MAXULONGLONG;
	rand_t rnd_context;

	const HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	const HANDLE hStdErr = GetStdHandle(STD_ERROR_HANDLE);

	if (!parse_arguments(&count))
	{
		echo(hStdErr, "Error: Invalid argument specified!\n");
		ExitProcess(1);
	}

	rand_init(&rnd_context);

	while (count)
	{
		const ULONG size = (count >= BUFF_SIZE) ? BUFF_SIZE : (ULONG)count;
		DWORD written;
		rand_bytes(&rnd_context, buffer, size);
		if(!WriteFile(hStdOut, buffer, sizeof(BYTE) * size, &written, NULL))
		{
			ExitProcess(-1);
		}
		if (count != MAXULONGLONG)
		{
			count -= written;
		}
	}

	rand_close(&rnd_context);
	ExitProcess(0);
}

