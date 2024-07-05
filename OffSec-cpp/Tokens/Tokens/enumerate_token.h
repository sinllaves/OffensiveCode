#include <sddl.h>
#include <psapi.h>
#include <tlhelp32.h>

DWORD enumerateTokenSessionID(HANDLE &hToken) {
	DWORD dwSessionId;
	DWORD dwTokenLen = sizeof(dwSessionId);
	DWORD dwRetLen;

	if (!GetTokenInformation(hToken, TokenSessionId, &dwSessionId, dwTokenLen, &dwRetLen)) {
		wprintf(L"GetTokenInformation failed: %u\n", GetLastError());
	}
	else {
		return dwSessionId;
	}
}

void enumerateTokenGroups(HANDLE& hToken) {
	DWORD dwTokenInfoLen = 0;
	PTOKEN_GROUPS pTokenGroups = NULL;
	SID_NAME_USE SidType;
	wchar_t lpName[MAX_PATH];
	wchar_t lpDomain[MAX_PATH];
	DWORD dwName = MAX_PATH;
	DWORD dwDomain = MAX_PATH;

	if (!GetTokenInformation(hToken, TokenGroups, NULL, 0, &dwTokenInfoLen) && GetLastError() != ERROR_INSUFFICIENT_BUFFER) {
		wprintf(L"GetTokenInformation failed: %u\n", GetLastError());
		return;
	}

	pTokenGroups = (PTOKEN_GROUPS)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwTokenInfoLen);
	if (pTokenGroups == NULL) {
		wprintf(L"HeapAlloc failed: %u\n", GetLastError());
		return;
	}

	if (!GetTokenInformation(hToken, TokenGroups, pTokenGroups, dwTokenInfoLen, &dwTokenInfoLen)) {
		wprintf(L"GetTokenInformation failed: %u\n", GetLastError());
		return;
	}

	wprintf(L"[+] Token Groups\n");
	for (DWORD i = 0; i < pTokenGroups->GroupCount; i++) {
	// Call LookupAccountSid first time, if it fails, call it again with the correct buffer size given by the first call
		if (!LookupAccountSidW(NULL, pTokenGroups->Groups[i].Sid, lpName, &dwName, lpDomain, &dwDomain, &SidType)) {
			if (!LookupAccountSidW(NULL, pTokenGroups->Groups[i].Sid, lpName, &dwName, lpDomain, &dwDomain, &SidType)) {
				wprintf(L"LookupAccountSid failed: %u\n", GetLastError());
				return;
			}
		}
		wprintf(L"    [+] %ws\\%ws\n", lpDomain, lpName);
	}
}

void enumerateTokenUser(HANDLE& hToken) {
	PTOKEN_USER tokUser = NULL;
	DWORD dwTokenInfoLen = 0, dwRetLen = 0;
	WCHAR* pSid = NULL;
	WCHAR Account[MAX_PATH], Domain[MAX_PATH];
	DWORD dwAccount = MAX_PATH, dwDomain = MAX_PATH;
	SID_NAME_USE eSidType;

	// Call GetTokenInformation, with TokenInformation set to NULL, to get the required buffer size
	if (!GetTokenInformation(hToken, TokenUser, NULL, 0, &dwTokenInfoLen) && GetLastError() != ERROR_INSUFFICIENT_BUFFER) {
		wprintf(L"[!] GetTokenInformation failed: %u\n", GetLastError());
		return;
	}
	
	// Allocate buffer for PTOKEN_USER based on size returned by GetTokenInformation
	tokUser = (PTOKEN_USER)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwTokenInfoLen);
	if (tokUser == NULL) {
		wprintf(L"[!] HeapAlloc failed: %u\n", GetLastError());
		return;
	}

	// Call GetTokenInformation again, this time with TokenInformation pointer to buffer with required size
	if (!GetTokenInformation(hToken, TokenUser, (LPVOID)tokUser, dwTokenInfoLen, &dwTokenInfoLen)) {
		wprintf(L"[!] GetTokenInformation failed: %u\n", GetLastError());
		return;
	}

	if (!ConvertSidToStringSidW(tokUser->User.Sid, &pSid)) {
		wprintf(L"[!] ConvertSidToStringSidW failed: %u\n", GetLastError());
		return;
	}

	// LookupAccountSid
	if (!LookupAccountSidW(NULL, tokUser->User.Sid, Account, &dwAccount, Domain, &dwDomain, &eSidType)) {
		wprintf(L"[!] LookupAccountSid failed: %u\n", GetLastError());
		return;
	}

	wprintf(L"[+] User SID: %ws\n", pSid);
	wprintf(L"[+] Domain\\Account : %ws\\%ws \n", Domain, Account);

}

void enumerateTokenIntegrity(HANDLE& hToken) {
	PTOKEN_ELEVATION_TYPE tokenElevate = NULL;
	DWORD dwTokenInfoLen = 0, dwRetLen = 0;

	if (!GetTokenInformation(hToken, TokenElevationType, NULL, 0, &dwTokenInfoLen) && GetLastError() != ERROR_INSUFFICIENT_BUFFER) {
		wprintf(L"[!] GetTokenInformation failed: %u\n", GetLastError());
		return;
	}

	tokenElevate = (PTOKEN_ELEVATION_TYPE)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwTokenInfoLen);
	if (tokenElevate == NULL) {
		printf("Failed to allocate memory for token information: %u\n", GetLastError());
		return;
	}

	if (!GetTokenInformation(hToken, TokenElevationType, tokenElevate, dwTokenInfoLen, &dwRetLen) && GetLastError() != ERROR_INSUFFICIENT_BUFFER) {
		wprintf(L"[!] GetTokenInformation failed: %u\n", GetLastError());
		LocalFree(tokenElevate);
		return;
	}

	// tokenElevate->TokenIsElevated ? wprintf(L"[+] Token is elevated\n") : wprintf(L"[+] Token is not elevated\n");
	// wprintf(L"[+] TokenElevationType: %d\n", tokenElevate->ElevationType);
}


