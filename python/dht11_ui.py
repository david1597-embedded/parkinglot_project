from PyQt5 import QtCore, QtGui, QtWidgets
import subprocess


from dht11_graph_widget import DHT11GraphWidget
from photo_table_widget import ParkingStatusWidget


class Ui_MainWindow(object):

    def __init__(self):
        self.photo_proc = subprocess.Popen(["python3", "photo.py"])
        self.dht_logger = subprocess.Popen(["python3", "DHT11_DB.py"])
        self.normal_size = QtCore.QSize(640, 400)
        self.expanded_size = QtCore.QSize(900, 600)

    def show_parking(self):
        self.stackedWidget.setCurrentIndex(1)

    def show_ht_graph(self):
        self.stackedWidget.setCurrentIndex(2)
        MainWindow.resize(self.expanded_size)

    def go_home_parking(self):
        self.stackedWidget.setCurrentIndex(0)

    def go_home_dht11(self):
        self.stackedWidget.setCurrentIndex(0)
        MainWindow.resize(self.normal_size)

    def stop_all_processes(self):
        if self.photo_proc and self.photo_proc.poll() is None:
            self.photo_proc.terminate()
            self.photo_proc.wait()
        if self.dht_logger and self.dht_logger.poll() is None:
            self.dht_logger.terminate()
            self.dht_logger.wait()

    def quit_program(self):
        self.stop_all_processes()
        QtWidgets.QApplication.quit()

    def setupUi(self, MainWindow):
        MainWindow.setObjectName("MainWindow")
        MainWindow.resize(640, 400)
        self.centralwidget = QtWidgets.QWidget(MainWindow)
        self.centralwidget.setObjectName("centralwidget")
        self.stackedWidget = QtWidgets.QStackedWidget(self.centralwidget)
        self.stackedWidget.setGeometry(QtCore.QRect(0, 0, 900, 600))
        self.stackedWidget.setObjectName("stackedWidget")

        # 첫 페이지 (홈 화면)
        self.page = QtWidgets.QWidget()
        self.page.setObjectName("page")
        self.label = QtWidgets.QLabel(self.page)
        self.label.setGeometry(QtCore.QRect(50, 90, 241, 231))
        self.label.setText("")
        self.label.setPixmap(QtGui.QPixmap("parking.jpg"))
        self.label.setScaledContents(True)
        self.label.setObjectName("label")
        self.HTmaintxt = QtWidgets.QTextEdit(self.page)
        self.HTmaintxt.setGeometry(QtCore.QRect(50, 20, 551, 41))
        self.HTmaintxt.setObjectName("HTmaintxt")
        self.shutdown = QtWidgets.QPushButton(self.page)
        self.shutdown.setGeometry(QtCore.QRect(360, 280, 241, 41))
        self.shutdown.setObjectName("shutdown")
        self.pushButton_2 = QtWidgets.QPushButton(self.page)
        self.pushButton_2.setGeometry(QtCore.QRect(360, 140, 241, 41))
        self.pushButton_2.setObjectName("pushButton_2")
        self.pushButton = QtWidgets.QPushButton(self.page)
        self.pushButton.setGeometry(QtCore.QRect(360, 90, 241, 41))
        self.pushButton.setObjectName("pushButton")
        self.stackedWidget.addWidget(self.page)

        # 두 번째 페이지 (주차장 테이블)
        self.page_1 = QtWidgets.QWidget()
        self.page_1.setObjectName("page_1")
        self.pushButton_4 = QtWidgets.QPushButton(self.page_1)
        self.pushButton_4.setGeometry(QtCore.QRect(300, 320, 75, 24))
        self.pushButton_4.setObjectName("pushButton_4")

        self.parking_status_widget = ParkingStatusWidget(self.page_1)
        self.parking_status_widget.setGeometry(QtCore.QRect(50, 20, 550, 250))
        self.stackedWidget.addWidget(self.page_1)

        # 세 번째 페이지 (온습도 그래프)
        self.page_2 = QtWidgets.QWidget()
        self.page_2.setObjectName("page_2")

        self.graph_widget = DHT11GraphWidget(self.page_2)
        self.graph_widget.setGeometry(QtCore.QRect(10, 10, 880, 500))
        self.graph_widget.setObjectName("graph_widget")

        self.pushButton_3 = QtWidgets.QPushButton(self.page_2)
        self.pushButton_3.setGeometry(QtCore.QRect(400, 530, 100, 30))
        self.pushButton_3.setObjectName("pushButton_3")
        self.stackedWidget.addWidget(self.page_2)

        MainWindow.setCentralWidget(self.centralwidget)
        self.menubar = QtWidgets.QMenuBar(MainWindow)
        self.menubar.setGeometry(QtCore.QRect(0, 0, 674, 22))
        self.menubar.setObjectName("menubar")
        MainWindow.setMenuBar(self.menubar)
        self.statusbar = QtWidgets.QStatusBar(MainWindow)
        self.statusbar.setObjectName("statusbar")
        MainWindow.setStatusBar(self.statusbar)

        self.retranslateUi(MainWindow)
        self.stackedWidget.setCurrentIndex(0)
        QtCore.QMetaObject.connectSlotsByName(MainWindow)

        # 버튼 연결
        self.pushButton.clicked.connect(self.show_parking)
        self.pushButton_2.clicked.connect(self.show_ht_graph)
        self.pushButton_3.clicked.connect(self.go_home_dht11)
        self.pushButton_4.clicked.connect(self.go_home_parking)
        self.shutdown.clicked.connect(self.quit_program)

    def retranslateUi(self, MainWindow):
        _translate = QtCore.QCoreApplication.translate
        MainWindow.setWindowTitle(_translate("MainWindow", "MainWindow"))
        self.HTmaintxt.setHtml(_translate("MainWindow",
                                          "<html><head/><body><p align=\"center\"><span style=\" font-size:16pt; font-weight:700;\">주차장 모니터링 시스템</span></p></body></html>"))
        self.shutdown.setText(_translate("MainWindow", "프로그램 종료"))
        self.pushButton_2.setText(_translate("MainWindow", "온/습도"))
        self.pushButton.setText(_translate("MainWindow", "주차장 현황"))
        self.pushButton_4.setText(_translate("MainWindow", "홈으로"))
        self.pushButton_3.setText(_translate("MainWindow", "홈으로"))


if __name__ == "__main__":
    import sys

    app = QtWidgets.QApplication(sys.argv)
    MainWindow = QtWidgets.QMainWindow()
    ui = Ui_MainWindow()
    ui.setupUi(MainWindow)
    MainWindow.show()
    sys.exit(app.exec_())
