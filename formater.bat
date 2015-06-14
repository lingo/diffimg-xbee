cd src
dir   /b  *.cpp > lfiles.txt
dir   /b  *.h >> lfiles.txt
uncrustify.exe -F lfiles.txt --no-backup -c ..\uncrustify.cfg
del lfiles.txt
cd metrics
dir   /b  *.cpp *.h > lfiles.txt
uncrustify.exe -F lfiles.txt --no-backup -c ..\..\uncrustify.cfg
del lfiles.txt
cd ..
pause
