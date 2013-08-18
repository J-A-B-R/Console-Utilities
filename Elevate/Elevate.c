#include "stdafx.h"
#include "Global.h"
#include "Process.h"


int _tmain(int argc, TCHAR* argv[])
{
    int result;

    ENSURE_DEBUGGER();

    if (argc == 1 || (argc == 2 && !_tcsicmp(*argv, _T("/?"))))
        return PrintUsage();

    // TODO:
    // /s return without blocking, like start
    // /n do not copy env vars

    SetUp();
    result = (!IsVistaOrAbove() || IsElevated()) ? ExecuteProgram() : ExecuteStub();
    return CleanUp(result);
}
