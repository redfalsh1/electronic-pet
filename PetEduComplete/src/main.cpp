#include <QApplication>
#include <QMainWindow>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QProgressBar>
#include <QStackedWidget>
#include <QMessageBox>
#include <QFile>
#include <QFont>
#include <QTimer>

// 前向声明
class TeacherPanel;
class StudentView;

// 主窗口
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr) : QMainWindow(parent) {
        setWindowTitle("PetEdu - 电子宠物教学系统");
        setMinimumSize(1024, 768);

        // 加载样式表
        QFile styleFile(":/styles/dark_theme.qss");
        if (styleFile.open(QFile::ReadOnly)) {
            QString styleSheet = QLatin1String(styleFile.readAll());
            qApp->setStyleSheet(styleSheet);
            styleFile.close();
        }

        // 中央控件
        QWidget *centralWidget = new QWidget(this);
        setCentralWidget(centralWidget);
        QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

        // 顶部状态栏
        QWidget *statusBar = new QWidget();
        QHBoxLayout *statusLayout = new QHBoxLayout(statusBar);
        
        petImageLabel = new QLabel();
        petImageLabel->setFixedSize(150, 150);
        petImageLabel->setStyleSheet(
            "QLabel { background-color: #4a9eff; border-radius: 75px; }"
        );
        petImageLabel->setAlignment(Qt::AlignCenter);
        petImageLabel->setText("🐾");
        petImageLabel->setFont(QFont("Microsoft YaHei", 48));
        
        // 状态信息
        QWidget *infoPanel = new QWidget();
        QVBoxLayout *infoLayout = new QVBoxLayout(infoPanel);
        
        pointsLabel = new QLabel("积分：0");
        pointsLabel->setStyleSheet("color: #ffd700; font-size: 20px; font-weight: bold;");
        
        foodLabel = new QLabel("粮食：0");
        foodLabel->setStyleSheet("color: #90EE90; font-size: 20px; font-weight: bold;");
        
        expLabel = new QLabel("经验：0/100");
        expLabel->setStyleSheet("color: #87CEEB; font-size: 20px; font-weight: bold;");
        
        infoLayout->addWidget(pointsLabel);
        infoLayout->addWidget(foodLabel);
        infoLayout->addWidget(expLabel);
        
        // 成长进度条
        growthProgress = new QProgressBar();
        growthProgress->setRange(0, 100);
        growthProgress->setValue(0);
        growthProgress->setFixedHeight(25);
        growthProgress->setStyleSheet(
            "QProgressBar { background-color: #2d2d2d; border-radius: 12px; }"
            "QProgressBar::chunk { background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
            "stop:0 #4a9eff, stop:1 #90EE90); border-radius: 12px; }"
        );
        
        statusLayout->addWidget(petImageLabel);
        statusLayout->addWidget(infoPanel);
        statusLayout->addStretch();
        
        mainLayout->addWidget(statusBar);
        mainLayout->addWidget(growthProgress);

        // 视图切换
        stackedWidget = new QStackedWidget();
        
        studentView = new StudentView(this);
        teacherPanel = new TeacherPanel(this);
        
        stackedWidget->addWidget(studentView);
        stackedWidget->addWidget(teacherPanel);
        
        mainLayout->addWidget(stackedWidget);

        // 切换按钮
        QWidget *buttonPanel = new QWidget();
        QHBoxLayout *buttonLayout = new QHBoxLayout(buttonPanel);
        
        QPushButton *toStudentBtn = new QPushButton("学生视图");
        QPushButton *toTeacherBtn = new QPushButton("教师面板");
        
        connect(toStudentBtn, &QPushButton::clicked, [this]() {
            stackedWidget->setCurrentIndex(0);
        });
        
        connect(toTeacherBtn, &QPushButton::clicked, [this]() {
            stackedWidget->setCurrentIndex(1);
        });
        
        buttonLayout->addStretch();
        buttonLayout->addWidget(toStudentBtn);
        buttonLayout->addWidget(toTeacherBtn);
        
        mainLayout->addWidget(buttonPanel);

        // 初始化数据
        petLevel = 1;
        petExp = 0;
        petPoints = 0;
        petFood = 0;
        
        updateDisplay();
        
        // 定时器更新时间
        QTimer *timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &MainWindow::updateOverTime);
        timer->start(60000); // 每分钟更新
    }

public slots:
    void addPoints(int amount, const QString &reason) {
        petPoints += amount;
        updateDisplay();
        QMessageBox::information(this, "获得积分", QString("因%1获得%2积分！").arg(reason).arg(amount));
    }

    void addFood(int amount) {
        petFood += amount;
        updateDisplay();
    }

    void feedPet() {
        if (petFood > 0) {
            petFood--;
            petExp += 5;
            checkLevelUp();
            updateDisplay();
            QMessageBox::information(this, "喂食", "宠物吃了粮食，经验 +5！");
        } else {
            QMessageBox::warning(this, "提示", "粮食不足！");
        }
    }

    void exchangePointsToFood(int pointsAmount) {
        if (petPoints >= pointsAmount) {
            petPoints -= pointsAmount;
            int foodAmount = pointsAmount / 10;
            petFood += foodAmount;
            updateDisplay();
            QMessageBox::information(this, "兑换成功", 
                QString("使用%1积分兑换了%2粮食！").arg(pointsAmount).arg(foodAmount));
        } else {
            QMessageBox::warning(this, "积分不足", "积分不够兑换！");
        }
    }

