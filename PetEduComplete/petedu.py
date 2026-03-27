#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
PetEdu - 电子宠物教学系统 (Python 版本)
可直接运行，无需编译
"""

import sys
import json
from datetime import datetime
from pathlib import Path

# 检查 PyQt5
try:
    from PyQt5.QtWidgets import (QApplication, QMainWindow, QWidget, QVBoxLayout, 
                                  QHBoxLayout, QLabel, QPushButton, QProgressBar,
                                  QStackedWidget, QMessageBox, QComboBox, QSpinBox,
                                  QGroupBox, QDialog, QScrollArea)
    from PyQt5.QtCore import Qt, QTimer
    from PyQt5.QtGui import QFont, QPixmap, QPainter, QColor, QIcon
    HAS_PYQT = True
except ImportError:
    HAS_PYQT = False
    print("未找到 PyQt5，正在尝试安装...")
    import subprocess
    subprocess.check_call([sys.executable, "-m", "pip", "install", "PyQt5", "-q"])
    from PyQt5.QtWidgets import (QApplication, QMainWindow, QWidget, QVBoxLayout, 
                                  QHBoxLayout, QLabel, QPushButton, QProgressBar,
                                  QStackedWidget, QMessageBox, QComboBox, QSpinBox,
                                  QGroupBox, QDialog, QScrollArea)
    from PyQt5.QtCore import Qt, QTimer
    from PyQt5.QtGui import QFont, QPixmap, QPainter, QColor, QIcon
    HAS_PYQT = True


class PetEduApp(QMainWindow):
    """主应用程序"""
    
    def __init__(self):
        super().__init__()
        self.init_data()
        self.init_ui()
        self.load_data()
        
        # 自动保存定时器
        self.timer = QTimer()
        self.timer.timeout.connect(self.save_data)
        self.timer.start(30000)  # 30 秒自动保存
        
    def init_data(self):
        """初始化数据"""
        self.pet_level = 1
        self.pet_exp = 0
        self.pet_points = 0
        self.pet_food = 0
        self.students = ["张三", "李四", "王五", "赵六"]
        self.student_scores = {name: 0 for name in self.students}
        self.data_file = Path.home() / "petedu_data.json"
        
    def init_ui(self):
        """初始化界面"""
        self.setWindowTitle("PetEdu - 电子宠物教学系统 🐾")
        self.setMinimumSize(1024, 768)
        self.setStyleSheet(self.get_dark_style())
        
        # 中央控件
        central = QWidget()
        self.setCentralWidget(central)
        main_layout = QVBoxLayout(central)
        
        # 顶部状态栏
        status_widget = QWidget()
        status_layout = QHBoxLayout(status_widget)
        
        # 宠物图片
        self.pet_label = QLabel("🥚")
        self.pet_label.setFont(QFont("Microsoft YaHei", 72))
        self.pet_label.setAlignment(Qt.AlignCenter)
        self.pet_label.setFixedSize(150, 150)
        self.pet_label.setStyleSheet("""
            QLabel {
                background-color: #4a9eff;
                border-radius: 75px;
            }
        """)
        
        # 状态信息
        info_widget = QWidget()
        info_layout = QVBoxLayout(info_widget)
        
        self.points_label = QLabel("积分：0")
        self.points_label.setStyleSheet("color: #ffd700; font-size: 24px; font-weight: bold;")
        
        self.food_label = QLabel("粮食：0")
        self.food_label.setStyleSheet("color: #90EE90; font-size: 24px; font-weight: bold;")
        
        self.exp_label = QLabel("经验：0/100")
        self.exp_label.setStyleSheet("color: #87CEEB; font-size: 24px; font-weight: bold;")
        
        info_layout.addWidget(self.points_label)
        info_layout.addWidget(self.food_label)
        info_layout.addWidget(self.exp_label)
        
        status_layout.addWidget(self.pet_label)
        status_layout.addWidget(info_widget)
        status_layout.addStretch()
        
        main_layout.addWidget(status_widget)
        
        # 成长进度条
        self.progress = QProgressBar()
        self.progress.setRange(0, 100)
        self.progress.setValue(0)
        self.progress.setFixedHeight(30)
        self.progress.setStyleSheet("""
            QProgressBar {
                background-color: #2d2d2d;
                border-radius: 15px;
                text-align: center;
                color: white;
                font-weight: bold;
            }
            QProgressBar::chunk {
                background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                    stop:0 #4a9eff, stop:1 #90EE90);
                border-radius: 15px;
            }
        """)
        main_layout.addWidget(self.progress)
        
        # 视图切换
        self.stacked = QStackedWidget()
        
        self.student_view = self.create_student_view()
        self.teacher_view = self.create_teacher_view()
        
        self.stacked.addWidget(self.student_view)
        self.stacked.addWidget(self.teacher_view)
        
        main_layout.addWidget(self.stacked)
        
        # 切换按钮
        btn_widget = QWidget()
        btn_layout = QHBoxLayout(btn_widget)
        
        self.student_btn = QPushButton("🎮 学生视图")
        self.teacher_btn = QPushButton("👨‍🏫 教师面板")
        
        self.student_btn.setFixedHeight(45)
        self.teacher_btn.setFixedHeight(45)
        
        self.student_btn.clicked.connect(lambda: self.stacked.setCurrentIndex(0))
        self.teacher_btn.clicked.connect(lambda: self.stacked.setCurrentIndex(1))
        
        btn_layout.addStretch()
        btn_layout.addWidget(self.student_btn)
        btn_layout.addWidget(self.teacher_btn)
        
        main_layout.addWidget(btn_widget)
        
        self.update_display()
        
    def create_student_view(self):
        """创建学生视图"""
        widget = QWidget()
        layout = QVBoxLayout(widget)
        
        title = QLabel("🎮 学生中心")
        title.setStyleSheet("font-size: 28px; font-weight: bold; color: #4a9eff;")
        title.setAlignment(Qt.AlignCenter)
        layout.addWidget(title)
        
        # 宠物信息卡片
        card = QWidget()
        card.setStyleSheet("background-color: #2d2d2d; border-radius: 15px; padding: 20px;")
        card_layout = QVBoxLayout(card)
        
        pet_info = QLabel(f"我的宠物 - 等级 {self.pet_level}")
        pet_info.setStyleSheet("color: white; font-size: 20px;")
        pet_info.setAlignment(Qt.AlignCenter)
        card_layout.addWidget(pet_info)
        
        layout.addWidget(card)
        layout.addSpacing(20)
        
        # 功能按钮
        feed_btn = QPushButton("🍖 喂食宠物")
        feed_btn.setFixedHeight(60)
        feed_btn.setStyleSheet("""
            QPushButton {
                background-color: #4a9eff;
                color: white;
                font-size: 20px;
                font-weight: bold;
                border-radius: 10px;
            }
            QPushButton:hover {
                background-color: #5aafff;
            }
        """)
        feed_btn.clicked.connect(self.feed_pet)
        layout.addWidget(feed_btn)
        
        exchange_btn = QPushButton("💰 积分兑换")
        exchange_btn.setFixedHeight(60)
        exchange_btn.setStyleSheet("""
            QPushButton {
                background-color: #90EE90;
                color: #1e1e1e;
                font-size: 20px;
                font-weight: bold;
                border-radius: 10px;
            }
            QPushButton:hover {
                background-color: #a0ff90;
            }
        """)
        exchange_btn.clicked.connect(self.show_exchange_dialog)
        layout.addWidget(exchange_btn)
        
        layout.addStretch()
        return widget
        
    def create_teacher_view(self):
        """创建教师视图"""
        widget = QWidget()
        layout = QVBoxLayout(widget)
        
        title = QLabel("👨‍🏫 教师管理面板")
        title.setStyleSheet("font-size: 28px; font-weight: bold; color: #ffd700;")
        title.setAlignment(Qt.AlignCenter)
        layout.addWidget(title)
        
        # 学生选择
        select_widget = QWidget()
        select_layout = QHBoxLayout(select_widget)
        select_layout.addWidget(QLabel("选择学生:"))
        self.student_combo = QComboBox()
        self.student_combo.addItems(self.students)
        self.student_combo.setFixedHeight(35)
        select_layout.addWidget(self.student_combo)
        select_layout.addStretch()
        layout.addWidget(select_widget)
        
        # 发放积分
        points_group = QGroupBox("📊 发放积分")
        points_layout = QVBoxLayout(points_group)
        
        reason_layout = QHBoxLayout()
        reason_layout.addWidget(QLabel("原因:"))
        self.reason_combo = QComboBox()
        self.reason_combo.addItems(["课堂答题", "完成作业", "课堂表现", "其他"])
        reason_layout.addWidget(self.reason_combo)
        points_layout.addLayout(reason_layout)
        
        amount_layout = QHBoxLayout()
        amount_layout.addWidget(QLabel("积分:"))
        self.points_spin = QSpinBox()
        self.points_spin.setRange(1, 100)
        self.points_spin.setValue(5)
        self.points_spin.setFixedHeight(35)
        amount_layout.addWidget(self.points_spin)
        amount_layout.addStretch()
        points_layout.addLayout(amount_layout)
        
        award_btn = QPushButton("发放积分")
        award_btn.setFixedHeight(45)
        award_btn.setStyleSheet("""
            QPushButton {
                background-color: #ffd700;
                color: #1e1e1e;
                font-size: 16px;
                font-weight: bold;
                border-radius: 8px;
            }
        """)
        award_btn.clicked.connect(self.award_points)
        points_layout.addWidget(award_btn)
        
        layout.addWidget(points_group)
        
        # 发放奖励
        reward_group = QGroupBox("🎁 发放奖励")
        reward_layout = QHBoxLayout(reward_group)
        
        food_reward_btn = QPushButton("🍖 粮食")
        food_reward_btn.setFixedHeight(45)
        food_reward_btn.clicked.connect(lambda: self.award_reward("粮食"))
        
        prop_reward_btn = QPushButton("✨ 道具")
        prop_reward_btn.setFixedHeight(45)
        prop_reward_btn.clicked.connect(lambda: self.award_reward("道具"))
        
        reward_layout.addWidget(food_reward_btn)
        reward_layout.addWidget(prop_reward_btn)
        
        layout.addWidget(reward_group)
        layout.addStretch()
        return widget
        
    def get_dark_style(self):
        """获取深色主题样式"""
        return """
            QMainWindow, QWidget {
                background-color: #1e1e1e;
                color: #ffffff;
                font-family: "Microsoft YaHei";
            }
            QLabel {
                color: #ffffff;
            }
            QPushButton {
                border: none;
                padding: 10px;
            }
            QComboBox, QSpinBox {
                background-color: #2d2d2d;
                border: 1px solid #4a9eff;
                border-radius: 5px;
                padding: 5px;
                color: white;
            }
            QGroupBox {
                font-weight: bold;
                font-size: 16px;
                border: 2px solid #4a9eff;
                border-radius: 10px;
                margin-top: 15px;
                padding-top: 15px;
            }
            QGroupBox::title {
                subcontrol-origin: margin;
                left: 15px;
                padding: 0 10px;
                color: #4a9eff;
            }
        """
        
    def update_display(self):
        """更新显示"""
        self.points_label.setText(f"积分：{self.pet_points}")
        self.food_label.setText(f"粮食：{self.pet_food}")
        exp_needed = self.pet_level * 100
        self.exp_label.setText(f"经验：{self.pet_exp}/{exp_needed}")
        self.progress.setValue(self.pet_exp % 100)
        
        # 根据等级更新宠物外观
        if self.pet_level < 5:
            emoji = "🥚"
        elif self.pet_level < 15:
            emoji = "🐣"
        elif self.pet_level < 30:
            emoji = "🐤"
        elif self.pet_level < 50:
            emoji = "🐔"
        elif self.pet_level < 60:
            emoji = "🦚"
        else:
            emoji = "🦅"
            
        self.pet_label.setText(emoji)
        
    def feed_pet(self):
        """喂食宠物"""
        if self.pet_food > 0:
            self.pet_food -= 1
            self.pet_exp += 5
            self.check_level_up()
            self.update_display()
            self.save_data()
            QMessageBox.information(self, "喂食", "宠物吃了粮食，经验 +5！✨")
        else:
            QMessageBox.warning(self, "提示", "粮食不足！请先兑换粮食。🍖")
            
    def check_level_up(self):
        """检查升级"""
        exp_needed = self.pet_level * 100
        while self.pet_exp >= exp_needed:
            self.pet_exp -= exp_needed
            self.pet_level += 1
            self.pet_level = min(self.pet_level, 60)
            QMessageBox.information(self, "升级！", f"恭喜！宠物升到{self.pet_level}级！🎉")
            exp_needed = self.pet_level * 100
            
    def show_exchange_dialog(self):
        """显示兑换对话框"""
        dialog = QDialog(self)
        dialog.setWindowTitle("积分兑换")
        dialog.setMinimumWidth(400)
        layout = QVBoxLayout(dialog)
        
        title = QLabel("请选择兑换物品")
        title.setStyleSheet("font-size: 20px; font-weight: bold;")
        layout.addWidget(title)
        
        food_btn = QPushButton("🍖 10 积分 = 1 粮食")
        food_btn.setFixedHeight(45)
        food_btn.clicked.connect(lambda: self.exchange_points_to_food(10, dialog))
        layout.addWidget(food_btn)
        
        bulk_btn = QPushButton("🎁 50 积分 = 6 粮食 (优惠)")
        bulk_btn.setFixedHeight(45)
        bulk_btn.clicked.connect(lambda: self.exchange_points_to_food(50, dialog))
        layout.addWidget(bulk_btn)
        
        cancel_btn = QPushButton("取消")
        cancel_btn.setFixedHeight(40)
        cancel_btn.clicked.connect(dialog.reject)
        layout.addWidget(cancel_btn)
        
        dialog.exec()
        
    def exchange_points_to_food(self, points, dialog):
        """积分兑换粮食"""
        if self.pet_points >= points:
            self.pet_points -= points
            food_amount = points // 10
            if points >= 50:
                food_amount += 1  # 批量优惠
            self.pet_food += food_amount
            self.update_display()
            self.save_data()
            dialog.accept()
            QMessageBox.information(self, "兑换成功", f"使用{points}积分兑换了{food_amount}粮食！✨")
        else:
            QMessageBox.warning(self, "积分不足", "积分不够兑换！💰")
            
    def award_points(self):
        """发放积分"""
        student = self.student_combo.currentText()
        reason = self.reason_combo.currentText()
        points = self.points_spin.value()
        
        self.pet_points += points
        self.student_scores[student] = self.student_scores.get(student, 0) + points
        self.update_display()
        self.save_data()
        
        QMessageBox.information(self, "发放成功", 
            f"给{student}发放了{points}积分（原因：{reason}）\n当前积分：{self.pet_points}")
            
    def award_reward(self, reward_type):
        """发放奖励"""
        student = self.student_combo.currentText()
        self.pet_food += 1
        self.update_display()
        self.save_data()
        QMessageBox.information(self, "发放奖励", 
            f"给{student}发放了{reward_type}！\n当前粮食：{self.pet_food}")
            
    def save_data(self):
        """保存数据"""
        data = {
            "pet_level": self.pet_level,
            "pet_exp": self.pet_exp,
            "pet_points": self.pet_points,
            "pet_food": self.pet_food,
            "student_scores": self.student_scores,
            "last_save": datetime.now().isoformat()
        }
        try:
            with open(self.data_file, "w", encoding="utf-8") as f:
                json.dump(data, f, ensure_ascii=False, indent=2)
        except Exception as e:
            print(f"保存失败：{e}")
            
    def load_data(self):
        """加载数据"""
        try:
            if self.data_file.exists():
                with open(self.data_file, "r", encoding="utf-8") as f:
                    data = json.load(f)
                self.pet_level = data.get("pet_level", 1)
                self.pet_exp = data.get("pet_exp", 0)
                self.pet_points = data.get("pet_points", 0)
                self.pet_food = data.get("pet_food", 0)
                self.student_scores = data.get("student_scores", {})
                self.update_display()
        except Exception as e:
            print(f"加载失败：{e}")


def main():
    """主函数"""
    print("=" * 50)
    print("PetEdu - 电子宠物教学系统")
    print("=" * 50)
    print("正在启动...")
    
    app = QApplication(sys.argv)
    app.setStyle("Fusion")
    
    window = PetEduApp()
    window.show()
    
    print("[OK] 程序已启动！")
    print("数据保存位置：" + str(Path.home() / "petedu_data.json"))
    
    sys.exit(app.exec_())


if __name__ == "__main__":
    main()
