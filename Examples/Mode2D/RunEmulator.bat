@echo off

rem Tienes que cambiar las rutas del emulador por las tuyas, solo funciona con el emulador PPSSPP
rem You have to change the emulator's paths by yourself, it only works on PPSSPP

cls
move Release\EBOOT.PBP C:\devkitPro\devkitPSP\psp\sdk\ppsspp\memstick\PSP\GAME
start C:\devkitPro\devkitPSP\psp\sdk\ppsspp\PPSSPPWindows64.exe
pause
exit
