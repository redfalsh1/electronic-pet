#include "studentview.h"
#include <QHeaderView>
#include <QMessageBox>

StudentView::StudentView(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
    setupStyleSheet();
    createConnections();
}

StudentView::~StudentView()
{
}

void StudentView::setupUI()
{
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(15, 15, 15, 15);
    mainLayout->setSpacing(20);
    
    // === 左侧：宠物展示区 ===
    QWidget *leftWidget = new QWidget();
    leftWidget->setMaximumWidth(400);
    QVBoxLayout *leftLayout = new QVBoxLayout(leftWidget);
    leftLayout->setContentsMargins(0, 0, 0, 0);
    leftLayout->setSpacing(15);
    
    // 宠物图片
    petImageLabel = new QLabel();
    petImageLabel->setFixedSize(300, 300);
    petImageLabel->setStyleSheet(
        "QLabel { "
        "background-color: #2d2d2d; "
        "border-radius: 150px; "
        "border: 4px solid #4a9eff; "
        "}"
    );
    petImageLabel->setAlignment(Qt::AlignCenter);
    petImageLabel->setText(tr("宠物\n图片"));
    leftLayout->addWidget(petImageLabel, 0, Qt::AlignHCenter);
    
    // 宠物信息
    petNameLabel = new QLabel(tr("宠物名称：小可爱"));
    petNameLabel->setStyleSheet("QLabel { font-size: 20px; font-weight: bold; color: #ffffff; }");
    petNameLabel->setAlignment(Qt::AlignCenter);
    leftLayout->addWidget(petNameLabel);
    
    petLevelLabel = new QLabel(tr("等级：1"));
    petLevelLabel->setStyleSheet("QLabel { font-size: 16px; color: #ffd700; }");
    petLevelLabel->setAlignment(Qt::AlignCenter);
    leftLayout->addWidget(petLevelLabel);
    
    // 状态信息
    QGroupBox *statusGroupBox = new QGroupBox(tr("宠物状态"));
    QVBoxLayout *statusLayout = new QVBoxLayout(statusGroupBox);
    statusLayout->setSpacing(12);
    
    // 积分
    scoreLabel = new QLabel(tr("积分：0"));
    scoreLabel->setStyleSheet("QLabel { font-size: 18px; color: #ffd700; }");
    statusLayout->addWidget(scoreLabel);
    
    // 粮食
    foodLabel = new QLabel(tr("粮食：0"));
    foodLabel->setStyleSheet("QLabel { font-size: 18px; color: #90EE90; }");
    statusLayout->addWidget(foodLabel);
    
    // 经验值
    expLabel = new QLabel(tr("经验：0 / 100"));
    expLabel->setStyleSheet("QLabel { font-size: 18px; color: #87CEEB; }");
    statusLayout->addWidget(expLabel);
    
    // 经验进度条
    expProgressBar = new QProgressBar();
    expProgressBar->setRange(0, 100);
    expProgressBar->setValue(0);
    expProgressBar->setFixedHeight(20);
    expProgressBar->setStyleSheet(
        "QProgressBar { "
        "background-color: #2d2d2d; "
        "border-radius: 10px; "
        "border: 2px solid #4a9eff; "
        "} "
        "QProgressBar::chunk { "
        "background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "stop:0 #4a9eff, stop:1 #00d4ff); "
        "border-radius: 8px; "
        "} "
    );
    statusLayout->addWidget(expProgressBar);
    
    leftLayout->addWidget(statusGroupBox);
    
    // 操作按钮
    exchangeButton = new QPushButton(tr("积分兑换"));
    exchangeButton->setFixedHeight(45);
    exchangeButton->setStyleSheet(
        "QPushButton { "
        "background-color: #ffd700; "
        "color: #1e1e1e; "
        "font-weight: bold; "
        "font-size: 16px; "
        "} "
        "QPushButton:hover { "
        "background-color: #ffe033; "
        "} "
    );
    leftLayout->addWidget(exchangeButton);
    
    feedPetButton = new QPushButton(tr("喂养宠物"));
    feedPetButton->setFixedHeight(45);
    leftLayout->addWidget(feedPetButton);
    
    leftLayout->addStretch();
    
    // === 右侧：成长记录 ===
    QWidget *rightWidget = new QWidget();
    QVBoxLayout *rightLayout = new QVBoxLayout(rightWidget);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    
    QLabel *recordTitle = new QLabel(tr("成长记录"));
    recordTitle->setStyleSheet("QLabel { font-size: 20px; font-weight: bold; color: #4a9eff; }");
    rightLayout->addWidget(recordTitle);
    
    growthRecordTable = new QTableWidget();
    growthRecordTable->setColumnCount(3);
    growthRecordTable->setHorizontalHeaderLabels({tr("时间"), tr("事件"), tr("获得积分")});
    growthRecordTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    growthRecordTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    growthRecordTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    growthRecordTable->setAlternatingRowColors(true);
    growthRecordTable->verticalHeader()->setVisible(false);
    rightLayout->addWidget(growthRecordTable, 1);
    
    // 添加到主布局
    mainLayout->addWidget(leftWidget);
    mainLayout->addWidget(rightWidget, 1);
    
    // 创建兑换对话框
    exchangeDialog = new ExchangeDialog(this);
}

