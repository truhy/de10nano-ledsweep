@IF NOT DEFINED SCRIPT_PATH CALL ..\scripts-env\env-win.bat

make -C ../scripts-generic -f Makefile-elf-info.mk debug

@PAUSE
