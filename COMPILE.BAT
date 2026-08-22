@echo off
REM =============================================
REM  GS-CARD Compilation Script
REM  Run from project root (C:\), outputs to BUILD\
REM
REM  Compile:   tcc -c -nBUILD  (obj в BUILD)
REM  Link:      tlink @response file
REM
REM  Memory model: SMALL (-ml)
REM =============================================

set PATH=C:\TURBOC\BIN;%PATH%
set TURBOC=C:\TURBOC

set BLD=BUILD
set SRC=SRC
set INC=%TURBOC%\INCLUDE
set LIB=%TURBOC%\LIBRARY
set ASST=ASSETS
set RU=-DLANGUAGE_RU

REM ensure BUILD directory exists
if not exist %BLD% mkdir %BLD%

echo ======================================== >  %BLD%\build.log
echo    GS-CARD Compilation                   >> %BLD%\build.log
echo ======================================== >> %BLD%\build.log
echo.                                         >> %BLD%\build.log

REM ---------- compile each .c to BUILD\*.obj ----------
echo Compiling CARD.C...                      >> %BLD%\build.log
tcc -c -ml %RU% -n%BLD% -I%INC% -I%SRC% -I%TURBOC%\BGI -L%LIB% %SRC%\CARD.C >> %BLD%\build.log
if errorlevel 1 goto err

echo Compiling UI\GUI.C...                       >> %BLD%\build.log
tcc -c -ml %RU% -n%BLD% -I%INC% -I%SRC% -I%TURBOC%\BGI -L%LIB% %SRC%\UI\GUI.C >> %BLD%\build.log
if errorlevel 1 goto err

echo Compiling DATA\READER.C...                    >> %BLD%\build.log
tcc -c -ml %RU% -n%BLD% -I%INC% -I%SRC% -I%TURBOC%\BGI -L%LIB% %SRC%\DATA\READER.C >> %BLD%\build.log
if errorlevel 1 goto err

echo Compiling CORE\FINDER.C...                    >> %BLD%\build.log
tcc -c -ml %RU% -n%BLD% -I%INC% -I%SRC% -I%TURBOC%\BGI -L%LIB% %SRC%\CORE\FINDER.C >> %BLD%\build.log
if errorlevel 1 goto err

echo Compiling UI\AD.C...                        >> %BLD%\build.log
tcc -c -ml %RU% -n%BLD% -I%INC% -I%SRC% -I%TURBOC%\BGI -L%LIB% %SRC%\UI\AD.C >> %BLD%\build.log
if errorlevel 1 goto err

echo Compiling UI\SPLASH.C...                    >> %BLD%\build.log
tcc -c -ml %RU% -n%BLD% -I%INC% -I%SRC% -I%TURBOC%\BGI -L%LIB% %SRC%\UI\SPLASH.C >> %BLD%\build.log
if errorlevel 1 goto err

echo Compiling CORE\OBJECTS.C...                    >> %BLD%\build.log
tcc -c -ml %RU% -n%BLD% -I%INC% -I%SRC% -I%TURBOC%\BGI -L%LIB% %SRC%\CORE\OBJECTS.C >> %BLD%\build.log
if errorlevel 1 goto err

echo Compiling UI\MAP\MAPWND.C...                      >> %BLD%\build.log
tcc -c -ml %RU% -n%BLD% -I%INC% -I%SRC% -I%TURBOC%\BGI -L%LIB% %SRC%\UI\MAP\MAPWND.C >> %BLD%\build.log
if errorlevel 1 goto err

echo Compiling UI\MAP\NAV.C...                         >> %BLD%\build.log
tcc -c -ml %RU% -n%BLD% -I%INC% -I%SRC% -I%TURBOC%\BGI -L%LIB% %SRC%\UI\MAP\NAV.C >> %BLD%\build.log
if errorlevel 1 goto err

echo Compiling UI\MAP\PATHWND.C...                     >> %BLD%\build.log
tcc -c -ml %RU% -n%BLD% -I%INC% -I%SRC% -I%TURBOC%\BGI -L%LIB% %SRC%\UI\MAP\PATHWND.C >> %BLD%\build.log
if errorlevel 1 goto err

