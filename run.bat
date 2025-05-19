@echo off
setlocal enabledelayedexpansion

rem ========================
rem XÓA THƯ MỤC build (NẾU CÓ)
rem ========================
if exist build (
    echo Dang xoa thu muc build...
    rmdir /s /q build
    echo Da xoa folder build.
	echo.
)

rem ========================
rem LẤY PHIÊN BẢN CMAKE
rem ========================
for /f "tokens=3" %%i in ('cmake --version ^| findstr /i "version"') do set CMAKE_VER=%%i

rem Lọc số phiên bản chính (bỏ phần "-rc2" hoặc từ không cần thiết)
for /f "tokens=1 delims=-" %%a in ("%CMAKE_VER%") do set VERSION=%%a

rem In ra phiên bản CMake
echo.
echo CMake version: %VERSION%
echo.

rem ========================
rem CHẠY CONAN INSTALL
rem ========================
echo.
echo Dang cai bang Conan...
echo.
conan install . --output-folder=build --build=missing --profile:host=default

rem ========================
rem CẤU HÌNH & BUILD DỰ ÁN
rem ========================
cd build

echo.
echo ======= GENERATE CMAKE =======

rem Lấy phần major và minor để so sánh
for /f "tokens=1,2 delims=." %%a in ("%VERSION%") do set MAJOR=%%a & set MINOR=%%b

rem So sánh với 3.23
if %MAJOR% GEQ 3 if %MINOR% GEQ 23 (
	echo.
    echo Dung cmake preset (>= 3.23)
	echo.
    cmake --preset conan-release ..
) else (
	echo.
    echo Dung cmake (< 3.23)
	echo.
    cmake .. -G "MinGW Makefiles" ^
        -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake ^
        -DCMAKE_SH=CMAKE_SH-NOTFOUND ^
        -DCMAKE_POLICY_DEFAULT_CMP0091=NEW ^
        -DCMAKE_BUILD_TYPE=Release
)

rem ========================
rem BUILD DỰ ÁN
rem ========================
echo ====== BUILD PROJECT ======
cmake --build .

endlocal
