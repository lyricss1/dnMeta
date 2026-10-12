@echo off
gcc -O2 -o tools\embed.exe tools\embed.c || exit /b 1
tools\embed.exe web src\ui.c || exit /b 1
gcc -O2 -std=c99 -Wall -o dnmeta.exe src\*.c
rem cl /O2 /Fe:dnmeta.exe src\*.c
