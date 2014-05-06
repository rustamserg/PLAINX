@echo off
echo Clearing old version...
if exist ..\runtime\masterdx rd /s /q ..\runtime\masterdx
echo Copying executable files...
md ..\runtime\masterdx
copy ..\..\..\thirdlibs\fmod\bin\fmod.dll ..\runtime\masterdx\fmod.dll
copy ..\out\bin\plainx.exe ..\runtime\masterdx\masterdx.exe  
..\..\..\tools\upx.exe --best ..\runtime\masterdx\masterdx.exe


if "%1" == "RELEASE" goto release
if "%1" == "" echo Target not specified. Targeting DEBUG.
if "%1" == "DEBUG" echo Targeting DEBUG.

echo Copying media files...
xcopy /s /e /y /exclude:excl.lst ..\media\masterdx ..\runtime\masterdx\
goto end

:release

echo Targeting RELEASE.
echo Prepearing media files to packing...
md ..\tmp
xcopy /s /e /y /exclude:excl.lst ..\media\masterdx ..\tmp

echo Crypting files...
cd ..\tmp

dir /b > ..\tmplst
..\..\..\tools\crypt.exe -c ..\tmplst

cd data
dir /b > ..\..\tmplst
..\..\..\..\tools\crypt.exe -c ..\..\tmplst

cd ..\sfx
dir /b > ..\..\tmplst
..\..\..\..\tools\crypt.exe -c ..\..\tmplst

cd ..\scene
dir /b > ..\..\tmplst
..\..\..\..\tools\crypt.exe -c ..\..\tmplst
cd ..

echo Packing media files...
..\..\..\tools\makear.exe /c /z /e /m anim master.gaf
..\..\..\tools\makear.exe /c /z /e /m sound sound.gaf

cd ..

echo Copying media files...
xcopy /s /e /y .\tmp runtime\masterdx\

echo Removing any temporary files used...
rd /s /q tmp
del tmplst

:end
echo Build succeded.