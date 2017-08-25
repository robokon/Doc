for /f "delims=" %%a in ('CD') do @set current_dir=%%a
CD /d C:\cygwin64\bin
bash --login -i -c "./make.sh"
CD %current_dir%