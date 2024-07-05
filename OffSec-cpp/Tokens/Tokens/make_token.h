void makeToken(WCHAR* username, WCHAR* domain, WCHAR* password, HANDLE &hToken) {
	if (!LogonUserW(username, domain, password, LOGON32_LOGON_NEW_CREDENTIALS, LOGON32_PROVIDER_DEFAULT, &hToken))
	{
		wprintf(L"LogonUserW failed: %u\n", GetLastError());
		return;
	}
	else {
		wprintf(L"[+] Token created for user: %ws\n", username);
	}
}