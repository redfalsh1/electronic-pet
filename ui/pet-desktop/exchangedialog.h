#ifndef EXCHANGEDIALOG_H
#define EXCHANGEDIALOG_H

#include <QDialog>
#include <QTabWidget>
#include <QListWidget>
#include <QSpinBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

class ExchangeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ExchangeDialog(QWidget *parent = nullptr);
    ~ExchangeDialog();

    // 获取选择的物品类型和数量
    int getSelectedItemType() const;
    int getQuantity() const;

signals:
    void exchangeConfirmed(int itemType, int quantity);

private slots:
    void onFoodListCurrentItemChanged();
    void onPropListCurrentItemChanged();
    void onQuantityChanged();
    void onConfirmClicked();
    void onCancelClicked();

private:
    void setupUI();
    void setupStyleSheet();
    void createConnections();
    void initFoodList();
    void initPropList();
    void updateTotalCost();

    // 选项卡
    QTabWidget *tabWidget;
    
    // 粮食购买页面
    QWidget *foodTab;
    QListWidget *foodListWidget;
    
    // 道具购买页面
    QWidget *propTab;
    QListWidget *propListWidget;
    
    // 数量选择
    QSpinBox *quantitySpinBox;
    
    // 总价显示
    QLabel *totalCostLabel;
    
    // 按钮
    QPushButton *confirmButton;
    QPushButton *cancelButton;
    
    // 当前选择
    int selectedItemType;
    int selectedItemId;
    int itemCost;
    int quantity;
};

#endif // EXCHANGEDIALOG_H
