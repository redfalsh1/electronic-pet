/**
 * @file mainwindow.h
 * @brief 主窗口类定义
 * @note 待 qt_dev 实现
 */

#pragma once

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onNewPet();
    void onFeedPet();
    void onPlayWithPet();
    void onTrainPet();

private:
    Ui::MainWindow *ui;
    
    // 初始化 UI
    void setupUI();
    void setupMenuBar();
    void setupToolBar();
    void setupStatusBar();
};
