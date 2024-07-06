@ECHO OFF

NetClone\\NetClone.exe --target x64\\Release\\dllfunctest.dll --reference C:\\Windows\\System32\\%1.dll -o %1.dll