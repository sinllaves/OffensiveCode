/*
Summary:

1. Header Inclusion:
   - Includes the essential Windows API and NT system function headers for low-level operations involving file systems and directory management.

2. Reparse Point Structures:
   - Defines the _REPARSE_DATA_BUFFER structure to handle various types of reparse points, crucial for filesystem operations like symbolic links and mount points.
   - Utilizes unions to allow different data types in a single structure, supporting multiple reparse point formats including symbolic links, mount points, and generic data buffers.

3. Directory Information Structure:
   - Introduces the _OBJECT_DIRECTORY_INFORMATION structure, which holds name and type information about NT system directory objects, facilitating directory operations and management.

4. NT Status Constants:
   - Defines constants such as STATUS_MORE_ENTRIES and STATUS_NO_MORE_ENTRIES to manage flow control in directory traversal and operations, reflecting common NT status responses.

5. NT API Function Pointers:
   - Declares pointers to NT API functions, such as _NtCreateFile, _NtOpenDirectoryObject, and others, enabling direct system calls for file and directory management.
   - These pointers are essential for bypassing standard API limitations and accessing more granular system functionalities.

6. Dynamic Linking to NT Functions:
   - Sets up function pointers (_RtlInitUnicodeString, _NtCreateFile, _NtSetInformationFile) for dynamic linking, allowing runtime resolution of these functions which are crucial for advanced file and directory operations.

7. System-Level Integration:
   - Through the use of advanced structures and NT function pointers, the code provides a robust framework for interacting with lower-level Windows internals.
   - This setup is typically used in scenarios requiring direct and detailed control over filesystem operations and is critical in custom file management solutions, security frameworks, and system recovery tools.

*/
// Include necessary system libraries for Windows API and internal NT functions.
#include <windows.h>
#include <winternl.h>

// Structure to manage different types of reparse points (used in shortcuts and mounted drives)
typedef struct _REPARSE_DATA_BUFFER {
    ULONG ReparseTag;  // Identifies what kind of reparse point this is (e.g., symbolic link, mount point)
    USHORT ReparseDataLength;  // The length in bytes of the reparse data
    USHORT Reserved;  // Space reserved for future use, ensures alignment
    // A union allows this part of the structure to hold different kinds of data, depending on the reparse point type
    union {
        // Used for symbolic links
        struct {
            USHORT SubstituteNameOffset;  // The start of the substitute name string in PathBuffer
            USHORT SubstituteNameLength;  // The total length of the substitute name string
            USHORT PrintNameOffset;       // The start of the print name string in PathBuffer
            USHORT PrintNameLength;       // The total length of the print name string
            ULONG  Flags;                 // Flags that provide additional options for the symbolic link
            WCHAR  PathBuffer[1];         // A buffer holding the actual paths involved in the reparse point
        } SymbolicLinkReparseBuffer;
        // Used for mount points
        struct {
            USHORT SubstituteNameOffset;  // The start of the substitute name string in PathBuffer
            USHORT SubstituteNameLength;  // The total length of the substitute name string
            USHORT PrintNameOffset;       // The start of the print name string in PathBuffer
            USHORT PrintNameLength;       // The total length of the print name string
            WCHAR  PathBuffer[1];         // A buffer holding the actual paths involved in the reparse point
        } MountPointReparseBuffer;
        // Generic structure for other types of reparse points
        struct {
            UCHAR DataBuffer[1];  // A buffer to hold any kind of data for other reparse types
        } GenericReparseBuffer;
    } DUMMYUNIONNAME;
} REPARSE_DATA_BUFFER, * PREPARSE_DATA_BUFFER;

// Structure to represent directory information in NT systems
typedef struct _OBJECT_DIRECTORY_INFORMATION {
    UNICODE_STRING Name;      // The name of the directory object
    UNICODE_STRING TypeName;  // The type (category) of the directory object
} OBJECT_DIRECTORY_INFORMATION, * POBJECT_DIRECTORY_INFORMATION;

// Constants to represent specific responses from NT functions
#define STATUS_MORE_ENTRIES 0x00000105       // There are more entries to process
#define STATUS_NO_MORE_ENTRIES 0x8000001A    // There are no more entries to process
#define IO_REPARSE_TAG_MOUNT_POINT (0xA0000003L)  // Special identifier for mount point reparse tags

// Definitions for pointers to functions provided by the NT API, used to interact with files and directories
typedef NTSYSAPI NTSTATUS(NTAPI* _NtCreateFile)(
    PHANDLE FileHandle, ACCESS_MASK DesiredAccess, POBJECT_ATTRIBUTES ObjectAttributes,
    PIO_STATUS_BLOCK IoStatusBlock, PLARGE_INTEGER AllocationSize, ULONG FileAttributes,
    ULONG ShareAccess, ULONG CreateDisposition, ULONG CreateOptions,
    PVOID EaBuffer, ULONG EaLength);

typedef NTSYSAPI VOID(NTAPI* _RtlInitUnicodeString)(
    PUNICODE_STRING DestinationString, PCWSTR SourceString);

typedef NTSYSAPI NTSTATUS(NTAPI* _NtOpenDirectoryObject)(
    OUT PHANDLE DirectoryHandle, IN ACCESS_MASK DesiredAccess, POBJECT_ATTRIBUTES ObjectAttributes);

typedef NTSYSAPI NTSTATUS(NTAPI* _NtQueryDirectoryObject)(
    _In_ HANDLE DirectoryHandle, _Out_opt_ PVOID Buffer, _In_ ULONG Length,
    _In_ BOOLEAN ReturnSingleEntry, _In_ BOOLEAN RestartScan, _Inout_ PULONG Context,
    _Out_opt_ PULONG ReturnLength);

typedef NTSYSCALLAPI NTSTATUS(NTAPI* _NtSetInformationFile)(
    HANDLE FileHandle, PIO_STATUS_BLOCK IoStatusBlock, PVOID FileInformation,
    ULONG Length, ULONG FileInformationClass);

// Pointers to dynamically linked NT API functions
_RtlInitUnicodeString pRtlInitUnicodeString;
_NtCreateFile pNtCreateFile;
_NtSetInformationFile pNtSetInformationFile;
