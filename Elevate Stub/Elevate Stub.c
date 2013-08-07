#include "stdafx.h"
#include "Global.h"
#include "Process.h"


int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);

    ENSURE_DEBUGGER();

    SetUp();
    CheckSuitability();
    StartIpcCommunication(lpCmdLine);
    ReceiveIpcData();
    PrepareForLaunching();
    CreateTargetProcess();
    FinishIpcCommunication();
    ResumeTargetProcess();
    CleanUp();

    return EXIT_SUCCESS;
}
