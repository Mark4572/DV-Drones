import os
from flask import Flask, send_file

app = Flask(__name__)


app.config['SECRET_KEY'] = os.environ.get('SECRET_KEY', 'admin')

@app.route('/')
def home():
    return send_file(os.path.join(os.path.dirname(__file__), 'index.html'))

@app.route('/dashboard.css')
def dashboard_css():
    return send_file(os.path.join(os.path.dirname(__file__), 'dashboard.css'))

@app.route('/dashboard.js')
def dashboard_js():
    return send_file(os.path.join(os.path.dirname(__file__), 'dashboard.js'))

@app.route('/cam.jpg')
def camera_image():
    return send_file(os.path.join(os.path.dirname(__file__), 'cam.jpg'))

@app.route('/data')
def data():
    return send_file(os.path.join(os.path.dirname(__file__), 'data.html'))

@app.route('/config.html')
def config():
    return send_file(os.path.join(os.path.dirname(__file__), 'config.html'))

@app.route('/home')
def main_home():
    return send_file(os.path.join(os.path.dirname(__file__), 'home.html'))

@app.route('/home.css')
def home_css():
    return send_file(os.path.join(os.path.dirname(__file__), 'home.css'))


@app.route('/dashboard.html')
def dashboard():
    return send_file(os.path.join(os.path.dirname(__file__), 'dashboard.html'))

@app.route('/dev.html')
def dev():
    return send_file(os.path.join(os.path.dirname(__file__), 'dev.html'))



if __name__ == "__main__":

    port = int(os.environ.get("PORT", 47000))
    app.run(host='0.0.0.0', port=port)