private slots:
    void updateDisplay() {
        pointsLabel->setText(QString("积分：%1").arg(petPoints));
        foodLabel->setText(QString("粮食：%1").arg(petFood));
        expLabel->setText(QString("经验：%1/%2").arg(petExp).arg(petLevel * 100));
        growthProgress->setValue(petExp % 100);
        
        // 根据等级更新宠物图片
        QString emoji;
        if (petLevel < 5) emoji = "🥚";
        else if (petLevel < 15) emoji = "🐣";
        else if (petLevel < 30) emoji = "🐤";
        else if (petLevel < 50) emoji = "🐔";
        else emoji = "🦚";
        
        petImageLabel->setText(emoji);
        petImageLabel->setFont(QFont("Microsoft YaHei", 48));
    }

    void checkLevelUp() {
        int expNeeded = petLevel * 100;
        while (petExp >= expNeeded) {
            petExp -= expNeeded;
            petLevel++;
            petLevel = qMin(petLevel, 60); // 最高 60 级
            QMessageBox::information(this, "升级！", 
                QString("恭喜！宠物升到%1级！").arg(petLevel));
            expNeeded = petLevel * 100;
        }
    }

    void updateOverTime() {
        // 每分钟自动减少饥饿等（简化版）
    }

private:
    QLabel *petImageLabel;
    QLabel *pointsLabel;
    QLabel *foodLabel;
    QLabel *expLabel;
    QProgressBar *growthProgress;
    QStackedWidget *stackedWidget;
    StudentView *studentView;
    TeacherPanel *teacherPanel;
    
    int petLevel;
    int petExp;
    int petPoints;
    int petFood;
};

// 学生视图
class StudentView : public QWidget {
    Q_OBJECT
public:
    StudentView(MainWindow *mainWnd, QWidget *parent = nullptr) 
        : QWidget(parent), mainWindow(mainWnd) {
        
        QVBoxLayout *layout = new QVBoxLayout(this);
        
        QLabel *titleLabel = new QLabel("🎮 学生中心");
        titleLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: #4a9eff;");
        layout->addWidget(titleLabel);
        
        // 宠物信息卡片
        QWidget *petCard = new QWidget();
        petCard->setStyleSheet("background-color: #2d2d2d; border-radius: 10px; padding: 20px;");
        QVBoxLayout *petLayout = new QVBoxLayout(petCard);
        
        QLabel *petInfoLabel = new QLabel("我的宠物");
        petInfoLabel->setStyleSheet("font-size: 18px; color: #ffffff;");
        petLayout->addWidget(petInfoLabel);
        
        layout->addWidget(petCard);
        
        // 功能按钮
        QPushButton *feedBtn = new QPushButton("🍖 喂食宠物");
        feedBtn->setFixedHeight(50);
        feedBtn->setStyleSheet(
            "QPushButton { background-color: #4a9eff; color: white; "
            "font-size: 16px; border-radius: 8px; }"
            "QPushButton:hover { background-color: #5aafff; }"
        );
        connect(feedBtn, &QPushButton::clicked, mainWindow, &MainWindow::feedPet);
        layout->addWidget(feedBtn);
        
        QPushButton *exchangeBtn = new QPushButton("💰 积分兑换");
        exchangeBtn->setFixedHeight(50);
        exchangeBtn->setStyleSheet(
            "QPushButton { background-color: #90EE90; color: #1e1e1e; "
            "font-size: 16px; border-radius: 8px; }"
            "QPushButton:hover { background-color: #a0ff90; }"
        );
        connect(exchangeBtn, &QPushButton::clicked, this, &StudentView::showExchangeDialog);
        layout->addWidget(exchangeBtn);
        
        layout->addStretch();
    }

private slots:
    void showExchangeDialog() {
        QDialog dialog(this);
        dialog.setWindowTitle("积分兑换");
        dialog.setMinimumWidth(400);
        
        QVBoxLayout *layout = new QVBoxLayout(&dialog);
        
        QLabel *titleLabel = new QLabel("请选择兑换物品");
        titleLabel->setStyleSheet("font-size: 18px; font-weight: bold;");
        layout->addWidget(titleLabel);
        
        // 粮食兑换
        QPushButton *foodBtn = new QPushButton("🍖 10 积分 = 1 粮食");
        foodBtn->setFixedHeight(40);
        connect(foodBtn, &QPushButton::clicked, [&]() {
            mainWindow->exchangePointsToFood(10);
            dialog.accept();
        });
        layout->addWidget(foodBtn);
        
        // 批量兑换
        QPushButton *bulkBtn = new QPushButton("🎁 50 积分 = 6 粮食 (优惠)");
        bulkBtn->setFixedHeight(40);
        connect(bulkBtn, &QPushButton::clicked, [&]() {
            mainWindow->exchangePointsToFood(50);
            dialog.accept();
        });
        layout->addWidget(bulkBtn);
        
        QPushButton *cancelBtn = new QPushButton("取消");
        connect(cancelBtn, &QPushButton::clicked, [&]() { dialog.reject(); });
        layout->addWidget(cancelBtn);
        
        dialog.exec();
    }

private:
    MainWindow *mainWindow;
};

