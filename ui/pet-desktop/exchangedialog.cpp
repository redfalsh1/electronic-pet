#include "exchangedialog.h"
#include <QMessageBox>

ExchangeDialog::ExchangeDialog(QWidget *parent)
    : QDialog(parent)
    , selectedItemType(0)
    , selectedItemId(0)
    , itemCost(0)
    , quantity(1)
{
    setupUI();
    setupStyleSheet();
    createConnections();
    initFoodList();
    initPropList();
}

ExchangeDialog::~ExchangeDialog()
{
}

void ExchangeDialog::setupUI()
{
    setWindowTitle(tr("积分兑换"));
    setMinimumSize(600, 500);
    setModal(true);
    
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(15);
    
    // 选项卡
    tabWidget = new QTabWidget();
    
    // 粮食购买页面
    foodTab = new QWidget();
    QVBoxLayout *foodLayout = new QVBoxLayout(foodTab);
    
    foodListWidget = new QListWidget();
    foodListWidget->setViewMode(QListWidget::IconMode);
    foodListWidget->setIconSize(QSize(64, 64));
    foodListWidget->setGridSize(QSize(120, 100));
    foodListWidget->setSpacing(10);
    foodLayout->addWidget(foodListWidget);
    
    tabWidget->addTab(foodTab, tr("粮食购买"));
    
    // 道具购买页面
    propTab = new QWidget();
    QVBoxLayout *propLayout = new QVBoxLayout(propTab);
    
    propListWidget = new QListWidget();
    propListWidget->setViewMode(QListWidget::IconMode);
    propListWidget->setIconSize(QSize(64, 64));
    propListWidget->setGridSize(QSize(120, 100));
    propListWidget->setSpacing(10);
    propLayout->addWidget(propListWidget);
    
    tabWidget->addTab(propTab, tr("道具购买"));
    
    mainLayout->addWidget(tabWidget);
    
    // 数量选择区域
    QGroupBox *quantityGroupBox = new QGroupBox(tr("购买数量"));
    QHBoxLayout *quantityLayout = new QHBoxLayout(quantityGroupBox);
    
    quantityLayout->addWidget(new QLabel(tr("数量:")));
    quantitySpinBox = new QSpinBox();
    quantitySpinBox->setRange(1, 99);
    quantitySpinBox->setValue(1);
    quantitySpinBox->setFixedWidth(80);
    quantityLayout->addWidget(quantitySpinBox);
    
    quantityLayout->addStretch();
    
    // 总价显示
    totalCostLabel = new QLabel(tr("总价：0 积分"));
    totalCostLabel->setStyleSheet("QLabel { font-size: 18px; font-weight: bold; color: #ffd700; }");
    quantityLayout->addWidget(totalCostLabel);
    
    mainLayout->addWidget(quantityGroupBox);
    
    // 按钮区域
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    
    confirmButton = new QPushButton(tr("确认兑换"));
    confirmButton->setFixedWidth(120);
    confirmButton->setFixedHeight(40);
    buttonLayout->addWidget(confirmButton);
    
    cancelButton = new QPushButton(tr("取消"));
    cancelButton->setFixedWidth(100);
    cancelButton->setFixedHeight(40);
    buttonLayout->addWidget(cancelButton);
    
    mainLayout->addLayout(buttonLayout);
    
    // 初始禁用确认按钮
    confirmButton->setEnabled(false);
}

void ExchangeDialog::setupStyleSheet()
{
    // 继承主窗口样式
}

void ExchangeDialog::createConnections()
{
    connect(foodListWidget, &QListWidget::currentItemChanged, this, &ExchangeDialog::onFoodListCurrentItemChanged);
    connect(propListWidget, &QListWidget::currentItemChanged, this, &ExchangeDialog::onPropListCurrentItemChanged);
    connect(quantitySpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &ExchangeDialog::onQuantityChanged);
    connect(confirmButton, &QPushButton::clicked, this, &ExchangeDialog::onConfirmClicked);
    connect(cancelButton, &QPushButton::clicked, this, &ExchangeDialog::onCancelClicked);
}

