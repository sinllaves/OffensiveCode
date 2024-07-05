2 Things are in here. 1. what is going on. 2.How do i do it.

1. what is going on.
- Folder Creation:
  Automatically creates "C:\Users\<USER>\AppData\Local\Temp\Catlogger\".

- File Monitoring:
  Monitors for the creation of "kissa.txt" in the Catlogger folder.

- Execution to Trigger:
  Running "fileopplay.exe" generates "kissa.txt" upon launch.

- OpLock Implementation:
  Sets an OpLock on "kissa.txt" immediately after its creation. Check 'readdirectorychanges' for details.
  Uncomment "PathCchFindExtension" in the code to filter by file extension.

- OpLock Activation:
  Once OpLock is activated, move all files from "C:\Users\<USER>\AppData\Local\Temp\Catlogger\" to "C:\Windows\Temp" to prepare for junction creation.

- Junction and Symbolic Link Setup:
  Creates a junction from "C:\Users\<USER>\AppData\Local\Temp\Catlogger\" to "\\RPC Control". An empty folder is essential for this step.
  Function to denywrite and createfile in folder are in the works!
  Constructs a symbolic link from "GLOBAL\GLOBALROOT\RPC Control\kissa.txt" to a target file, e.g., "C:\Windows\System32\information.txt".

- Callback Execution:
  Uses callback function 'cb1' to release OpLock, triggering further actions.

- File Operation Completion:
  After operations, "fileopplay.exe" attempts to delete 'setdisposition' on "kissa.txt".
  The target file (e.g., "C:\Windows\System32\information.txt") would be deleted.

2.How do i do it.
Operations Summary:

  There are two main operations: delete arbitrary file and escalate privilege.

- Delete Arbitrary File:
  Command: `PoC.exe del targetFile`
  Executes OpLock on "kissa.txt" and sets the file as the target for deletion. Running "fileopplay.exe" will trigger this vulnerability.

- Escalate Privilege:
  Command: `PoC.exe pe RollbackScript.rbs`
  Executes the command and triggers the vulnerability. "cmd.rbs" will spawn a command prompt; "public_run_bat.rbs" executes "C:\Users\Public\run.bat".
