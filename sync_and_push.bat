@echo off
cd /d "%~dp0"

echo Step 1: Fetch remote...
git fetch origin newbranch

echo Step 2: Reset to remote...
git reset --hard origin/newbranch

echo Step 3: Now copy your modified files back manually
echo Then run: git add . && git commit -m "房间预览场景，包含：房间列表 增加贴图和地板生成" && git push

pause
