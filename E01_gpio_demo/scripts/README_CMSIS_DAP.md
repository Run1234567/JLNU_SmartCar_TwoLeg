说明：通过 CMSIS-DAP (pyOCD) 自动构建并烧写

用途：
- 本脚本 `flash_cmsisdap.bat` 会：
  1) 调用 IAR 的 `IarBuild.exe` 编译 `E01_gpio_demo` 工程（`cyt4bb7.eww`）
  2) 使用 `pyocd` 通过 CMSIS-DAP 将生成的 `HEX` / `ELF` 烧写到板子

使用前提：
- 已安装 IAR Embedded Workbench 并能通过 `IarBuild.exe` 命令行构建（请在脚本中修改 `IARBUILD` 变量为你的安装路径）。
- 已安装 Python 与 `pyocd`：
  - 安装命令：`pip install pyocd`
- 开发板通过 CMSIS-DAP (DAPLink) 连接电脑，并已通电。

使用方法：
1. 打开 `d:\SmartCar\mine\JLNU_SmartCar_TwoLeg\E01_gpio_demo\scripts\flash_cmsisdap.bat`，根据你的机器调整 `IARBUILD` 的路径（如果 IAR 版本不同，只需改该路径）。
2. 在 Windows 的 `cmd` 中运行：
```
cd d:\SmartCar\mine\JLNU_SmartCar_TwoLeg\E01_gpio_demo\scripts
flash_cmsisdap.bat
```
3. 脚本会先构建项目，成功后尝试用 `pyocd` 烧写 `HEX` 或 `ELF` 文件，完成后会提示成功或失败原因。

常见问题：
- 若 `pyocd` 找不到目标或不支持该芯片，请优先使用 IAR GUI 中的 `Download and Debug`（Project -> Options -> Debugger 选择 CMSIS-DAP）。
- 若构建产物文件名与脚本候选路径不一致，请在脚本中修改 `HEX_CANDIDATE` / `ELF_CANDIDATE` 为实际生成的文件路径。

如需我代为生成针对 IAR GUI 的具体调试器设置截图说明或其他下载工具（如 SEGGER/J-Link）的脚本，我可以继续生成。