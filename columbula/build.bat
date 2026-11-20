@echo off
setlocal
if not exist dist mkdir dist
powershell -NoProfile -ExecutionPolicy Bypass -File tools\gen_table.ps1
if errorlevel 1 exit /b 1
cl /nologo /std:c11 /O2 /W4 /MT /Fe:dist\columbula-windows.exe ^
   src\main.c src\warble.c src\platform.c
if errorlevel 1 exit /b 1
del /q *.obj 2>nul
echo Done: dist\columbula-windows.exe
