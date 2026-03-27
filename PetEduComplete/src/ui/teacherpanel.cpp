#include "teacherpanel.h"
#include <QHeaderView>
#include <QMessageBox>

TeacherPanel::TeacherPanel(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
    setupStyleSheet();
    createConnections();
}

TeacherPanel::~TeacherPanel()
{
}

void TeacherPanel::setupUI()
{
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(15);
    
    // === 左侧：学生列表 ===
    QWidget *leftWidget = new QWidget();
    leftWidget->setMaximumWidth(300);
    QVBoxLayout *leftLayout = new QVBoxLayout(leftWidget);
    leftLayout->setContentsMargins(0, 0, 0, 0);
    
    // 学生列表标题
    QLabel *listTitle = new QLabel(tr("学生列表"));
    listTitle->setStyleSheet("QLabel { font-size: 18px; font-weight: bold; color: #4a9eff; }");
    leftLayout->addWidget(listTitle);
    
    // 学生列表
    studentListWidget = new QListWidget();
    studentListWidget->setAlternatingRowColors(true);
    leftLayout->addWidget(studentListWidget);
    
    // 刷新按钮
    refreshButton = new QPushButton(tr("刷新列表"));
    leftLayout->addWidget(refreshButton);
    
    leftLayout->addStretch();
    
    // === 右侧：操作面板 ===
    QWidget *rightWidget = new QWidget();
    QVBoxLayout *rightLayout = new QVBoxLayout(rightWidget);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(15);
    
    // 积分发放区域
    pointsGroupBox = new QGroupBox(tr("积分发放"));
    QVBoxLayout *pointsLayout = new QVBoxLayout(pointsGroupBox);
    pointsLayout->setSpacing(10);
    
    // 发放原因选择
    QHBoxLayout *reasonLayout = new QHBoxLayout();
    reasonLayout->addWidget(new QLabel(tr("原因:")));
    reasonComboBox = new QComboBox();
    reasonComboBox->addItems({
        tr("课堂答题"),
        tr("作业优秀"),
        tr("表现突出"),
        tr("进步显著"),
        tr("其他")
    });
    reasonLayout->addWidget(reasonComboBox, 1);
    pointsLayout->addLayout(reasonLayout);
    
    // 积分数量
    QHBoxLayout *pointsLayout2 = new QHBoxLayout();
    pointsLayout2->addWidget(new QLabel(tr("积分:")));
    pointsSpinBox = new QSpinBox();
    pointsSpinBox->setRange(1, 100);
    pointsSpinBox->setValue(5);
    pointsSpinBox->setSuffix(tr(" 分"));
    pointsLayout2->addWidget(pointsSpinBox, 1);
    pointsLayout->addLayout(pointsLayout2);
    
    // 发放按钮
    awardPointsButton = new QPushButton(tr("发放积分"));
    awardPointsButton->setFixedHeight(40);
    pointsLayout->addWidget(awardPointsButton);
    
    rightLayout->addWidget(pointsGroupBox);
    
    // 奖励发放区域
    rewardGroupBox = new QGroupBox(tr("奖励发放"));
    QVBoxLayout *rewardLayout = new QVBoxLayout(rewardGroupBox);
    rewardLayout->setSpacing(10);
    
    // 奖励类型选择
    QHBoxLayout *rewardTypeLayout = new QHBoxLayout();
    rewardTypeLayout->addWidget(new QLabel(tr("类型:")));
    rewardTypeComboBox = new QComboBox();
    rewardTypeComboBox->addItems({
        tr("宠物粮食"),
        tr("装饰道具"),
        tr("经验加成"),
        tr("特殊技能"),
        tr("其他奖励")
    });
    rewardTypeLayout->addWidget(rewardTypeComboBox, 1);
    rewardLayout->addLayout(rewardTypeLayout);
    
    // 发放按钮
    awardRewardButton = new QPushButton(tr("发放奖励"));
    awardRewardButton->setFixedHeight(40);
    rewardLayout->addWidget(awardRewardButton);
    
    rightLayout->addWidget(rewardGroupBox);
    
    // 学生积分表格
    QGroupBox *scoreGroupBox = new QGroupBox(tr("学生积分排行"));
    QVBoxLayout *scoreLayout = new QVBoxLayout(scoreGroupBox);
    
    scoreTableWidget = new QTableWidget();
    scoreTableWidget->setColumnCount(3);
    scoreTableWidget->setHorizontalHeaderLabels({tr("排名"), tr("姓名"), tr("积分")});
    scoreTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    scoreTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    scoreTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    scoreTableWidget->setAlternatingRowColors(true);
    scoreLayout->addWidget(scoreTableWidget);
    
    rightLayout->addWidget(scoreGroupBox, 1);
    
    // 添加到主布局
    mainLayout->addWidget(leftWidget);
    mainLayout->addWidget(rightWidget, 1);
    
    // 初始禁用操作按钮
    awardPointsButton->setEnabled(false);
    awardRewardButton->setEnabled(false);
}

