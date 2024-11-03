@IF NOT DEFINED APP_HOME_PATH CALL ..\scripts-env\env-win.bat

@CD %APP_HOME_PATH%

@CALL runsof.bat
@IF %errorlevel% NEQ 0 GOTO :err_handler

@GOTO :end_of_script

:err_handler
:: If run from double-click
@IF /I %0 EQU "%~dpnx0" @PAUSE

:end_of_script
