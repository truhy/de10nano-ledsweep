%QUARTUS_ROOTDIR%/bin64/quartus_pgm -m jtag -c 1 -o "p;ledsweep.sof@2"
@IF %errorlevel% NEQ 0 GOTO :err_handler

@GOTO :end_of_script

:err_handler
:: If run from double-click
@IF /I %0 EQU "%~dpnx0" @PAUSE

:end_of_script
