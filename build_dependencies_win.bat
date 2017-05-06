echo "NOTE: This must run from MSVC Developer console"
call vcvarsall.bat x64

echo "Building lua..."
cd 3rdParty\LuaJIT\src
call msvcbuild.bat
cd ..\..\..\