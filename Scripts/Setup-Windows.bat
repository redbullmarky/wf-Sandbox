@echo off
pushd ..
Vendor\Binaries\Premake\Windows\premake5.exe --verbose --file=Build.lua vs2022
popd
pause