@echo off
cls
del Release/EBOOT.PBP
rd Release
cls
make clean
make
pause
del PARAM.SFO
del *.o
del *.elf
mkdir Release
move EBOOT.PBP Release
make delete
cls