// 教师面板
class TeacherPanel : public QWidget {
    Q_OBJECT
public:
    TeacherPanel(MainWindow *mainWnd, QWidget *parent = nullptr)
        : QWidget(parent), mainWindow(mainWnd) {
        
        QVBoxLayout *layout = new QVBoxLayout(this);
        
        QLabel *titleLabel = new QLabel("👨‍🏫 教师管理面板");
        titleLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: #ffd700;");
        layout->addWidget(titleLabel);
        
        // 选择学生
        QWidget *studentSelect = new QWidget();
        QHBoxLayout *selectLayout = new QHBoxLayout(studentSelect);
        selectLayout->addWidget(new QLabel("学生:"));
        studentCombo = new QComboBox();
        studentCombo->addItems({"张三", "李四", "王五", "赵六"});
        selectLayout->addWidget(studentCombo);
        selectLayout->addStretch();
        layout->addWidget(studentSelect);
        
        // 发放积分
        QGroupBox *pointsGroup = new QGroupBox("📊 发放积分");
        QVBoxLayout *pointsLayout = new QVBoxLayout(pointsGroup);
        
        QHBoxLayout *reasonLayout = new QHBoxLayout();
        reasonLayout->addWidget(new QLabel("原因:"));
        reasonCombo = new QComboBox();
        reasonCombo->addItems({"课堂答题", "完成作业", "课堂表现", "其他"});
        reasonLayout->addWidget(reasonCombo);
        pointsLayout->addLayout(reasonLayout);
        
        QHBoxLayout *amountLayout = new QHBoxLayout();
        amountLayout->addWidget(new QLabel("积分:"));
        pointsSpin = new QSpinBox();
        pointsSpin->setRange(1, 100);
        pointsSpin->setValue(5);
        amountLayout->addWidget(pointsSpin);
        amountLayout->addStretch();
        pointsLayout->addLayout(amountLayout);
        
        QPushButton *awardBtn = new QPushButton("发放积分");
        awardBtn->setStyleSheet(
            "QPushButton { background-color: #ffd700; color: #1e1e1e; "
            "font-size: 14px; font-weight: bold; padding: 10px; }"
        );
        connect(awardBtn, &QPushButton::clicked, this, &TeacherPanel::awardPoints);
        pointsLayout->addWidget(awardBtn);
        
        layout->addWidget(pointsGroup);
        
        // 发放奖励
        QGroupBox *rewardGroup = new QGroupBox("🎁 发放奖励");
        QVBoxLayout *rewardLayout = new QVBoxLayout(rewardGroup);
        
        QHBoxLayout *rewardBtnLayout = new QHBoxLayout();
        
        QPushButton *foodRewardBtn = new QPushButton("🍖 粮食");
        connect(foodRewardBtn, &QPushButton::clicked, [&]() {
            QString student = studentCombo->currentText();
            QMessageBox::information(this, "发放奖励",
                QString("给%1发放了粮食！").arg(student));
        });
        rewardBtnLayout->addWidget(foodRewardBtn);
        
        QPushButton *propRewardBtn = new QPushButton("✨ 道具");
        connect(propRewardBtn, &QPushButton::clicked, [&]() {
            QString student = studentCombo->currentText();
            QMessageBox::information(this, "发放奖励",
                QString("给%1发放了道具！").arg(student));
        });
        rewardBtnLayout->addWidget(propRewardBtn);
        
        rewardLayout->addLayout(rewardBtnLayout);
        layout->addWidget(rewardGroup);
        
        layout->addStretch();
    }

private slots:
    void awardPoints() {
        QString student = studentCombo->currentText();
        QString reason = reasonCombo->currentText();
        int points = pointsSpin->value();
        
        QMessageBox::information(this, "发放成功",
            QString("给%1发放了%2积分（原因：%3）")
                .arg(student).arg(points).arg(reason));
        
        // 通知主窗口（简化版直接显示）
    }

private:
    MainWindow *mainWindow;
    QComboBox *studentCombo;
    QComboBox *reasonCombo;
    QSpinBox *pointsSpin;
};

#include "mainwindow.moc"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // 设置应用程序信息
    QCoreApplication::setOrganizationName("PetEdu");
    QCoreApplication::setApplicationName("PetEdu");
    QCoreApplication::setApplicationVersion("1.0.0");
    
    MainWindow window;
    window.show();
    
    return app.exec();
}
