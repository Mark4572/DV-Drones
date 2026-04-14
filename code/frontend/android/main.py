import threading
import os
import time
from flask import Flask
from host import app as flask_app  # Import your existing Flask app
from kivy.app import App
from kivy.uix.vboxlayout import VBoxLayout
from jnius import autoclass # Specific to Android for WebView

class DroneApp(App):
    def build(self):
        # 1. Start Flask in a background thread
        threading.Thread(target=self.start_flask, daemon=True).start()
        
        # 2. Return a basic Kivy layout
        # Note: On Android, we usually use a service or a specific WebView library
        layout = VBoxLayout()
        return layout

    def start_flask(self):
        # Run your existing host.py logic
        flask_app.run(host='127.0.0.1', port=47000, debug=False)

    def on_start(self):
        # Use Android's native WebView via Pyjnius
        try:
            PythonActivity = autoclass('org.kivy.android.PythonActivity')
            WebView = autoclass('android.webkit.WebView')
            WebViewClient = autoclass('android.webkit.WebViewClient')
            activity = PythonActivity.mActivity

            def create_webview():
                webview = WebView(activity)
                webview.getSettings().setJavaScriptEnabled(True)
                webview.setWebViewClient(WebViewClient())
                activity.setContentView(webview)
                webview.loadUrl('http://127.0.0.1:47000/')

            # Run on the UI thread
            activity.runOnUiThread(create_webview)
        except Exception as e:
            print(f"WebView Error: {e}")

if __name__ == '__main__':
    DroneApp().run()