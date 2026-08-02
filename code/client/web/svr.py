import flask as fs
import os
import PyQt6
import PyQt6.QtCore
import PyQt6.QtWidgets
from flask import json
import requests as rq

api = "http://127.0.0.1:504/api/client/update"


#Flask server
class Server:
    def __init__(self):
        self.svr = fs.Flask(__name__, static_folder=".", static_url_path="/static")
        self.register_routes()

    def index(self):
        return fs.send_file("index.html")

    def status(self):
        return fs.send_file("status.html")

    def api(self):
        return fs.send_file("api.js")


    def register_routes(self):
        self.svr.add_url_rule("/", "index", self.index)
        self.svr.add_url_rule("/api/status", "status", self.status)
        self.svr.add_url_rule("/api", "api", self.api)


    def add_route(self, rule, endpoint, view_func, methods=None):
        if methods is None:
         methods = ["GET"]
         self.svr.add_url_rule(rule, endpoint, view_func, methods=methods)
         
def error():
    match code:
        case 000:
            print("Error: No Connection.")
        case 001:
            print("Error: Invalid response from the server.")
        case 002:
            print("Error: update could not be installed.")
        case 003:
            print("Error: update could not be downloaded.")
        case 004:
            print("Error: Data could not be parsed.")
        case 005:
            print("Error: Invalid API response.")
        case 006:
            print("Error: Invalid API login")
        case 007:
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
    
         
    
def update(api):
    rq.response(code) = rq.get(api)
    
    if rq.Response.status_code == 200:
        data = rq.Response.json
    else:
        error(000)
        return data

        
def init ():
    if not os.path.exists("config.json"):
        print("Error: config.json not found.")
        return error(007)
    else:
        return load.cfg_load("config.json")

class load:
    def cfg_load(file):
        with open(file, "r") as f:
            try:
                data = json.load(f)
                return data
            except json.JSONDecodeError:
                return error(007)

def run():
    server = Server()
    server.register_routes()
    server.svr.run(host="0.0.0.0", port=505, debug=False)

if __name__ == "__main__":
    run()
    update()

