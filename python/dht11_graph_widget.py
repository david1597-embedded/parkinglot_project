from PyQt5.QtWidgets import QWidget, QVBoxLayout
from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.figure import Figure
import pymysql
from matplotlib.animation import FuncAnimation

class DHT11GraphWidget(QWidget):
    def __init__(self, parent=None):
        super().__init__(parent)

        self.db_config = {
            "host": "10.10.10.106",
            "user": "root",
            "passwd": "1234",
            "db": "DHT11",
            "autocommit": True
        }

        self.max_points = 10
        self.x_data, self.temp_data, self.humi_data = [], [], []

        self.fig = Figure(figsize=(16, 6))
        self.canvas = FigureCanvas(self.fig)
        self.ax1 = self.fig.add_subplot(211)  # 위 그래프 (온도)
        self.ax2 = self.fig.add_subplot(212)  # 아래 그래프 (습도)

        layout = QVBoxLayout()
        layout.addWidget(self.canvas)
        self.setLayout(layout)

        self.ani = FuncAnimation(self.fig, self.animate, interval=1000)

    def animate(self, i):
        try:
            conn = pymysql.connect(**self.db_config)
            with conn.cursor() as cur:
                cur.execute("SELECT recorded_at, temp, humi FROM park1 ORDER BY recorded_at DESC LIMIT %s", (self.max_points,))
                rows = list(cur.fetchall())
                rows.reverse()

                self.x_data.clear()
                self.temp_data.clear()
                self.humi_data.clear()

                for row in rows:
                    self.x_data.append(row[0].strftime("%H:%M:%S"))
                    self.temp_data.append(row[1])
                    self.humi_data.append(row[2])

            conn.close()

            # 위 그래프 (온도) - x축 값 숨기기
            self.ax1.clear()
            self.ax1.plot(self.x_data, self.temp_data, marker='o', color='red', label='Temperature (°C)')
            for i, val in enumerate(self.temp_data):
                self.ax1.annotate(f'{val:.1f}', (self.x_data[i], self.temp_data[i]), textcoords="offset points", xytext=(0, 5), ha='center', fontsize=8)
            self.ax1.set_ylabel("Temp (°C)")
            self.ax1.legend()
            self.ax1.get_xaxis().set_visible(False)  # 위 그래프에서 x축 숨기기

            # 아래 그래프 (습도) - x축 값은 그대로 표시
            self.ax2.clear()
            self.ax2.plot(self.x_data, self.humi_data, marker='o', color='blue', label='Humidity (%)')
            for i, val in enumerate(self.humi_data):
                self.ax2.annotate(f'{val:.1f}', (self.x_data[i], self.humi_data[i]), textcoords="offset points", xytext=(0, 5), ha='center', fontsize=8)
            self.ax2.set_ylabel("Humidity (%)")
            self.ax2.set_xlabel("Time")
            self.ax2.legend()

            # x축 레이블 회전
            for label in self.ax2.get_xticklabels(): label.set_rotation(45)

            self.fig.tight_layout()
            self.canvas.draw()

        except Exception as e:
            print("❌ 그래프 업데이트 오류:", e)
