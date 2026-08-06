import flask as fs
import os
import threading
import sys
import PyQt6
from PyQt6.QtCore import QUrl     
from PyQt6.QtGui import QIcon                     
from PyQt6.QtWidgets import QApplication, QMainWindow
from PyQt6.QtWebEngineWidgets import QWebEngineView 
from flask import json
import requests as rq
import time

#global variables

ready = False
connect = False
uptodate = True

class Server:
    def __init__(self):
        self.base_dir = os.path.dirname(os.path.abspath(__file__))
        print(f"[DEBUG] Webfolder: {self.base_dir}")

        self.svr = fs.Flask(__name__, static_folder="static", static_url_path="/static")
        self.routes()

    def init(self):
        path = os.path.join(self.base_dir, "init.html")
        return fs.send_file(path)

    def status(self):
        path = os.path.join(self.base_dir, "status.html")
        return fs.send_file(path)

    def api(self):
        path = os.path.join(self.base_dir, "api.js")
        return fs.send_file(path)
    
    def theme_js(self):
        path = os.path.join(self.base_dir, "theme.js")
        return fs.send_file(path)
    
    def home(self):
        path = os.path.join(self.base_dir, "home.html")
        return fs.send_file(path)
    
    def update(self):
        path = os.path.join(self.base_dir, "update.html")
        return fs.send_file(path)
    
    def cs(self):
        global ready
        return fs.jsonify({"ready": ready})
    
    def theme(self):
        path = os.path.join(self.base_dir, "index.html")
        return fs.send_file(path)

    def get_version(self):
        path = os.path.join(self.base_dir, "latest.json")
        return fs.send_file(path)
    
    def error(self, code):
        match code:
            case 0: return '<h1>000 - No Connection</h1>'
            case 1: return '<h1>001 - Invalid response</h1>'
            case 2: return '<h1>002 - Update error</h1>'
            case 3: return '<h1>003 - Download error</h1>'
            case 4: return '<h1>004 - Parse error</h1>'
            case 5: return '<h1>005 - Invalid API</h1>'
            case 6: return '<h1>006 - Invalid Login</h1>'
            case 7: return '<h1>007 - Config file error</h1>'
            case 403: return '<h1>403 - Forbidden</h1>'
            case 404: return '<h1>404 - Not found</h1>'
            case 500: return '<h1>500 - Internal Server error</h1>'
            case _: return '<h1>Error</h1>'
            
    def routes(self):
        self.svr.route("/", methods=["GET"])(self.theme)
        self.svr.route("/theme.js", methods=["GET"])(self.theme_js)
        self.svr.route("/init", methods=["GET"])(self.init)
        self.svr.route("/api/status", methods=["GET"])(self.status)
        self.svr.route("/home", methods=["GET"])(self.home)
        self.svr.route("/api", methods=["GET"])(self.api)
        self.svr.route("/update", methods=["GET"])(self.update)
        self.svr.route("/api/ready", methods=["GET"])(self.cs)
        self.svr.route("/error", methods=["GET"])(self.error)
        self.svr.route("/latest.json", methods=["GET"])(self.get_version)
        
    def add_route(self, rule, view_func, methods=None):
        if methods is None:
            methods = ["GET"]
        self.svr.route(rule, methods=methods)(view_func)
         
def error(code):
    match code:
        case 0:
            print("Error: No Connection.")
        case 1:
            print("Error: Invalid response from the server.")
        case 2:
            print("Error: update could not be installed.")
        case 3:
            print("Error: update could not be downloaded.")
        case 4:
            print("Error: Data could not be parsed.")
        case 5:
            print("Error: Invalid API response.")
        case 6:
            print("Error: Invalid API login")
        case 7:
            print("Error: Config file is missing or corrupted.")
        case 403:
            print("Error: Access denied.")
            fs.abort(403)
        case 404:
            print("Error: Resource not found.")
            fs.abort(404)
        case 500:
            print("Error: Internal server error.")
            fs.abort(500)
        case "bug":
            print("bug")
        case -1:
            print("Error: Unknown error occurred.")
                
def update():
    API = "http://127.0.0.1:504/api/update"
    try:
        response = rq.get(API, timeout=3)
        if response.status_code == 200:
            global uptodate
            uptodate = response.json().get("uptodate", False)
            return response.json()
        else:
            error(0)
            return None
    except Exception:
        error(0)
        return None

def init ():
    if not os.path.exists("config.json"):
        print("Error: config.json not found.")
        return error(7)
    else:
        return load.cfg("config.json")

class load:
    def cfg(file):
        with open(file, "r") as f:
            try:
                data = json.load(f)
                return data
            except json.JSONDecodeError:
                return error(7)
    def update(file):
        with open(file, "w") as f:
            try:
                json.dump(f, indent=4)
            except Exception as e:
                return error(2)
    def app():
        global ready
        startparameters = [update]
        if all(startparameters):
            
            ready = True
         
class install:
    def update(file):
        global update
        with open(file, "w") as f:
            try:
                json.dump(f, indent=4)
                update = True
            except Exception as e:
                return error(2)
            time.sleep(5000)
            update = True
                      
class save:
    def cfg(file, parameters):
        with open(file, "w") as f:
            try:
                json.dump(parameters, f, indent=4)
            except Exception as e:
                return error(2)
            
class vx(QMainWindow):
    def __init__(self):
        super().__init__()
        
        app = self
        
        app.browser = QWebEngineView()
        app.setWindowTitle("DV Client")
        app.resize(800, 600)
        app.browser.setUrl(QUrl("http://127.0.0.1:505/"))
        app.setCentralWidget(app.browser)
        
        bsfolder = os.path.dirname(os.path.abspath(__file__))
        icopath = os.path.join(bsfolder, "static", "icon.ico")
        app.setWindowIcon(QIcon(icopath))

class thread(threading.Thread):
    @staticmethod
    
    def add(target, args=()):
        new_thread = threading.Thread(target=target, args=args, daemon=True)
        new_thread.start()
        return new_thread
        
    def remove(thread):
        thread.thread.join()
        
    def init(thread):
        thread.thread = threading.Thread(target=thread.target, args=thread.args)
        thread.thread.start()
        
    def stop(thread):
        thread.thread.join()
        
    def restart(thread):
        thread.thread.join()
        thread.thread = threading.Thread(target=thread.target, args=thread.args)
        thread.thread.start()

class client:
    def connect(ip, port):
        try:
            response = rq.get(f"http://{ip}:{port}/api/ready", timeout=3)
        except Exception:
            global connect
            connect = False
            error(0)
            return False
        connect = True
        
    def init(fs):
        fs = Server()
        fs.routes()
        fs.svr.run(host="0.0.0.0", port=505, debug=False, use_reloader=False)
          
if __name__ == "__main__":
        
    thread.add(target=client.init, args=(fs,))
    thread.add(target=vx.window)
    load.app()
    install.update("latest.json")
    qt_app = QApplication(sys.argv)
    window = vx()
    window.show()
    client.connect("127.0.0.1", 504)            
    sys.exit(qt_app.exec()) 
    

