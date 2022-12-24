import asyncio
# import ssl

import websockets
import json
import os

async def test(loop):
    async with websockets.connect('ws://192.168.72.201:3000') as websocket:
        ji = {
            "isfirst": "true",
            "device": "pc"
        }
        await websocket.send(json.dumps(ji))
        response = await websocket.recv()
        response = json.loads(response)
        conection = response["conection"]
        
        print(response)
        while True:
            out = input("Enter\n 1:Forward\n 2:Reverce\n 3:Stop\n")
            ji = {
                "isfirst": "false",
                "device": "pc",
                "target":"ESP",
                "conection":"message",
                "message" : int(out)
            }
            await websocket.send(json.dumps(ji))

            # if conection == "true":
            #     print("Connection Established")
            #     devices = json.loads(response["devices"])
            #     print(f"Mobile: {devices['mobile']}\tPC: {devices['pc']}\tESP: {devices['ESP']}")

            # elif conection == "newConnection" or conection == "lost":
            #     devices = json.loads(response["devices"])
            #     print(f"Mobile: {devices['mobile']}\tPC: {devices['pc']}\tESP: {devices['ESP']}")
                
            if conection == "alreadyConected":
                break
            
            # elif conection == "message":
            #     pass
                # print("response")
                # if response["message"] == "teamviewer":
                #     os.startfile("C:\\Program Files\\TeamViewer\\TeamViewer.exe")
                # elif response["message"] == "jupyternotebook":
                #     os.startfile("C:\\Users\\Madwesh\\PycharmProjects\\pysocket\\jupyter.bat")


loop = asyncio.get_event_loop()

x = loop.run_until_complete(test(loop))