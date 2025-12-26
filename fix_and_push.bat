@echo off
cd /d "%~dp0"

echo Pushing to GitHub...

:: Add remote
git remote remove origin 2>nul
git remote add origin https://github.com/CAPTCHA-Ma/DeadCells-Clone.git

:: Rename branch to main and push
git branch -M main
git push -u origin main:newbranch --force

echo.
echo Done!
pause
