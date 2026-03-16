import sys
import os
import subprocess
import ctypes
import socket
from PyQt6.QtWidgets import QApplication, QSplashScreen
from PyQt6.QtWebEngineWidgets import QWebEngineView
from PyQt6.QtCore import QUrl, Qt, QTimer
from PyQt6.QtGui import QPixmap, QIcon

# Konfiguration
APP_ID = 'mark4572.dvclient.v0.2'
BASE_DIR = os.path.dirname(os.path.abspath(__file__))
HOST = "127.0.0.1"
PORT = 47000
URL = f"http://{HOST}:{PORT}/"

if sys.platform == 'win32':
    ctypes.windll.shell32.SetCurrentProcessExplicitAppUserModelID(APP_ID)

class Launcher:
    def __init__(self):
        self.app = QApplication(sys.argv)
        self.setup_style()
        
       
        splash_pix = QPixmap(os.path.join(BASE_DIR, 'placeholder.ico')).scaled(400, 400, Qt.AspectRatioMode.KeepAspectRatio)
        self.splash = QSplashScreen(splash_pix)
        self.splash.show()
        
        self.view = QWebEngineView()
        self.view.setWindowTitle("DV Client")
        self.view.setContextMenuPolicy(Qt.ContextMenuPolicy.NoContextMenu)
        
        self.backend = None

    def setup_style(self):
        icon_path = os.path.join(BASE_DIR, "vscode32.png")
        if os.path.exists(icon_path):
            self.app.setWindowIcon(QIcon(icon_path))

    def is_server_ready(self):
        """Prüft per Socket, ob der Port bereits offen ist."""
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            return s.connect_ex((HOST, PORT)) == 0

    def start_backend(self):
        self.splash.showMessage("Init Client..", Qt.AlignmentFlag.AlignBottom, Qt.GlobalColor.white)
        flags = subprocess.CREATE_NO_WINDOW if sys.platform == 'win32' else 0
        self.backend = subprocess.Popen(
            [sys.executable, "host.py"], 
            cwd=BASE_DIR,
            creationflags=flags
        )

    def check_connection(self):
        if self.is_server_ready():
            self.finalize()
        else:
            QTimer.singleShot(200, self.check_connection)

    def finalize(self):
        self.splash.showMessage("Done", Qt.AlignmentFlag.AlignBottom, Qt.GlobalColor.white)
        self.view.load(QUrl(URL))
        self.view.resize(1200, 800)
        self.view.show()
        self.splash.finish(self.view)

    def run(self):
        self.start_backend()
        self.check_connection()
        
        exit_code = self.app.exec()
        
        if self.backend:
            self.backend.terminate()
            try:
                self.backend.wait(timeout=2)
            except subprocess.TimeoutExpired:
                self.backend.kill()
        
        sys.exit(exit_code)

if __name__ == "__main__":
    launcher = Launcher()
    launcher.run()
