#ifndef STUDENTVIEW_H
#define STUDENTVIEW_H

#include <QWidget>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QTableWidget>
#include "exchangedialog.h"

class StudentView : public QWidget
{
    Q_OBJECT

public:
    explicit StudentView(QWidget *parent = nullptr);
    ~StudentView();

    // 更新宠物状态
    void updatePetInfo(const QString &petName, const QString &petImage, int level);
    void updateStatus(int score, int food, int exp, int maxExp);
    void updateGrowthRecord(const QList<QString> &records);

signals:
    void requestExchange(int itemType, int quantity);
    void operationComplete();

private slots:
    void onExchangeButtonClicked();
    void onFeedPetClicked();
    void onDialogAccepted();

private:
    void setupUI();
    void setupStyleSheet();
    void createConnections();

    // 宠物信息区域
    QLabel *petImageLabel;
    QLabel *petNameLabel;
    QLabel *petLevelLabel;
    
    // 状态显示
    QLabel *scoreLabel;
    QLabel *foodLabel;
    QLabel *expLabel;
    QProgressBar *expProgressBar;
    
    // 操作按钮
    QPushButton *exchangeButton;
    QPushButton *feedPetButton;
    
    // 成长记录表格
    QTableWidget *growthRecordTable;
    
    // 兑换对话框
    ExchangeDialog *exchangeDialog;
};

#endif // STUDENTVIEW_H
