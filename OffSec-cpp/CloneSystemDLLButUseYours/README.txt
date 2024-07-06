modifications or creations of .manifest and local redirection files, dirs or junctions to cause the program to load a dll.

Schedule task path to dll hijack

Check through schedule task for what is writeable and run elevated.
Runs as system

Found exe 
 


**Look for dlls not found that the exe is looking for**
	
procmon
setup procmon w/ filters:

 user is NT AUTHORITY\SYSTEM
 Path ends with sys
 
pause capture
clear events
start capture
run found schedule task, refresh, wait until ready
pause capture
filter by exe. (right click 'include exe") choose a dll
select filtered ctrl c copy to a .txt as backup


I will choose version.dll
clone exported sys functions and add to your custom dll.

open dllfunctest> dllhijacktest.cpp (to dll function (do a command)test) and DLL LOADER ( the app that loads the dll and tells you the address of where it is loaded as the proof)  in vs

------TEST function----
-- Build dll -- 
in Explorer in the dllhijack solution folder delete .dll, (an old dll from a previous project) and open the dllfunctest to fix the function

change cmd arg to push txt file to
```
c:\\windows\\kissa.txt
```
rebuild
copy location from build output where dll is stored to enter that into loader


-- Load dll --
the dll loader exe is what is going to call or load the dll function into memory
add in the dllfunctestdll address

in the DLL LOADER main loadlibrary addin the dll path from above this should be in the solution folder x64 release next to the loader app, not the program folder.  

```
C:\\Users\\admin\\source\\repos\\Updated8-17-23\\OffsecDev-main\\cpp\\DLLforTest\\DllHijackTest\\x64\\Release\\dllfunctest.dll
```

rebuild solution


Test that a dll function will be called

A cmd window will flash

run .\dllloader.exe 
Verify kissa.txt is in the c:\windows folder on the vm. Ok its good.

-----
PREP THE DLL BY CLONING Sys32 DLL export functions
prep the dll and change the name with .\prepdll.bat version. This is the dll "name not found" (version.dll)
 
uses net clone to clone functions of sys32 dll??

---------------
Move version.dll to host machine

Move dll to dir that is missing dll
Run scheduled task that will trigger dll
Review c:\windows for the kissa.txt that was created. 
Ok it is. Back to procmon

Check for SUCCESS validation of exploitation
Success


