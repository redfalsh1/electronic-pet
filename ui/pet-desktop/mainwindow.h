#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QProgressBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStackedWidget>
#include "teacherpanel.h"
#include "studentview.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    // 宠物状态更新接口
    void updatePetStatus(const QString &petImage, int score, int food, int exp, int maxExp);
    void updateGrowthProgress(int current, int maximum);

signals:
    // 与后端通信的信号
    void requestPetData();
    void requestExchange(int itemType, int quantity);
    void switchToTeacherView();
    void switchToStudentView();

private slots:
    void onTeacherPanelComplete();
    void onStudentViewComplete();

private:
    void setupUI();
    void setupStatusBar();
    void setupStyleSheet();
    void createConnections();

    // 中心控件
    QWidget *centralWidget;
    QStackedWidget *mainStack;
    
    // 宠物显示区域
    QLabel *petImageLabel;
    
    // 状态显示
    QLabel *scoreLabel;
    QLabel *foodLabel;
    QLabel *expLabel;
    QProgressBar *growthProgressBar;
    
    // 面板
    TeacherPanel *teacherPanel;
    StudentView *studentView;
    
    // 状态栏
    QStatusBar *statusBar;
    
    // 当前视图
    int currentViewIndex;
};

#endif // MAINWINDOW_H
