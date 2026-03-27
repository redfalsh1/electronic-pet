#include "mainwindow.h"
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QApplication>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , currentViewIndex(0)
{
    setupUI();
    setupStatusBar();
    setupStyleSheet();
    createConnections();
    
    // 请求初始宠物数据
    emit requestPetData();
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUI()
{
    // 设置窗口属性
    setWindowTitle(tr("电子宠物 - 课堂激励系统"));
    setMinimumSize(1024, 768);
    
    // 创建主堆叠控件
    mainStack = new QStackedWidget(this);
    
    // 创建中央 widget
    centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(15);
    
    // === 顶部宠物显示区域 ===
    QWidget *petDisplayWidget = new QWidget();
    petDisplayWidget->setMaximumHeight(300);
    QHBoxLayout *petLayout = new QHBoxLayout(petDisplayWidget);
    petLayout->setContentsMargins(20, 20, 20, 20);
    
    // 宠物图片
    petImageLabel = new QLabel();
    petImageLabel->setFixedSize(200, 200);
    petImageLabel->setStyleSheet(
        "QLabel { "
        "background-color: #2d2d2d; "
        "border-radius: 100px; "
        "border: 3px solid #4a9eff; "
        "}"
    );
    petImageLabel->setAlignment(Qt::AlignCenter);
    petImageLabel->setText(tr("宠物\n图片"));
    petLayout->addWidget(petImageLabel);
    
    // 右侧状态信息
    QWidget *statusWidget = new QWidget();
    QVBoxLayout *statusLayout = new QVBoxLayout(statusWidget);
    statusLayout->setSpacing(15);
    
    // 积分显示
    scoreLabel = new QLabel(tr("积分：0"));
    scoreLabel->setStyleSheet("QLabel { font-size: 24px; font-weight: bold; color: #ffd700; }");
    statusLayout->addWidget(scoreLabel);
    
    // 粮食显示
    foodLabel = new QLabel(tr("粮食：0"));
    foodLabel->setStyleSheet("QLabel { font-size: 24px; font-weight: bold; color: #90EE90; }");
    statusLayout->addWidget(foodLabel);
    
    // 经验值显示
    expLabel = new QLabel(tr("经验：0 / 100"));
    expLabel->setStyleSheet("QLabel { font-size: 24px; font-weight: bold; color: #87CEEB; }");
    statusLayout->addWidget(expLabel);
    
    // 成长进度条
    growthProgressBar = new QProgressBar();
    growthProgressBar->setRange(0, 100);
    growthProgressBar->setValue(0);
    growthProgressBar->setFixedHeight(25);
    growthProgressBar->setStyleSheet(
        "QProgressBar { "
        "background-color: #2d2d2d; "
        "border-radius: 12px; "
        "border: 2px solid #4a9eff; "
        "} "
        "QProgressBar::chunk { "
        "background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "stop:0 #4a9eff, stop:1 #00d4ff); "
        "border-radius: 10px; "
        "} "
    );
    statusLayout->addWidget(growthProgressBar);
    
    statusLayout->addStretch();
    petLayout->addWidget(statusWidget, 1);
    
    mainLayout->addWidget(petDisplayWidget);
    
    // === 视图切换区域 ===
    teacherPanel = new TeacherPanel();
    studentView = new StudentView();
    
    mainStack->addWidget(teacherPanel);
    mainStack->addWidget(studentView);
    
    mainLayout->addWidget(mainStack, 1);
    
    setCentralWidget(centralWidget);
    
    // 创建菜单
    QMenuBar *menuBar = this->menuBar();
    QMenu *viewMenu = menuBar->addMenu(tr("视图 (&V)"));
    
    QAction *teacherAction = viewMenu->addAction(tr("教师面板 (&T)"));
    connect(teacherAction, &QAction::triggered, this, [this]() {
        mainStack->setCurrentIndex(0);
        currentViewIndex = 0;
    });
    
    QAction *studentAction = viewMenu->addAction(tr("学生视图 (&S)"));
    connect(studentAction, &QAction::triggered, this, [this]() {
        mainStack->setCurrentIndex(1);
        currentViewIndex = 1;
    });
    
    QMenu *helpMenu = menuBar->addMenu(tr("帮助 (&H)"));
    helpMenu->addAction(tr("关于"))->setMenuRole(QAction::AboutRole);
}

void MainWindow::setupStatusBar()
{
    statusBar = new QStatusBar(this);
    setStatusBar(statusBar);
    statusBar->showMessage(tr("就绪"), 3000);
}

void MainWindow::setupStyleSheet()
{
    // 深色主题样式表
    this->setStyleSheet(
        "QMainWindow { "
        "background-color: #1e1e1e; "
        "} "
        "QWidget { "
        "background-color: #1e1e1e; "
        "color: #ffffff; "
        "font-family: 'Microsoft YaHei', 'Segoe UI', sans-serif; "
        "} "
        "QLabel { "
        "color: #ffffff; "
        "} "
        "QPushButton { "
        "background-color: #4a9eff; "
        "color: white; "
        "border: none; "
        "border-radius: 5px; "
        "padding: 8px 16px; "
        "font-size: 14px; "
        "font-weight: bold; "
        "} "
        "QPushButton:hover { "
        "background-color: #5aafff; "
        "} "
        "QPushButton:pressed { "
        "background-color: #3a8eef; "
        "} "
        "QPushButton:disabled { "
        "background-color: #555555; "
        "color: #888888; "
        "} "
        "QListWidget { "
        "background-color: #2d2d2d; "
        "border: 1px solid #444444; "
        "border-radius: 5px; "
        "padding: 5px; "
        "} "
        "QListWidget::item { "
        "padding: 8px; "
        "border-radius: 3px; "
        "} "
        "QListWidget::item:selected { "
        "background-color: #4a9eff; "
        "} "
        "QListWidget::item:hover { "
        "background-color: #3a3a3a; "
        "} "
        "QComboBox { "
        "background-color: #2d2d2d; "
        "border: 1px solid #444444; "
        "border-radius: 5px; "
        "padding: 5px 10px; "
        "color: white; "
        "} "
        "QComboBox::drop-down { "
        "border: none; "
        "width: 20px; "
        "} "
        "QComboBox QAbstractItemView { "
        "background-color: #2d2d2d; "
        "border: 1px solid #444444; "
        "selection-background-color: #4a9eff; "
        "} "
        "QSpinBox, QDoubleSpinBox { "
        "background-color: #2d2d2d; "
        "border: 1px solid #444444; "
        "border-radius: 5px; "
        "padding: 5px; "
        "color: white; "
        "} "
        "QGroupBox { "
        "background-color: #2d2d2d; "
        "border: 1px solid #444444; "
        "border-radius: 5px; "
        "margin-top: 10px; "
        "padding-top: 10px; "
        "font-weight: bold; "
        "} "
        "QGroupBox::title { "
        "subcontrol-origin: margin; "
        "left: 10px; "
        "padding: 0 5px; "
        "color: #4a9eff; "
        "} "
    );
}

void MainWindow::createConnections()
{
    // 连接教师面板信号
    connect(teacherPanel, &TeacherPanel::awardPoints, this, [this](const QString &studentId, int points, const QString &reason) {
        statusBar->showMessage(tr("已为 %1 奖励 %2 积分 (%3)").arg(studentId).arg(points).arg(reason), 3000);
        // TODO: 发送信号到后端
    });
    
    connect(teacherPanel, &TeacherPanel::awardReward, this, [this](const QString &studentId, const QString &rewardType) {
        statusBar->showMessage(tr("已为 %1 发放奖励：%2").arg(studentId).arg(rewardType), 3000);
        // TODO: 发送信号到后端
    });
    
    // 连接学生视图信号
    connect(studentView, &StudentView::requestExchange, this, [this](int itemType, int quantity) {
        emit requestExchange(itemType, quantity);
    });
    
    // 视图切换完成信号
    connect(teacherPanel, &TeacherPanel::operationComplete, this, &MainWindow::onTeacherPanelComplete);
    connect(studentView, &StudentView::operationComplete, this, &MainWindow::onStudentViewComplete);
}

void MainWindow::updatePetStatus(const QString &petImage, int score, int food, int exp, int maxExp)
{
    // 更新宠物图片
    if (!petImage.isEmpty()) {
        petImageLabel->setPixmap(QPixmap(petImage).scaled(200, 200, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
    
    // 更新状态显示
    scoreLabel->setText(tr("积分：%1").arg(score));
    foodLabel->setText(tr("粮食：%1").arg(food));
    expLabel->setText(tr("经验：%1 / %2").arg(exp).arg(maxExp));
}

void MainWindow::updateGrowthProgress(int current, int maximum)
{
    growthProgressBar->setRange(0, maximum);
    growthProgressBar->setValue(current);
}

void MainWindow::onTeacherPanelComplete()
{
    statusBar->showMessage(tr("教师操作完成"), 2000);
}

void MainWindow::onStudentViewComplete()
{
    statusBar->showMessage(tr("学生操作完成"), 2000);
}
