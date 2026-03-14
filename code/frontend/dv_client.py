import sys
import os
import subprocess
from PyQt6.QtWidgets import QApplication, QMainWindow, QSplashScreen
from PyQt6.QtWebEngineWidgets import QWebEngineView
from PyQt6.QtCore import QUrl, QTimer, Qt
from PyQt6.QtGui import QIcon, QPixmap

class Browser(QMainWindow):
    def __init__(self, port):
        super().__init__()
        self.server_url = f"http://127.0.0.1:{port}/"
        
        self.setWindowTitle("DV Client")
        self.setGeometry(100, 100, 1280, 800)
        self.browser = QWebEngineView()
        self.browser.setContextMenuPolicy(Qt.ContextMenuPolicy.NoContextMenu) 
        
        self.setCentralWidget(self.browser)

    def load_content(self):
        self.browser.setUrl(QUrl(self.server_url))
        self.showMaximized()

def start_backend(base_dir, port):
    host_path = os.path.join(base_dir, 'host.py')
    return subprocess.Popen([sys.executable, host_path], cwd=base_dir)

if __name__ == "__main__":
    app = QApplication(sys.argv)
    base_dir = os.path.dirname(os.path.abspath(__file__))


    splash_path = os.path.join(base_dir, 'code/frontend/drohne.ico')
    splash = QSplashScreen(QPixmap(splash_path))
    splash.show()
    splash.showMessage("Init Client", Qt.AlignmentFlag.AlignBottom | Qt.AlignmentFlag.AlignCenter, Qt.GlobalColor.white)

    host_proc = start_backend(base_dir, 47000)

    window = Browser(47000)

    def finalize_launch():
        window.load_content()
        splash.finish(window) 

   
    QTimer.singleShot(0, finalize_launch)

    exit_code = app.exec()
    if host_proc:
        host_proc.terminate()
    sys.exit(exit_code)
