@echo off
rem 自动构建并通过 CMSIS-DAP (pyOCD) 烧录脚本
rem 请根据你的环境修改 IARBUILD 路径与 BUILD_CONFIG


































































exit /b 0pause
necho 烧写成功！)  exit /b 8  pause  echo 未找到可烧写的文件（HEX/ELF），请检查构建输出目录。) else (  )    exit /b 7    pause    echo 烧写 ELF 失败。  if errorlevel 1 (  pyocd flash "%ELF_CANDIDATE2%"  echo 使用 pyOCD 烧写 ELF...  echo 找到 ELF：%ELF_CANDIDATE2%) else if exist "%ELF_CANDIDATE2%" (  )    exit /b 6    pause    echo 烧写 OUT/ELF 失败。  if errorlevel 1 (  pyocd flash "%ELF_CANDIDATE%"  echo 使用 pyOCD 烧写 OUT 文件...  echo 找到 OUT/ELF：%ELF_CANDIDATE%) else if exist "%ELF_CANDIDATE%" (  )    exit /b 5    pause    echo 烧写 HEX 失败。  if errorlevel 1 (  pyocd flash "%HEX_CANDIDATE%"  echo 使用 pyOCD 烧写 HEX...  echo 找到 HEX：%HEX_CANDIDATE%
:: 找到可用的文件并烧录
nif exist "%HEX_CANDIDATE%" ()  exit /b 4  pause  echo    pip install pyocd  echo 未检测到 pyocd，请先安装 Python 并运行：if errorlevel 1 (where pyocd >nul 2>&1
n:: 确认 pyocd 是否安装)  exit /b 3  pause  echo 构建失败，请在 IAR 中检查错误。if errorlevel 1 ("%IARBUILD%" "%WORKSPACE%" -build "%BUILD_CONFIG%"
necho 正在构建工程...)  exit /b 2  pause  echo 请编辑此脚本将变量 IARBUILD 指向你本机的 IarBuild.exe  echo IarBuild.exe 未在路径找到：%IARBUILD%if not exist "%IARBUILD%" (
:check_iarbuildset "ELF_CANDIDATE2=d:\SmartCar\mine\JLNU_SmartCar_TwoLeg\E01_gpio_demo\Debug_m7_0\Exe\project.elf"set "ELF_CANDIDATE=d:\SmartCar\mine\JLNU_SmartCar_TwoLeg\E01_gpio_demo\Debug_m7_0\Exe\cyt4bb7_cm_7_0.out"set "HEX_CANDIDATE=d:\SmartCar\mine\JLNU_SmartCar_TwoLeg\E01_gpio_demo\iar\project_config\cyt4bb7_cm_7_0.hex"
:: 生成文件的常见位置（脚本会检查并尝试可用的一个）set "BUILD_CONFIG=Debug"set "WORKSPACE=d:\SmartCar\mine\JLNU_SmartCar_TwoLeg\E01_gpio_demo\iar\cyt4bb7.eww"set "IARBUILD=C:\Program Files (x86)\IAR Systems\Embedded Workbench 8.40\common\bin\IarBuild.exe":: 配置项 - 如有需要请修改