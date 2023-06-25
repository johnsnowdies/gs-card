@echo off

::if not linked egavga.bgi 
::     - then simple run "tcc bgidem_.c graphics.lib"  for compile.
::else:


set CNAME=BGIDEM_
set TC=c:\language\tc2
tcc -c -ml %CNAME%.C

:::::c:\language\tc3\bgi\bgiobj.exe %1.chr %1.obj _%1FontProc
:::::c:\language\tc3\bgi\bgiobj.exe %1.bgi %1.obj _%1DriverProc

%TC%\bgi\bgiobj.exe %TC%\bgi\egavga.bgi .\egavga.obj _egavgaDriverProc

if EXIST obj.lst goto obj_ok
echo %TC%\lib\c0l.obj+ EGAVGA.OBJ + %CNAME%.obj      >obj.lst
:obj_ok
if EXIST lib.lst goto lib_ok
echo %TC%\lib\emu.lib %TC%\lib\mathl.lib %TC%\lib\graphics.lib  %TC%\lib\cl.lib >lib.lst
:lib_ok

::::tlink /x /C /c c0l.obj+ EGAVGA.OBJ + %CNAME%.obj ,%CNAME%.exe,  ,emu.lib mathl.lib graphics.lib  cl.lib
    tlink /x /C /c @obj.lst,%CNAME%.exe,,@lib.lst

goto exit









:EXTRACT ALL .OBJects:
tlib  graphics.lib * gload
tlib  graphics.lib * bgiovr
tlib  graphics.lib * graph
tlib  graphics.lib * graphc

:exit
