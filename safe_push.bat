@echo off
chcp 65001 >nul
cd /d "%~dp0"

echo ========================================
echo 安全推送脚本 - 不会覆盖历史记录
echo ========================================
echo.

:: 设置你的信息
set GITHUB_USER=KBQ118
set /p GITHUB_TOKEN=请输入你的 GitHub Token: 

echo.
echo Step 1: 配置远程仓库...
git remote remove origin 2>nul
git remote add origin https://%GITHUB_USER%:%GITHUB_TOKEN%@github.com/CAPTCHA-Ma/DeadCells-Clone.git

echo Step 2: 获取远程最新代码...
git fetch origin newbranch

echo Step 3: 创建临时分支保存你的改动...
git stash

echo Step 4: 切换到远程分支...
git checkout -B newbranch origin/newbranch

echo Step 5: 恢复你的改动...
git stash pop

echo Step 6: 添加所有改动...
git add .

echo Step 7: 提交改动...
set /p COMMIT_MSG=请输入提交说明: 
git commit -m "%COMMIT_MSG%"

echo Step 8: 推送到远程 (不会覆盖历史)...
git push origin newbranch

echo.
echo ========================================
echo 完成！你的改动已经添加到现有历史记录之上
echo ========================================

pause
