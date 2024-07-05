void listSystem(wchar_t* system) {
    const wchar_t* uncPath = system;
    WIN32_FIND_DATAW findData;
    HANDLE hFind = INVALID_HANDLE_VALUE;

    wchar_t searchPath[MAX_PATH];
    _snwprintf_s(searchPath, MAX_PATH, L"%s\\*", uncPath);

    hFind = FindFirstFileW(searchPath, &findData);
    if (hFind == INVALID_HANDLE_VALUE) {
        wprintf(L"Failed to find files in %s\n", uncPath);
        return;
    }

    do {
        if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            wprintf(L"Directory: %s\n", findData.cFileName);
        }
        else {
            wprintf(L"File: %s\n", findData.cFileName);
        }
    } while (FindNextFileW(hFind, &findData) != 0);

    FindClose(hFind);
}