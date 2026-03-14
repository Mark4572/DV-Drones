import sys
import os
import subprocess
from PyQt6.QtWidgets import QApplication, QMainWindow
from PyQt6.QtWebEngineWidgets import QWebEngineView
from PyQt6.QtCore import QUrl, QTimer
from PyQt6.QtGui import QIcon

class Browser(QMainWindow):
    def __init__(self):
        super().__init__()

        # 1. Window Configuration
        self.setWindowTitle("DV_Client_0.2beta")
        icon_path = os.path.join(os.path.dirname(__file__), 'code/frontend/drohne.ico')
        if os.path.exists(icon_path):
            self.setWindowIcon(QIcon(icon_path))
        
        self.setGeometry(200, 200, 1000, 700)

        # 2. Setup WebEngine
        self.browser = QWebEngineView()
        self.setCentralWidget(self.browser)

        # 3. Start host.py with Force-Hide Flags
        host_path = os.path.join(os.path.dirname(__file__), 'host.py')
        
        if os.name == 'nt':  # Windows
            # This is the most "aggressive" way to hide the console
            startupinfo = subprocess.STARTUPINFO()
            startupinfo.dwFlags |= subprocess.STARTF_USESHOWWINDOW
            startupinfo.wShowWindow = 0 # SW_HIDE
            
            self.host_process = subprocess.Popen(
                [sys.executable, host_path],
                startupinfo=startupinfo,
                creationflags=subprocess.CREATE_NO_WINDOW,
                stdout=subprocess.PIPE, # Capturing output can sometimes prevent the window
                stderr=subprocess.PIPE,
                stdin=subprocess.PIPE
            )
        else: # Mac/Linux
            self.host_process = subprocess.Popen([sys.executable, host_path])

        # 4. Wait 3 seconds for the server to boot before loading the URL
        QTimer.singleShot(1000, self.load_server)

    def load_server(self):
        self.browser.setUrl(QUrl("http://127.0.0.1:47000/"))

    def closeEvent(self, event):
        if hasattr(self, 'host_process'):
            self.host_process.terminate()
            try:
                self.host_process.wait(timeout=1)
            except subprocess.TimeoutExpired:
                self.host_process.kill()
        event.accept()

if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = Browser()
    window.show()
    sys.exit(app.exec())