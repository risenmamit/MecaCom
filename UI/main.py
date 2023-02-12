from kivy.app import App
from kivy.config import Config
Config.set('kivy', 'exit_on_escape', '0')
from kivy.uix.button import Button
from kivy.uix.gridlayout import GridLayout
from kivy.core.window import Window

import asyncio
import websocket
import json
import threading

# Define the global variable for ws
ws = None

# Define the callback function for button click
def send_message(text):
    ji = {
        "isfirst": "false",
        "device": "pc",
        "target":"ESP",
        "conection":"message",
        "message" : text
    }
    ws.send(json.dumps(ji))

class MainLayout(GridLayout):
    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        self.cols=3
        self.row=3
        
        left_button = Button(text="FL")
        left_button.bind(on_press=lambda x: send_message(7))
        self.add_widget(left_button)

        forward_button = Button(text="Forward")
        forward_button.bind(on_press=lambda x: send_message(8))
        self.add_widget(forward_button)

        left_button = Button(text="FR")
        left_button.bind(on_press=lambda x: send_message(9))
        self.add_widget(left_button)

        left_button = Button(text="Left")
        left_button.bind(on_press=lambda x: send_message(4))
        self.add_widget(left_button)

        stop_button = Button(text="Stop")
        stop_button.bind(on_press=lambda x: send_message(5))
        self.add_widget(stop_button)
    
        right_button = Button(text="Right")
        right_button.bind(on_press=lambda x: send_message(6))
        self.add_widget(right_button)

        left_button = Button(text="RL")
        left_button.bind(on_press=lambda x: send_message(1))
        self.add_widget(left_button)

        reverse_button = Button(text="Reverse")
        reverse_button.bind(on_press=lambda x: send_message(2))
        self.add_widget(reverse_button)

        left_button = Button(text="RR")
        left_button.bind(on_press=lambda x: send_message(3))
        self.add_widget(left_button)

class MainApp(App):
    def __init__(self, ws, **kwargs):
        super(MainApp, self).__init__(**kwargs)
        self.ws = ws


    def build(self):
        Window.bind(on_request_close=self.on_request_close)
        layout = MainLayout()
        return layout

    def on_request_close(self, *args):
        global ws
        ji = {
        "isfirst": "false",
        "device": "pc",
        "conection":"close",
        }
        ws.send(json.dumps(ji))
        return False


def connect_ws():
    global ws
    
    def on_open(ws):
        ji = {
            "isfirst": "true",
            "device": "pc"
        }
        ws.send(json.dumps(ji))
        
    def on_message(ws, message):
        response = json.loads(response)
        conection = response["conection"]

        print("conection: ", conection)
        if conection == "true":
            print("Connection Established")
            devices = json.loads(response["devices"])
            print(f"Mobile: {devices['mobile']}\tPC: {devices['pc']}\tESP: {devices['ESP']}")

        elif conection == "newConnection" or conection == "lost":
            devices = json.loads(response["devices"])
            print(f"Mobile: {devices['mobile']}\tPC: {devices['pc']}\tESP: {devices['ESP']}")

        elif conection == "Sending":
            devices = json.loads(response["devices"])
            print(f"Mobile: {devices['mobile']}\tPC: {devices['pc']}\tESP: {devices['ESP']}")
            
        elif conection == "alreadyConected" or conection == "close":
            ws.close()
        
    def on_close(ws):
        print("Close connection")
        ws.close()
        
    socket = 'ws://192.168.0.108:3000'

    ws = websocket.WebSocketApp(socket, on_open=on_open, on_message=on_message, on_close=on_close)

    ws.run_forever()
    

if __name__ == '__main__':
    ws_thread = threading.Thread(target=connect_ws)
    ws_thread.start()
    MainApp(ws).run()
    Window.close()
    ws_thread.join()