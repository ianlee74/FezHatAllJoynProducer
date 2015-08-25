set basePath=i:\GitRepos\ianlee74\FezHatAllJoynProducer\FezHat\FezHat
echo BasePath = %basePath%
"C:\Program Files (x86)\Windows Kits\10\bin\x64\alljoyncodegenerator.exe" -i %basePath%\interface.xml -o %basePath%
move %basePath%\com.ianlee.fezHat\*.* %basePath%
rmdir %basePath%\com.ianlee.fezHat
pause