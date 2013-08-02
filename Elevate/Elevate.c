#include "stdafx.h"

#ifdef _DEBUG
#define STUB_NAME _T("_Elev.exe")
#else
#define STUB_NAME _T("Elev.exe")
#endif

int _tmain(int argc, TCHAR* argv[])
{
    ALERT("Arreando");
    return EXIT_SUCCESS;
}
