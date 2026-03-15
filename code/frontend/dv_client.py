import sys, os, subprocess, ctypes
from PyQt6.QtWidgets import QApplication, QSplashScreen
from PyQt6.QtWebEngineWidgets import QWebEngineView
from PyQt6.QtCore import QUrl, Qt, QTimer
from PyQt6.QtGui import QPixmap, QIcon


if sys.platform == 'win32':
    
    myappid = 'mark4572.dvclient.v0.2' 
    ctypes.windll.shell32.SetCurrentProcessExplicitAppUserModelID(myappid)

BASE_DIR = os.path.dirname(os.path.abspath(__file__))
URL = "http://127.0.0.1:47000/"
icon_path = os.path.join(BASE_DIR, "vscode32.png")


if not os.path.exists(icon_path):
    print(f"Icon not found at: {icon_path}")

app = QApplication(sys.argv)

app_icon = QIcon(icon_path)
app.setWindowIcon(app_icon)


splash_pix = QPixmap(os.path.join(BASE_DIR, 'placeholder.ico'))
splash = QSplashScreen(splash_pix)
splash.setWindowIcon(app_icon) 
splash.show()
splash.showMessage("Init Client", Qt.AlignmentFlag.AlignBottom | Qt.AlignmentFlag.AlignCenter, Qt.GlobalColor.white)


backend = subprocess.Popen([sys.executable, "host.py"], cwd=BASE_DIR)

view = QWebEngineView()
view.setWindowTitle("DV Client")
view.setWindowIcon(app_icon) 
view.setContextMenuPolicy(Qt.ContextMenuPolicy.NoContextMenu)

def finalize():
    view.load(QUrl(URL))
    view.resize(1200, 800) 
    view.show()
    splash.finish(view)


QTimer.singleShot(100, finalize)

exit_code = app.exec()
backend.terminate()
sys.exit(exit_code)