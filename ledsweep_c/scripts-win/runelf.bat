@IF NOT DEFINED APP_HOME_PATH CALL ..\scripts-env\env-win.bat

@CD %APP_HOME_PATH%

@IF "%1"=="debug" GOTO debug

:release
@SET app1_elf=Release//%APP_PROGRAM_NAME1%.elf
@IF NOT EXIST %app1_elf% @SET app1_elf=source//Release//%APP_PROGRAM_NAME1%.elf
@SET ubootspl=%APP_SRC_PATH1%//bsp//u-boot-spl-nocache
@GOTO find_elf_entry_point

:debug
@SET app1_elf=Debug//%APP_PROGRAM_NAME1%.elf
@IF NOT EXIST %app1_elf% @SET app1_elf=source//Debug//%APP_PROGRAM_NAME1%.elf
@SET ubootspl=%APP_SRC_PATH1%//bsp//u-boot-spl-nocache
@GOTO find_elf_entry_point

:find_elf_entry_point
@FOR /F "tokens=4" %%a IN ('arm-none-eabi-readelf -h %app1_elf% ^| FINDSTR "Entry point"') DO @SET user_entry=%%a
::@SET user_entry=0x00100040

:: Reset SoC HPS, load and execute U-Boot SPL elf
openocd -f interface/altera-usb-blaster2.cfg -f target/altera_fpgasoc_de.cfg -c "init; halt; c5_reset; halt; c5_spl %ubootspl%; sleep 200; halt; arm core_state arm; load_image %app1_elf%; resume %user_entry%; shutdown"
@IF %errorlevel% NEQ 0 GOTO :err_handler

@GOTO :end_of_script

:err_handler
:: If run from double-click
@IF /I %0 EQU "%~dpnx0" @PAUSE

:end_of_script
