from unittest import case

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

#Flask server
class Server:
    def __init__(self):
        self.svr = fs.Flask(__name__, static_folder="static", static_url_path="/static")
        self.register_routes()

    def init(self):
        return fs.send_file("init.html")

    def status(self):
        return fs.send_file("status.html")

    def api(self):
        return fs.send_file("api.js")
    
    def home(self):
        return fs.send_file("home.html")
    
    def update(self):
        return fs.send_file("update.html")
    
    def cs(self):
        global ready
        return fs.jsonify({"ready": ready})
    
    def theme(self):
        return fs.send_file("index.html")
    
    def error(self, code):
        match code:
            case 0:
                return """
                 <!DOCTYPE html>
                <html>
                <head>
                  <title>Error</title>
                 </head>
                <body>
                  <h1>001 - No Connection</h1>
                      <p>Please check your internet connection and try again.</p>
                     </body>
                 </html>
                        """
    


    def register_routes(self):
        self.svr.add_url_rule("/", "init", self.init)
        self.svr.add_url_rule("/api/status", "status", self.status)
        self.svr.add_url_rule("/home", "home", self.home)
        self.svr.add_url_rule("/api", "api", self.api)
        self.svr.add_url_rule("/update", "update", self.update)
        self.svr.add_url_rule("/api/ready", "cs", self.cs)

    def add_route(self, rule, endpoint, view_func, methods=None):
        if methods is None:
         methods = ["GET"]
         self.svr.add_url_rule(rule, endpoint, view_func, methods=methods)
         
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
            return response.json()
        else:
            error(0)
            return None
    except Exception:
        error(0)
        return None

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
    
    def add_thread(target, args=()):
        new_thread = threading.Thread(target=target, args=args, daemon=True)
        new_thread.start()
        return new_thread
        
    def remove_thread(thread):
        thread.thread.join()
        
    def init_thread(thread):
        thread.thread = threading.Thread(target=thread.target, args=thread.args)
        thread.thread.start()
        
    def stop_thread(thread):
        thread.thread.join()
        
    def restart_thread(thread):
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
    
    def init ():
        if not os.path.exists("config.json"):
          print("Error: config.json not found.")
          return error(7)
        else:
          return load.cfg("config.json")

def run(fs):
    fs = Server()
    fs.register_routes()
    fs.svr.run(host="0.0.0.0", port=505, debug=False, use_reloader=False)

if __name__ == "__main__":
        
    thread.add_thread(target=run, args=(fs,))
    load.app()
    install.update("latest.json")
    qt_app = QApplication(sys.argv)
    window = vx()
    window.show()
    client.connect("127.0.0.1", 504)            
    sys.exit(qt_app.exec()) 
    

