@echo off
make clean
make
pause
cls
rmdir /s Release
mkdir Release
copy AMG_Config.h Release
copy AMG_3D.h Release
copy AMG_Model.h Release
copy AMG_Physics.h Release
copy AMG_Texture.h Release
copy AMG_Thread.h Release
copy AMG_User.h Release
copy AMGLib.h Release
copy libAMG.a Release
copy lightmap.h Release
copy AMG_Multimedia.h Release
make clean
cd Release
mkdir include
mkdir lib
move AMG_Config.h include
move AMG_3D.h include
move AMG_Model.h include
move AMG_Physics.h include
move AMG_Texture.h include
move AMG_Thread.h include
move lightmap.h include
move AMG_User.h include
move AMGLib.h include
move AMG_Multimedia.h include
move libAMG.a lib
cd include
mkdir AMG
move AMG_Config.h AMG
move AMG_3D.h AMG
move AMG_Model.h AMG
move AMG_Physics.h AMG
move AMG_Texture.h AMG
move AMG_Thread.h AMG
move lightmap.h AMG
move AMG_User.h AMG
move AMG_Multimedia.h AMG
move AMGLib.h AMG
pause
exit
