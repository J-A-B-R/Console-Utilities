#include "stdafx.h"
#include "Global.h"
#include "Process.h"


int _tmain(int argc, TCHAR* argv[])
{
    int result;

    ENSURE_DEBUGGER();

    if (argc == 1 || (argc == 2 && !_tcsicmp(*(argv + 1), _T("/?"))))
        return PRINT_USAGE();

    // TODO:
    // /r (raw) do not interpret internal cmd commands as cmd /d /c command
    // /n do not copy env vars

    SetUp();
    result = (!IsVistaOrAbove() || IsElevated()) ? ExecuteProgram() : ExecuteStub();
    return CleanUp(result);
}
