#include "stdafx.h"
#include "Common.h"


#define IDR_HELP_TEXT 101
#define TRY_HELP(x) if (!(x)) return _tprintf(_T("No Help"));

// Expects lpszCommandLine to be non null
TCHAR* SkipFirstCmdLineArg(TCHAR* lpszCommandLine, BOOL nCorrectExtraWhiteSpace)
{
    TCHAR k = *lpszCommandLine;
    BOOL quoted = FALSE;
    WORD i = 0;

    while (k != 0) {
        if (k == '"')
            quoted = !quoted;
        else if (k == ' ' && !quoted)
            // The check is necessary because when launched from visual studio, the
            // separation between the first and second argument is one white space,
            // but when launched from the console, the separation is two white spaces.
            // Of course, if the first character of the second argument is a white
            // space, it will be lost when launching from the visual studio debugger.
            return lpszCommandLine + i + ((nCorrectExtraWhiteSpace && *(lpszCommandLine + i + 1) == ' ') ? 2 : 1);

        k = *(lpszCommandLine + ++i);
    }

    return lpszCommandLine + i;
}

// Expects the resource string to be null terminated 
TCHAR* GetString(UINT nMsgId, TCHAR* lpszDefaultMsg)
{
    TCHAR* message;

    return (LoadString(0, nMsgId, (TCHAR*)&message, 0)) ? message : (lpszDefaultMsg) ? lpszDefaultMsg : _T(""); 
}

// Expects the resource to be a null terminated unicode text with or without BOM
int PrintUsage()
{
    HRSRC rc;
    HGLOBAL rcData;
    DWORD size;
    wchar_t* text;

    TRY_HELP(rc = FindResource(NULL, MAKEINTRESOURCE(IDR_HELP_TEXT), _T("HELP_TEXT")));
    TRY_HELP(rcData = LoadResource(NULL, rc));
    TRY_HELP(size = SizeofResource(NULL, rc));
    TRY_HELP(text = (wchar_t*)(LockResource(rcData)));
    TRY_HELP(text[(size - 1) / sizeof(wchar_t)] == 0);

    // printf functions doesn't like BOM
    if (*text == 0xfeff || *text == 0xfffe)
        text++;

    wprintf(text);
    return EXIT_SUCCESS;
}