echo Compiling UI\MENUWND.C...                     >> %BLD%\build.log
tcc -c -ml %RU% -n%BLD% -I%INC% -I%SRC% -I%TURBOC%\BGI -L%LIB% %SRC%\UI\MENUWND.C >> %BLD%\build.log
if errorlevel 1 goto err

echo Compiling UI\STATWND.C...                     >> %BLD%\build.log
tcc -c -ml %RU% -n%BLD% -I%INC% -I%SRC% -I%TURBOC%\BGI -L%LIB% %SRC%\UI\STATWND.C >> %BLD%\build.log
if errorlevel 1 goto err

echo Compiling CORE\GAME.C...                     >> %BLD%\build.log
tcc -c -ml %RU% -n%BLD% -I%INC% -I%SRC% -I%TURBOC%\BGI -L%LIB% %SRC%\CORE\GAME.C >> %BLD%\build.log
if errorlevel 1 goto err





echo.                                         >> %BLD%\build.log
echo Linking GSCARD.EXE...                    >> %BLD%\build.log

REM TLINK response file format:
REM   line 1:    [options] objfiles (use + to continue objfiles to next line)
REM   line 2:    more objfiles, exefile
REM   line 3:    mapfile
REM   line 4:    libfiles (space-separated)

echo /x /c %LIB%\C0L.OBJ+ >  %BLD%\GS-LINK.RSP
echo CARD.OBJ+                              >> %BLD%\GS-LINK.RSP
echo GUI.OBJ+                                >> %BLD%\GS-LINK.RSP
echo READER.OBJ+                             >> %BLD%\GS-LINK.RSP
echo FINDER.OBJ+                             >> %BLD%\GS-LINK.RSP
echo AD.OBJ+                                 >> %BLD%\GS-LINK.RSP
echo SPLASH.OBJ+                                 >> %BLD%\GS-LINK.RSP
echo OBJECTS.OBJ+                                >> %BLD%\GS-LINK.RSP
echo MAPWND.OBJ+                                 >> %BLD%\GS-LINK.RSP
echo NAV.OBJ+                                    >> %BLD%\GS-LINK.RSP
echo MENUWND.OBJ+                                    >> %BLD%\GS-LINK.RSP
echo STATWND.OBJ+                                    >> %BLD%\GS-LINK.RSP
echo GAME.OBJ+                                    >> %BLD%\GS-LINK.RSP
echo PATHWND.OBJ, GSCARD.EXE, GSCARD.MAP, %LIB%\EMU.LIB %LIB%\MATHL.LIB %LIB%\GRAPHICS.LIB %LIB%\CL.LIB >> %BLD%\GS-LINK.RSP

cd %BLD%

REM Clean up old files
if exist GSCARD.EXE del GSCARD.EXE
if exist GSCARD.MAP del GSCARD.MAP

tlink @GS-LINK.RSP >> build.log
if errorlevel 1 goto err

REM ---------- copy runtime assets to BUILD ----------
echo.                                         >> build.log
echo Copying assets...                        >> build.log
if not exist BGI mkdir BGI
copy ..\%ASST%\BGI\*.BGI BGI\ > nul
copy ..\%ASST%\BGI\*.CHR BGI\ > nul
copy ..\%ASST%\LOGO.BMP . > nul
copy ..\%ASST%\SYSTEM.SOL . > nul
copy ..\%ASST%\OBJECTS.SOL . > nul
copy ..\%ASST%\BOUNDS.SOL . > nul

REM ---------- cleanup intermediate files ----------
echo Cleaning intermediates...                >> build.log
del *.obj
del *.rsp
del *.map

echo.                                         >> build.log
echo ======================================== >> build.log
echo    SUCCESS! GSCARD.EXE created!          >> build.log
echo ======================================== >> build.log
echo.                                         >> build.log

echo.
echo === Build log (build.log) ===
echo.
type build.log
echo.
goto end

:err
echo.                                         >> build.log
echo ======================================== >> build.log
echo    COMPILATION FAILED!                   >> build.log
echo ======================================== >> build.log
echo.                                         >> build.log
echo.
echo === Build log (build.log) ===
echo.
type build.log
echo.
echo Compilation failed — check BUILD\build.log above.

:end
cd ..\
exit
