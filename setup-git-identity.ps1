# EDA 团队 Git 快速配置脚本
# 使用方法：根据角色运行对应的配置

Write-Host "====================================" -ForegroundColor Cyan
Write-Host "  EDA 团队 Git 配置工具" -ForegroundColor Cyan
Write-Host "====================================" -ForegroundColor Cyan
Write-Host ""

# 显示角色选择菜单
Write-Host "请选择您的角色:" -ForegroundColor Yellow
Write-Host "  [1] C++ 开发工程师" -ForegroundColor White
Write-Host "  [2] Qt 界面工程师" -ForegroundColor White
Write-Host "  [3] 质量工程师 (QA)" -ForegroundColor White
Write-Host "  [4] DevOps 工程师" -ForegroundColor White
Write-Host "  [5] 技术负责人" -ForegroundColor White
Write-Host ""

$choice = Read-Host "请输入选项 (1-5)"

$repoRoot = Split-Path -Parent $MyInvocation.MyCommand.Path
Set-Location $repoRoot

switch ($choice) {
    "1" {
        $name = "EDA-CPP-Dev"
        $email = "cpp-dev@eda-team.local"
        $role = "C++ 开发工程师"
    }
    "2" {
        $name = "EDA-Qt-Dev"
        $email = "qt-dev@eda-team.local"
        $role = "Qt 界面工程师"
    }
    "3" {
        $name = "EDA-QA"
        $email = "qa@eda-team.local"
        $role = "质量工程师"
    }
    "4" {
        $name = "EDA-DevOps"
        $email = "devops@eda-team.local"
        $role = "DevOps 工程师"
    }
    "5" {
        $name = "EDA-TechLead"
        $email = "tech-lead@eda-team.local"
        $role = "技术负责人"
    }
    default {
        Write-Host "无效的选项！" -ForegroundColor Red
        exit 1
    }
}

Write-Host ""
Write-Host "正在为 $role 配置 Git 身份..." -ForegroundColor Green

# 配置本地 Git 身份（仅当前仓库）
git config user.name $name
git config user.email $email

# 验证配置
Write-Host ""
Write-Host "✅ 配置完成！" -ForegroundColor Green
Write-Host ""
Write-Host "当前配置:" -ForegroundColor Cyan
Write-Host "  用户名：$name" -ForegroundColor White
Write-Host "  邮箱：$email" -ForegroundColor White
Write-Host ""

# 显示配置信息
git config user.name
git config user.email

Write-Host ""
Write-Host "====================================" -ForegroundColor Cyan
Write-Host "  提交信息规范提示" -ForegroundColor Cyan
Write-Host "====================================" -ForegroundColor Cyan
Write-Host ""

$prefix = switch ($choice) {
    "1" { "[CPP]" }
    "2" { "[QT]" }
    "3" { "[QA]" }
    "4" { "[DEVOPS]" }
    "5" { "[LEAD]" }
}

Write-Host "您的提交信息前缀：$prefix" -ForegroundColor Yellow
Write-Host ""
Write-Host "示例:" -ForegroundColor White
Write-Host "  $prefix feat: 实现新功能" -ForegroundColor Gray
Write-Host "  $prefix fix: 修复 Bug" -ForegroundColor Gray
Write-Host "  $prefix refactor: 重构代码" -ForegroundColor Gray
Write-Host ""

Write-Host "====================================" -ForegroundColor Cyan
Write-Host "  下一步操作" -ForegroundColor Cyan
Write-Host "====================================" -ForegroundColor Cyan
Write-Host ""
Write-Host "1. 从 dev 分支创建功能分支:" -ForegroundColor White
Write-Host "   git checkout dev" -ForegroundColor Gray
Write-Host "   git checkout -b feature/$($choice.ToLower())/your-feature" -ForegroundColor Gray
Write-Host ""
Write-Host "2. 开发完成后提交:" -ForegroundColor White
Write-Host "   git add ." -ForegroundColor Gray
Write-Host "   git commit -m `"$prefix feat: 你的功能描述`"" -ForegroundColor Gray
Write-Host ""
Write-Host "3. 推送到远程:" -ForegroundColor White
Write-Host "   git push -u origin feature/$($choice.ToLower())/your-feature" -ForegroundColor Gray
Write-Host ""

Write-Host "配置完成！祝工作顺利！🚀" -ForegroundColor Green
Write-Host ""
