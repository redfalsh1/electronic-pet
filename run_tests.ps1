# EDA 项目测试运行脚本
# 使用方法：.\run_tests.ps1

Write-Host "====================================" -ForegroundColor Cyan
Write-Host "  EDA 项目测试套件" -ForegroundColor Cyan
Write-Host "====================================" -ForegroundColor Cyan
Write-Host ""

# 配置参数
$BUILD_DIR = "build"
$TEST_DIR = "$BUILD_DIR/tests"

# 检查构建目录
if (-not (Test-Path $BUILD_DIR)) {
    Write-Host "❌ 构建目录不存在，请先运行配置和构建" -ForegroundColor Red
    Write-Host ""
    Write-Host "配置步骤:" -ForegroundColor Yellow
    Write-Host "  1. mkdir $BUILD_DIR" -ForegroundColor Gray
    Write-Host "  2. cd $BUILD_DIR" -ForegroundColor Gray
    Write-Host "  3. cmake .. -DPET_CORE_BUILD_TESTS=ON" -ForegroundColor Gray
    Write-Host "  4. cmake --build ." -ForegroundColor Gray
    exit 1
}

# 进入测试目录
if (Test-Path $TEST_DIR) {
    Set-Location $TEST_DIR
    Write-Host "✅ 测试目录：$TEST_DIR" -ForegroundColor Green
} else {
    Write-Host "❌ 测试可执行文件目录不存在" -ForegroundColor Red
    exit 1
}

Write-Host ""
Write-Host "运行测试..." -ForegroundColor Yellow
Write-Host ""

# 运行所有测试
$testExecutables = @(
    "gdsii_parser_test.exe",
    "core_module_test.exe",
    "integration_test.exe"
)

$testResults = @()
$passedCount = 0
$failedCount = 0

foreach ($test in $testExecutables) {
    if (Test-Path $test) {
        Write-Host "📋 运行：$test" -ForegroundColor Cyan
        
        $result = & ".$test" --gtest_brief=1 2>&1
        $exitCode = $LASTEXITCODE
        
        if ($exitCode -eq 0) {
            Write-Host "   ✅ 通过" -ForegroundColor Green
            $passedCount++
        } else {
            Write-Host "   ❌ 失败" -ForegroundColor Red
            $failedCount++
        }
        
        $testResults += [PSCustomObject]@{
            Test = $test
            Status = if ($exitCode -eq 0) { "PASS" } else { "FAIL" }
            ExitCode = $exitCode
        }
    } else {
        Write-Host "⚠️  跳过：$test (未找到)" -ForegroundColor Yellow
    }
}

Write-Host ""
Write-Host "====================================" -ForegroundColor Cyan
Write-Host "  测试汇总" -ForegroundColor Cyan
Write-Host "====================================" -ForegroundColor Cyan
Write-Host ""

# 显示测试结果表格
$testResults | Format-Table -AutoSize

Write-Host "通过：$passedCount" -ForegroundColor Green
Write-Host "失败：$failedCount" -ForegroundColor $(if ($failedCount -eq 0) { "Green" } else { "Red" })
Write-Host ""

# 生成测试报告
$timestamp = Get-Date -Format "yyyy-MM-dd_HH-mm-ss"
$reportFile = "test_report_$timestamp.md"

$reportContent = @"
# EDA 项目测试报告

**生成时间**: $(Get-Date -Format "yyyy-MM-dd HH:mm:ss")
**测试人员**: EDA-QA
**分支**: $(git -C ../.. branch --show-current 2>$null)

## 测试汇总

| 测试项 | 状态 | 退出码 |
|--------|------|--------|
"@

foreach ($r in $testResults) {
    $statusIcon = if ($r.Status -eq "PASS") { "✅" } else { "❌" }
    $reportContent += "| $($r.Test) | $statusIcon $($r.Status) | $($r.ExitCode) |`n"
}

$reportContent += @"

## 统计

- **通过**: $passedCount
- **失败**: $failedCount
- **总计**: $($passedCount + $failedCount)
- **通过率**: $([math]::Round($passedCount / ($passedCount + $failedCount) * 100, 2))%

## 备注

_测试报告自动生成_
"@

$reportContent | Out-File -FilePath $reportFile -Encoding UTF8
Write-Host "📄 测试报告已生成：$reportFile" -ForegroundColor Green

Write-Host ""
Write-Host "====================================" -ForegroundColor Cyan

# 返回原始目录
Set-Location ../..

# 退出码
if ($failedCount -eq 0) {
    Write-Host "✅ 所有测试通过！" -ForegroundColor Green
    exit 0
} else {
    Write-Host "❌ 部分测试失败，请检查" -ForegroundColor Red
    exit 1
}