void TeacherPanel::setupStyleSheet()
{
    // 继承主窗口的样式表
}

void TeacherPanel::createConnections()
{
    connect(studentListWidget, &QListWidget::currentItemChanged, this, &TeacherPanel::onStudentSelected);
    connect(awardPointsButton, &QPushButton::clicked, this, &TeacherPanel::onAwardPointsClicked);
    connect(awardRewardButton, &QPushButton::clicked, this, &TeacherPanel::onAwardRewardClicked);
    connect(refreshButton, &QPushButton::clicked, this, &TeacherPanel::refreshStudentList);
}

void TeacherPanel::addStudent(const QString &studentId, const QString &studentName)
{
    QListWidgetItem *item = new QListWidgetItem(studentName);
    item->setData(Qt::UserRole, studentId);
    studentListWidget->addItem(item);
}

void TeacherPanel::removeStudent(const QString &studentId)
{
    for (int i = 0; i < studentListWidget->count(); ++i) {
        QListWidgetItem *item = studentListWidget->item(i);
        if (item->data(Qt::UserRole).toString() == studentId) {
            delete studentListWidget->takeItem(i);
            break;
        }
    }
}

void TeacherPanel::updateStudentScore(const QString &studentId, int score)
{
    // 更新表格中的分数
    for (int row = 0; row < scoreTableWidget->rowCount(); ++row) {
        QTableWidgetItem *idItem = scoreTableWidget->item(row, 0);
        if (idItem && idItem->data(Qt::UserRole).toString() == studentId) {
            QTableWidgetItem *scoreItem = scoreTableWidget->item(row, 2);
            if (scoreItem) {
                scoreItem->setText(QString::number(score));
            }
            break;
        }
    }
}

void TeacherPanel::onStudentSelected()
{
    QListWidgetItem *currentItem = studentListWidget->currentItem();
    if (currentItem) {
        currentStudentId = currentItem->data(Qt::UserRole).toString();
        currentStudentName = currentItem->text();
        awardPointsButton->setEnabled(true);
        awardRewardButton->setEnabled(true);
    } else {
        currentStudentId.clear();
        currentStudentName.clear();
        awardPointsButton->setEnabled(false);
        awardRewardButton->setEnabled(false);
    }
}

void TeacherPanel::onAwardPointsClicked()
{
    if (currentStudentId.isEmpty()) {
        QMessageBox::warning(this, tr("警告"), tr("请先选择学生"));
        return;
    }
    
    int points = pointsSpinBox->value();
    QString reason = reasonComboBox->currentText();
    
    emit awardPoints(currentStudentId, points, reason);
    emit operationComplete();
}

void TeacherPanel::onAwardRewardClicked()
{
    if (currentStudentId.isEmpty()) {
        QMessageBox::warning(this, tr("警告"), tr("请先选择学生"));
        return;
    }
    
    QString rewardType = rewardTypeComboBox->currentText();
    
    emit awardReward(currentStudentId, rewardType);
    emit operationComplete();
}

void TeacherPanel::refreshStudentList()
{
    // TODO: 从后端重新加载学生列表
    QMessageBox::information(this, tr("提示"), tr("学生列表已刷新"));
}
