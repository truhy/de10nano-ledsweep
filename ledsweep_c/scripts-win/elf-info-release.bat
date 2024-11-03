@IF NOT DEFINED APP_HOME_PATH CALL ..\scripts-env\env-win.bat

make -C ../scripts-generic -f Makefile-elf-info.mk

@PAUSE
