@echo off
REM =============================================
REM  GS-CARD Compilation Script
REM  Run from \SRC, outputs to \BUILD
REM
REM  Compile:   tcc -c -n..\BUILD  (obj в BUILD)
REM  Link:      tlink @response file  (обходит лимит 127 символов DOS)
REM
REM  Memory model: SMALL (-ms) — small static data, far code+data
REM =============================================

set PATH=C:\TURBOC\BIN;%PATH%
set TURBOC=C:\TURBOC

set BLD=..\BUILD
set INC=%TURBOC%\INCLUDE
set LIB=%TURBOC%\LIBRARY

echo ======================================== > ..\last.log
echo    GS-CARD Compilation                   >> ..\last.log
echo ======================================== >> ..\last.log
echo.                                         >> ..\last.log

REM ---------- compile each .c to ..\BUILD\*.obj ----------
echo Compiling CARD.C...                      >> ..\last.log
tcc -c -ms -n%BLD% -I%INC% -I..\SRC -I%TURBOC%\BGI -L%LIB% ..\SRC\CARD.C >> ..\last.log
if errorlevel 1 goto err

echo Compiling GUI.C...                       >> ..\last.log
tcc -c -ms -n%BLD% -I%INC% -I..\SRC -I%TURBOC%\BGI -L%LIB% ..\SRC\GUI.C >> ..\last.log
if errorlevel 1 goto err

echo Compiling READER.C...                    >> ..\last.log
tcc -c -ms -n%BLD% -I%INC% -I..\SRC -I%TURBOC%\BGI -L%LIB% ..\SRC\READER.C >> ..\last.log
if errorlevel 1 goto err

echo Compiling FINDER.C...                    >> ..\last.log
tcc -c -ms -n%BLD% -I%INC% -I..\SRC -I%TURBOC%\BGI -L%LIB% ..\SRC\FINDER.C >> ..\last.log
if errorlevel 1 goto err

echo Compiling AD.C...                        >> ..\last.log
tcc -c -ms -n%BLD% -I%INC% -I..\SRC -I%TURBOC%\BGI -L%LIB% ..\SRC\AD.C >> ..\last.log
if errorlevel 1 goto err

echo Compiling SPLASH.C...                    >> ..\last.log
tcc -c -ms -n%BLD% -I%INC% -I..\SRC -I%TURBOC%\BGI -L%LIB% ..\SRC\SPLASH.C >> ..\last.log
if errorlevel 1 goto err

echo.                                         >> ..\last.log

REM ---------- link via TLINK with response file ----------
echo Linking GSCARD.EXE...                    >> ..\last.log

REM TLINK response file format:
REM   line 1:    [options] objfiles (use + to continue objfiles to next line)
REM   line 2:    more objfiles, exefile
REM   line 3:    mapfile
REM   line 4:    libfiles (space-separated)
REM
REM Example: tlink /c main wd+ln tx, fin, mfin, lib\comm lib\support

echo /x /c %TURBOC%\LIBRARY\C0H.OBJ+ >  %BLD%\GS-LINK.RSP
echo CARD.OBJ+                              >> %BLD%\GS-LINK.RSP
echo GUI.OBJ+                                >> %BLD%\GS-LINK.RSP
echo READER.OBJ+                             >> %BLD%\GS-LINK.RSP
echo FINDER.OBJ+                             >> %BLD%\GS-LINK.RSP
echo AD.OBJ+                                 >> %BLD%\GS-LINK.RSP
echo SPLASH.OBJ, GSCARD.EXE, GSCARD.MAP, %LIB%\EMU.LIB %LIB%\MATHH.LIB %LIB%\GRAPHICS.LIB %LIB%\CH.LIB >> %BLD%\GS-LINK.RSP

cd %BLD%

REM Clean up old files
if exist GSCARD.EXE del GSCARD.EXE
if exist GSCARD.MAP del GSCARD.MAP

tlink @GS-LINK.RSP >> ..\last.log
if errorlevel 1 goto err

echo.                                         >> ..\last.log
echo ======================================== >> ..\last.log
echo    SUCCESS! GSCARD.EXE created!          >> ..\last.log
echo ======================================== >> ..\last.log
echo.                                         >> ..\last.log

echo.
echo === Build log (last.log) ===
echo.
type ..\last.log
echo.
goto end

:err
echo.                                         >> ..\last.log
echo ======================================== >> ..\last.log
echo    COMPILATION FAILED!                   >> ..\last.log
echo ======================================== >> ..\last.log
echo.                                         >> ..\last.log
echo.
echo === Build log (last.log) ===
echo.
type ..\last.log
echo.
echo Compilation failed — check last.log above.
pause

:end
cd ..