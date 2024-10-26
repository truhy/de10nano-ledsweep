SET SOFFILE="..\..\..\ledsweep_verilog\output_files\ledsweep.sof"
@IF EXIST %SOFFILE% (
    copy %SOFFILE%
) ELSE (
    SET errorlevel=1
    ECHO File does not exist: %SOFFILE%
)
@IF %errorlevel% NEQ 0 GOTO :err_handler

@GOTO :end_of_script

:err_handler
:: If run from double-click
@IF /I %0 EQU "%~dpnx0" @PAUSE

:end_of_script
