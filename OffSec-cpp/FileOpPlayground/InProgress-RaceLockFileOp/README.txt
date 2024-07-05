creates folder "C:\Users\<USER>\AppData\Local\Temp\Catlogger\"
	
waits for a new file with file extension "kissa.txt" to be created in that folder.

you run "fileopplay.exe" creates outputfile "C:\Users\<USER>\AppData\Local\Temp\Catlogger\kissa.txt" during start. 

sets OpLock on that output file once the file was created.   so the POC looks for when "kissa.txt" (you can change this in code) is created and sets the lock then. !! See readdirectorychanges
PathCchFindExtension(fileName, MAX_PATH, &extension2); //  uncomment to look by the file extension
The process "fileopplay.exe" will be paused due to the OpLock

When OpLock is triggered, user move all files in "C:\Users\<USER>\AppData\Local\Temp\Catlogger\" to c win temp to empty the folder, to create junction
BOOL Move, function to move to a new location via a uuid to c windows temp.

creates junction C:\Users\<USER>\AppData\Local\Temp\Catlogger\ to "\RPC Control", empty folder is necesary. function to denywrite and createfile in folder are in the works!
creates symbolic link "GLOBAL\GLOBALROOT\RPC Control\kissa.txt" to target file (e.g., C:\Windows\System32\information.txt
function cb1 is the callback function to release OpLock
The process "fileopplay.exe" attempts to delete 'setdisposition' on the output file after it finishs doing other operations.
Target file (e.g., C:\Windows\System32\information.txt) would be deleted
delete symbolic link

-------------------------------------------------------------------------------

There are 2 main operations 
	-delete arbitrary file
	-escalte priviliege

-delete arbitrary file

command 
PoC.exe del targetFile

will perform oplock on a kissa.txt file, and you set a file as a target to delete.
user will run fileopPlay.exe program and it will trigger vulnerability.
Function to deletefolder and foldercontents  coming. file op copy and write args to come
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
-escalte priviliege
cmd prompt wont appear yet.

1. use the command
command

PoC.exe pe RollbackScript.rbs

2.
run app and trigger vulnerability

cmd.rbs will spawn command prompt

public_run_bat.rbs will execute C:\Users\Public\run.bat