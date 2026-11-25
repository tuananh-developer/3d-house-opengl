@echo off
setlocal
cd /d "%~dp0"

echo ===================================================
echo     3D HOUSE OPENGL - MODERN LUXURY VILLA 3D
echo ===================================================

if not exist "build\3d-house-opengl.exe" (
    echo [INFO] Dang bien dich du an lan dau tien...
    cmake -S . -B build
    cmake --build build --parallel
    if not exist "build\libfreeglut.dll" (
        copy /y "D:\g\mingw64\minGW\mingw64\bin\libfreeglut.dll" "build\" >nul 2>&1
    )
)

if exist "build\3d-house-opengl.exe" (
    echo [INFO] Dang khoi chay cua so 3D House OpenGL...
    start "" "build\3d-house-opengl.exe"
) else (
    echo [ERROR] Khong tim thay file thuc thi build\3d-house-opengl.exe.
    pause
)