void ExchangeDialog::initFoodList()
{
    // 添加示例粮食物品
    QStringList foodItems = {
        tr("普通饲料|10|恢复 10 点饱食度"),
        tr("高级饲料|20|恢复 25 点饱食度"),
        tr("营养大餐|50|恢复 60 点饱食度"),
        tr("豪华盛宴|100|恢复 100 点饱食度"),
        tr("神秘果实|200|特殊效果")
    };
    
    for (const QString &item : foodItems) {
        QStringList parts = item.split("|");
        if (parts.size() >= 3) {
            QListWidgetItem *listItem = new QListWidgetItem();
            listItem->setText(parts[0]);
            listItem->setData(Qt::UserRole, parts[1].toInt()); // 价格
            listItem->setData(Qt::UserRole + 1, parts[2]); // 描述
            listItem->setFlags(listItem->flags() | Qt::ItemIsSelectable);
            
            // 占位图标（实际使用时替换为真实图片）
            listItem->setIcon(QIcon(":images/food_placeholder.png"));
            
            foodListWidget->addItem(listItem);
        }
    }
}

void ExchangeDialog::initPropList()
{
    // 添加示例道具物品
    QStringList propItems = {
        tr("装饰帽子|30|可爱的装饰帽子"),
        tr("时尚眼镜|50|提升魅力的眼镜"),
        tr("彩色项圈|40|漂亮的项圈"),
        tr("经验药水|80|增加 50 点经验"),
        tr("成长激素|150|立即提升 1 级"),
        tr("稀有宠物蛋|500|孵化稀有宠物")
    };
    
    for (const QString &item : propItems) {
        QStringList parts = item.split("|");
        if (parts.size() >= 3) {
            QListWidgetItem *listItem = new QListWidgetItem();
            listItem->setText(parts[0]);
            listItem->setData(Qt::UserRole, parts[1].toInt()); // 价格
            listItem->setData(Qt::UserRole + 1, parts[2]); // 描述
            listItem->setFlags(listItem->flags() | Qt::ItemIsSelectable);
            
            // 占位图标
            listItem->setIcon(QIcon(":images/prop_placeholder.png"));
            
            propListWidget->addItem(listItem);
        }
    }
}

void ExchangeDialog::onFoodListCurrentItemChanged()
{
    QListWidgetItem *currentItem = foodListWidget->currentItem();
    if (currentItem) {
        selectedItemType = 1; // 粮食类型
        selectedItemId = currentItem->data(Qt::UserRole).toInt();
        itemCost = currentItem->data(Qt::UserRole).toInt();
        updateTotalCost();
        confirmButton->setEnabled(true);
    }
}

void ExchangeDialog::onPropListCurrentItemChanged()
{
    QListWidgetItem *currentItem = propListWidget->currentItem();
    if (currentItem) {
        selectedItemType = 2; // 道具类型
        selectedItemId = currentItem->data(Qt::UserRole).toInt();
        itemCost = currentItem->data(Qt::UserRole).toInt();
        updateTotalCost();
        confirmButton->setEnabled(true);
    }
}

void ExchangeDialog::onQuantityChanged()
{
    quantity = quantitySpinBox->value();
    updateTotalCost();
}

void ExchangeDialog::updateTotalCost()
{
    int total = itemCost * quantity;
    totalCostLabel->setText(tr("总价：%1 积分").arg(total));
}

void ExchangeDialog::onConfirmClicked()
{
    if (selectedItemType == 0) {
        QMessageBox::warning(this, tr("警告"), tr("请选择要兑换的物品"));
        return;
    }
    
    int total = itemCost * quantity;
    QString itemName;
    
    if (selectedItemType == 1) {
        QListWidgetItem *item = foodListWidget->currentItem();
        if (item) itemName = item->text();
    } else {
        QListWidgetItem *item = propListWidget->currentItem();
        if (item) itemName = item->text();
    }
    
    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        tr("确认兑换"),
        tr("确定要兑换 %1 x %2 吗？\n共需 %3 积分").arg(itemName).arg(quantity).arg(total),
        QMessageBox::Yes | QMessageBox::No
    );
    
    if (reply == QMessageBox::Yes) {
        emit exchangeConfirmed(selectedItemType, quantity);
        accept();
    }
}

void ExchangeDialog::onCancelClicked()
{
    reject();
}

int ExchangeDialog::getSelectedItemType() const
{
    return selectedItemType;
}

int ExchangeDialog::getQuantity() const
{
    return quantity;
}
