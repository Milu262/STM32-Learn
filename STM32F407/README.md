# ARM GNU Toolchain 安装指南

在使用该工程时，可能需要下载 `arm-gnu-toolchain-14.3.Rel1-x86_64-arm-none-eabi.tar.xz` 工具链，并解压到指定目录。

## 下载工具链
使用以下命令获取该工具链：
```bash
wget https://developer.arm.com/-/media/Files/downloads/gnu/14.3.rel1/binrel/arm-gnu-toolchain-14.3.Rel1-x86_64-arm-none-eabi.tar.xz
```
🔗 官方下载地址：[ARM GNU Toolchain 14.3.Rel1](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads)
## 解压到/opt/目录
执行以下命令将工具链解压到系统目录：
```bash
sudo tar -xf arm-gnu-toolchain-14.3.Rel1-x86_64-arm-none-eabi.tar.xz -C /opt/
```
解压完成后，工具链的根目录为：
```bash
/opt/arm-gnu-toolchain-14.3.Rel1-x86_64-arm-none-eabi/
```
## 添加环境变量
将工具链的` bin `目录添加到当前用户的` PATH `环境变量中：
```bash
echo 'export PATH="/opt/arm-gnu-toolchain-14.3.rel1-x86_64-arm-none-eabi/bin:$PATH"' >> ~/.bashrc
source ~/.bashrc
```
💡 说明：

~/.bashrc 适用于 Bash 用户（大多数 Linux 默认 shell）。
如果你使用 Zsh，请将命令中的 ~/.bashrc 替换为 ~/.zshrc。

## 验证安装
1. 检查编译器版本
运行以下命令确认 `arm-none-eabi-gcc` 可用：
```bash
arm-none-eabi-gcc --version
```
成功时应输出类似内容：
```test
arm-none-eabi-gcc (GNU Toolchain for the Arm Architecture 14.3.Rel1) 14.2.0
Copyright (C) 2024 Free Software Foundation, Inc.
This is free software; see the source for copying conditions.  There is NO
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
```
2. 验证标准库头文件是否存在
确保 C 标准库头文件可访问（对代码补全和编译很重要）：
```bash
ls /opt/arm-gnu-toolchain-14.3.Rel1-x86_64-arm-none-eabi/arm-none-eabi/include/string.h
```