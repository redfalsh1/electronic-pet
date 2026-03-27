#!/usr/bin/env pwsh
# EDA Desktop Application - 本地构建脚本
# 使用方法：.\build.ps1 [-Config Release|Debug] [-Clean] [-Test]

param(
    [ValidateSet("Release", "Debug")]
    [string]$Config = "Release",
    
    [switch]$Clean,
    
    [switch]$Test,
    
    [switch]$Install
)

$BuildDir = "build"
$InstallDir = "install"

Write-Host "====================================" -ForegroundColor Cyan
Write-Host "  EDA Desktop Build Script" -ForegroundColor Cyan
Write-Host "====================================" -ForegroundColor Cyan
Write-Host ""

# 清理
if ($Clean) {
    Write-Host "清理构建目录..." -ForegroundColor Yellow
    if (Test-Path $BuildDir) {
        Remove-Item -Recurse -Force $BuildDir
    }
    if (Test-Path $InstallDir) {
        Remove-Item -Recurse -Force $InstallDir
    }
}

# 创建构建目录
if (-not (Test-Path $BuildDir)) {
    New-Item -ItemType Directory -Path $BuildDir | Out-Null
}

# CMake 配置
Write-Host "配置 CMake ($Config)..." -ForegroundColor Green
Push-Location PetEduComplete
try {
    cmake -S . -B ../$BuildDir `
        -DCMAKE_BUILD_TYPE=$Config `
        -DCMAKE_INSTALL_PREFIX=../$InstallDir `
        -DPET_CORE_BUILD_TESTS=ON
    
    if ($LASTEXITCODE -ne 0) {
        throw "CMake 配置失败"
    }
} finally {
    Pop-Location
}

# 构建
Write-Host "构建中..." -ForegroundColor Green
cmake --build $BuildDir --config $Config --parallel
if ($LASTEXITCODE -ne 0) {
    Write-Host "构建失败!" -ForegroundColor Red
    exit 1
}

# 测试
if ($Test) {
    Write-Host "运行测试..." -ForegroundColor Green
    ctest --test-dir $BuildDir -C $Config --output-on-failure
    if ($LASTEXITCODE -ne 0) {
        Write-Host "测试失败!" -ForegroundColor Red
        exit 1
    }
}

# 安装
if ($Install) {
    Write-Host "安装到 $InstallDir..." -ForegroundColor Green
    cmake --install $BuildDir --config $Config
    if ($LASTEXITCODE -ne 0) {
        Write-Host "安装失败!" -ForegroundColor Red
        exit 1
    }
}

Write-Host ""
Write-Host "✅ 构建完成!" -ForegroundColor Green
Write-Host ""
Write-Host "输出目录: $BuildDir" -ForegroundColor Cyan
Write-Host "安装目录: $InstallDir" -ForegroundColor Cyan
Write-Host ""
