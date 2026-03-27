/**
 * @file pet.h
 * @brief 宠物类定义
 * @note 待 cpp_dev 实现
 */

#pragma once

#include <QString>
#include <QDateTime>

class Pet
{
public:
    explicit Pet(const QString &name = QString());
    ~Pet() = default;

    // 属性
    QString name() const { return m_name; }
    int level() const { return m_level; }
    int experience() const { return m_experience; }
    int happiness() const { return m_happiness; }
    int hunger() const { return m_hunger; }
    QDateTime createdAt() const { return m_createdAt; }

    // 行为
    void feed(int foodValue);
    void play(int funValue);
    void train(int expValue);
    void rest();

    // 状态检查
    bool isAlive() const;
    bool needsCare() const;

private:
    QString m_name;
    int m_level = 1;
    int m_experience = 0;
    int m_happiness = 100;
    int m_hunger = 100;
    QDateTime m_createdAt;
};