void StudentView::setupStyleSheet()
{
    // 继承主窗口的样式表
}

void StudentView::createConnections()
{
    connect(exchangeButton, &QPushButton::clicked, this, &StudentView::onExchangeButtonClicked);
    connect(feedPetButton, &QPushButton::clicked, this, &StudentView::onFeedPetClicked);
    connect(exchangeDialog, &ExchangeDialog::accepted, this, &StudentView::onDialogAccepted);
}

void StudentView::updatePetInfo(const QString &petName, const QString &petImage, int level)
{
    petNameLabel->setText(tr("宠物名称：%1").arg(petName));
    petLevelLabel->setText(tr("等级：%1").arg(level));
    
    if (!petImage.isEmpty()) {
        petImageLabel->setPixmap(QPixmap(petImage).scaled(300, 300, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
}

void StudentView::updateStatus(int score, int food, int exp, int maxExp)
{
    scoreLabel->setText(tr("积分：%1").arg(score));
    foodLabel->setText(tr("粮食：%1").arg(food));
    expLabel->setText(tr("经验：%1 / %2").arg(exp).arg(maxExp));
    expProgressBar->setRange(0, maxExp);
    expProgressBar->setValue(exp);
}

void StudentView::updateGrowthRecord(const QList<QString> &records)
{
    growthRecordTable->setRowCount(0);
    
    for (const QString &record : records) {
        int row = growthRecordTable->rowCount();
        growthRecordTable->insertRow(row);
        
        // 简单解析记录格式："时间|事件|积分"
        QStringList parts = record.split("|");
        if (parts.size() >= 3) {
            growthRecordTable->setItem(row, 0, new QTableWidgetItem(parts[0]));
            growthRecordTable->setItem(row, 1, new QTableWidgetItem(parts[1]));
            growthRecordTable->setItem(row, 2, new QTableWidgetItem(parts[2]));
        } else {
            growthRecordTable->setItem(row, 0, new QTableWidgetItem(record));
        }
    }
}

void StudentView::onExchangeButtonClicked()
{
    exchangeDialog->show();
    exchangeDialog->raise();
    exchangeDialog->activateWindow();
}

void StudentView::onFeedPetClicked()
{
    // TODO: 实现喂养宠物逻辑
    QMessageBox::information(this, tr("提示"), tr("宠物喂养功能开发中..."));
}

void StudentView::onDialogAccepted()
{
    int itemType = exchangeDialog->getSelectedItemType();
    int quantity = exchangeDialog->getQuantity();
    
    if (itemType > 0 && quantity > 0) {
        emit requestExchange(itemType, quantity);
        emit operationComplete();
    }
}
