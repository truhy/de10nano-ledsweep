@ECHO OFF
IF NOT DEFINED APP_HOME_PATH CALL ..\scripts-env\env-win.bat

CD "%APP_HOME_PATH%"

SET SOFFILE="..\%FPGA_SRC_PATH%\output_files\%FPGA_PROGRAM_NAME%.sof"
IF EXIST %SOFFILE% (
    copy %SOFFILE% %APP_SRC_PATH1%\bsp
) ELSE (
    SET errorlevel=1
    ECHO File does not exist: %SOFFILE%
)
IF %errorlevel% NEQ 0 GOTO :err_handler

GOTO :end_of_script

:err_handler
:: If run from double-click
IF /I %0 EQU "%~dpnx0" @PAUSE

:end_of_script
