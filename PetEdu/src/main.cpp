/**
 * @file main.cpp
 * @brief PetEdu 电子宠物应用程序入口
 * @version 1.0.0
 * @date 2026-03-26
 */

#include <QApplication>
#include <QMainWindow>
#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>

/**
 * @brief 主窗口类 (临时实现)
 * @note 将由 qt_dev 完善
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr) : QMainWindow(parent)
    {
        auto *centralWidget = new QWidget(this);
        auto *layout = new QVBoxLayout(centralWidget);
        
        auto *label = new QLabel("PetEdu - 电子宠物", this);
        label->setAlignment(Qt::AlignCenter);
        label->setStyleSheet("font-size: 24px; font-weight: bold;");
        
        layout->addWidget(label);
        setCentralWidget(centralWidget);
        
        resize(800, 600);
        setWindowTitle("PetEdu v1.0.0");
    }
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // 设置应用程序信息
    QApplication::setApplicationName("PetEdu");
    QApplication::setApplicationVersion("1.0.0");
    QApplication::setOrganizationName("EDA Team");
    
    MainWindow window;
    window.show();
    
    return app.exec();
}

#include "main.moc"