void enumerateTokenOwner(HANDLE& hToken) {
	PTOKEN_OWNER tokenOwner = NULL;
	DWORD dwTokenInfoLen = 0, dwRetLen = 0;
	WCHAR* pSid = NULL;
	WCHAR Account[MAX_PATH], Domain[MAX_PATH];
	DWORD dwAccount = MAX_PATH, dwDomain = MAX_PATH;
	SID_NAME_USE eSidType;

	if (!GetTokenInformation(hToken, TokenOwner, NULL, 0, &dwTokenInfoLen) && GetLastError() != ERROR_INSUFFICIENT_BUFFER) {
		wprintf(L"[!] GetTokenInformation failed: %u\n", GetLastError());
		return;
	}

	tokenOwner = (PTOKEN_OWNER)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwTokenInfoLen);
	if (tokenOwner == NULL) {
		wprintf(L"[!] Failed to allocate memory for token information: %u\n", GetLastError());
		return;
	}

	if (!GetTokenInformation(hToken, TokenOwner, tokenOwner, dwTokenInfoLen, &dwRetLen) && GetLastError() != ERROR_INSUFFICIENT_BUFFER) {
		wprintf(L"[!] GetTokenInformation failed: %u\n", GetLastError());
		LocalFree(tokenOwner);
		return;
	}

	// LookupAccountSid
	if (!LookupAccountSidW(NULL, tokenOwner->Owner, Account, &dwAccount, Domain, &dwDomain, &eSidType)) {
		wprintf(L"[!] LookupAccountSid failed: %u\n", GetLastError());
		return;
	}

	wprintf(L"[+] TokenOwner : %ws\\%ws \n\n", Domain, Account);
}


void enumerateSystemTokens() {
	DWORD processes[1024], cbNeeded, cProcesses;
	unsigned int i;

	// Get the list of process identifiers
	if (!EnumProcesses(processes, sizeof(processes), &cbNeeded)) {
		printf("Failed to enumerate processes: %u\n", GetLastError());
		return;
	}

	// Calculate how many process identifiers were returned
	cProcesses = cbNeeded / sizeof(DWORD);

	// Iterate through each process
	for (i = 0; i < cProcesses; i++) {
		HANDLE hProcess;
		HMODULE hModule;
		DWORD cbNeeded;

		// Open the process with PROCESS_QUERY_INFORMATION and PROCESS_VM_READ access rights
		hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_VM_READ, FALSE, processes[i]);
		if (hProcess != NULL) {
			wchar_t szProcessName[MAX_PATH];

			// Get the process name
			if (EnumProcessModules(hProcess, &hModule, sizeof(hModule), &cbNeeded)) {
				GetModuleBaseNameW(hProcess, hModule, szProcessName, sizeof(szProcessName) / sizeof(wchar_t));

				// Convert the process name to lowercase
				_wcslwr_s(szProcessName, sizeof(szProcessName) / sizeof(wchar_t));

				// Print the process name
				wprintf(L"[%d] %ws\n", processes[i], szProcessName);

				// Get token handle
				HANDLE hToken = NULL;

				if (!OpenProcessToken(hProcess, TOKEN_QUERY, &hToken)) {
					wprintf(L"OpenProcessToken failed: %u\n", GetLastError());
				}
				else {
					// Parse subcommand and excute accordingly
					enumerateTokenUser(hToken);
					enumerateTokenOwner(hToken);
				}
			}

			// Close the process handle
			CloseHandle(hProcess);
		}
	}
}

void enumerateThreadToken(DWORD pid) {
	HANDLE hThreadSnap = INVALID_HANDLE_VALUE;
	THREADENTRY32 te32;

	// Take a snapshot of all running threads in specified process
	hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, pid);
	if (hThreadSnap == INVALID_HANDLE_VALUE) {
		printf("Failed to create thread snapshot: %u\n", GetLastError());
		return;
	}

	// Set the size of the structure before using it
	te32.dwSize = sizeof(THREADENTRY32);

	// Retrieve information about the first thread in the snapshot
	if (!Thread32First(hThreadSnap, &te32)) {
		printf("Failed to retrieve first thread: %u\n", GetLastError());
		CloseHandle(hThreadSnap);
		return;
	}

	// Iterate through the threads
	do {
		if (te32.th32OwnerProcessID == pid) {
			PTOKEN_OWNER tokenOwner = NULL;
			DWORD dwTokenInfoLen = 0, dwRetLen = 0;
			WCHAR* pSid = NULL;
			WCHAR Account[MAX_PATH], Domain[MAX_PATH];
			DWORD dwAccount = MAX_PATH, dwDomain = MAX_PATH;
			SID_NAME_USE eSidType;


			wprintf(L"[+] Thread ID: %lu\n", te32.th32ThreadID);

			// Open a handle to each thread
			HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, te32.th32ThreadID);
			if (hThread == NULL) {
				wprintf(L"[!] OpenThread failed: %u\n", GetLastError());
			}

			// Open the thread token
			HANDLE thToken = NULL;
			if (!OpenThreadToken(hThread, MAXIMUM_ALLOWED, FALSE, &thToken)) {
				wprintf(L"[!] OpenThreadToken failed: %u\n", GetLastError());
			}

			enumerateTokenOwner(thToken);

		}
	} while (Thread32Next(hThreadSnap, &te32));

	// Close the snapshot handle
	CloseHandle(hThreadSnap);
}