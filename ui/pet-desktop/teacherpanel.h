#ifndef TEACHERPANEL_H
#define TEACHERPANEL_H

#include <QWidget>
#include <QListWidget>
#include <QComboBox>
#include <QSpinBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QTableWidget>

class TeacherPanel : public QWidget
{
    Q_OBJECT

public:
    explicit TeacherPanel(QWidget *parent = nullptr);
    ~TeacherPanel();

    // 学生列表管理
    void addStudent(const QString &studentId, const QString &studentName);
    void removeStudent(const QString &studentId);
    void updateStudentScore(const QString &studentId, int score);

signals:
    void awardPoints(const QString &studentId, int points, const QString &reason);
    void awardReward(const QString &studentId, const QString &rewardType);
    void operationComplete();

private slots:
    void onAwardPointsClicked();
    void onAwardRewardClicked();
    void onStudentSelected();
    void refreshStudentList();

private:
    void setupUI();
    void setupStyleSheet();
    void createConnections();

    // 学生列表
    QListWidget *studentListWidget;
    
    // 积分发放区域
    QGroupBox *pointsGroupBox;
    QComboBox *reasonComboBox;
    QSpinBox *pointsSpinBox;
    QPushButton *awardPointsButton;
    
    // 奖励发放区域
    QGroupBox *rewardGroupBox;
    QComboBox *rewardTypeComboBox;
    QPushButton *awardRewardButton;
    
    // 学生积分表格
    QTableWidget *scoreTableWidget;
    
    // 刷新按钮
    QPushButton *refreshButton;
    
    // 当前选中的学生
    QString currentStudentId;
    QString currentStudentName;
};

#endif // TEACHERPANEL_H
