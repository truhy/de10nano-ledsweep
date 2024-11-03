@IF NOT DEFINED APP_HOME_PATH CALL ..\scripts-env\env-win.bat

@CD "%APP_HOME_PATH%"

:: Program .sof to the FPGA
:: Parameters: -c 1 = selects J-TAG cable number 1, @2 is referring to device index on the J-TAG chain (1 = HPS SoC CPU, 2 = Cyclone V FPGA)
quartus_pgm -m jtag -c 1 -o "p;%APP_SRC_PATH1%\bsp\%FPGA_PROGRAM_NAME%.sof@2"
@IF %errorlevel% NEQ 0 GOTO :err_handler

@GOTO :end_of_script

:err_handler
:: If run from double-click
@IF /I %0 EQU "%~dpnx0" @PAUSE

:end_of_script
