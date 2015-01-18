@echo off
echo Clearing old version...
if exist ..\runtime\nucage rd /s /q ..\runtime\nucage
echo Copying executable files...
md ..\runtime\nucage
copy ..\..\..\thirdlibs\fmod\bin\fmod.dll ..\runtime\nucage\fmod.dll
copy ..\out\bin\client.exe ..\runtime\nucage\nucage.exe  
..\..\..\tools\upx.exe --best ..\runtime\nucage\nucage.exe


if "%1" == "RELEASE" goto release
if "%1" == "" echo Target not specified. Targeting DEBUG.
if "%1" == "DEBUG" echo Targeting DEBUG.

echo Copying media files...
xcopy /s /e /y /exclude:excl.lst ..\media ..\runtime\nucage\
goto end

:release

echo Targeting RELEASE.
echo Prepearing media files to packing...
md ..\tmp
xcopy /s /e /y /exclude:excl.lst ..\media ..\tmp

echo Crypting files...
cd ..\tmp

dir /b > ..\tmplst
..\..\..\tools\crypt.exe -c ..\tmplst

cd data
dir /b > ..\..\tmplst
..\..\..\..\tools\crypt.exe -c ..\..\tmplst

cd ..\fonts
dir /b > ..\..\tmplst
..\..\..\..\tools\crypt.exe -c ..\..\tmplst

cd ..\scenes
dir /b > ..\..\tmplst
..\..\..\..\tools\crypt.exe -c ..\..\tmplst

cd ..\sfx
dir /b > ..\..\tmplst
..\..\..\..\tools\crypt.exe -c ..\..\tmplst

cd ..

echo Packing media files...
..\..\..\tools\makear.exe /c /z /e /m anim master.gaf
..\..\..\tools\makear.exe /c /z /e /m sound sound.gaf

cd ..

echo Copying media files...
xcopy /s /e /y .\tmp runtime\nucage\

echo Removing any temporary files used...
rd /s /q tmp
del tmplst

:end
echo Build succeded.