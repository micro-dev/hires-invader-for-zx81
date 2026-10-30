@echo off
rem  Build Ssingle-invader demo for the ZX81 with z88dk
rem  (z88dk must be installed and its bin folder on your PATH)

set OPTS=+zx81 -subtype=wrx -clib=wrx -O2 -Cz--collapsed
set LIB=zx81lib/screen.c zx81lib/sprite.c zx81lib/text.c zx81lib/input.c zx81lib/timing.c


zcc %OPTS% singleInvader.c zx81lib/screen.c zx81lib/sprite.c -o singleInvader -create-app
if errorlevel 1 goto failed

echo Built singleInvader.P - load it into EightyOne with WRX hi-res on
goto end

:failed
echo Build FAILED

:end
pause
