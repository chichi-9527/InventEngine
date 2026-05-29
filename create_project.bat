@echo off
setlocal enabledelayedexpansion

chcp 65001 >nul

:: 切换到 bat 自身所在的目录
cd /d "%~dp0"

:: 检查参数
if "%~1"=="" (
    echo ==============================================
    echo 用法: %~nx0 ^<ProjectName^>
    echo 例如: %~nx0 MyNewApp
    echo ==============================================
    echo 错误: 未指定项目名称，程序退出。
    exit /b 1
)

:: 调用 python 脚本（假设 python 在环境变量中）
python pytools\createProject.py %1

if %errorlevel% neq 0 (
    echo 项目创建失败，请检查控制台输出。
) else (
    echo 项目创建脚本执行完毕。
)

endlocal
exit /b 0