#include "stdafx.h"

int PrintAttributes(int what)
{
    WORD attrs = GetAttributes();

    if (attrs == ATTRIBUTE_ERROR)
        SysError();

    switch (what) {
    case 1:
        attrs = attrs & 0xf;
        break;
    case 2:
        attrs = (attrs >> 4) & 0xf;
        break;
    case 3:
        attrs = attrs & 0xff;
    }

    _tprintf((what == 3) ? _T("%02X") : _T("%X"), attrs);
    return EXIT_SUCCESS;
}

int _tmain(int argc, TCHAR* argv[])
{
    WORD oldAttrs;
    WORD newAttrs;
    TCHAR* arg;
    BOOL isConsole = TRUE;

    if (argc == 1)
        return PrintAttributes(3);

    if (argc != 2)
        AppError(IDS_WRONG_ARG_COUNT);

    arg = argv[1];

    if (!_tcsicmp(arg, _T("/?")))
        return PrintUsage();

    if (!_tcsicmp(arg, _T("/p")))
        return PrintAttributes(3);

    if (!_tcsicmp(arg, _T("/pb")))
        return PrintAttributes(2);

    if (!_tcsicmp(arg, _T("/pg")))
        return PrintAttributes(1);

    if ((oldAttrs = GetAttributes()) == ATTRIBUTE_ERROR) {
        SysError();
        isConsole = FALSE;
    }

    if ((newAttrs = MergeAttributes(arg, oldAttrs)) == ATTRIBUTE_ERROR)
        AppError(IDS_WRONG_INDEX_FORMAT);

    if (isConsole && !SetAttributes(newAttrs))
        SysError();

    return EXIT_SUCCESS;
}
