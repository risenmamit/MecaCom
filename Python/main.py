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
        while True:
            out = input("Enter F or R")
            ji = {
                "isfirst": "false",
                "device": "pc",
                "target":"ESP",
                "conection":"message",
                "message" : out
            }
            await websocket.send(json.dumps(ji))
            
            # response = await websocket.recv()
            # response = json.loads(response)
            # conection = response["conection"]
            
            # print(response)
            # out = loop.call_soon(sendMsg())
            # print(out)
            # if(True):
            #     ji = {
            #         "isfirst": "false",
            #         "device": "pc",
            #         "target":"ESP",
            #         "conection":"message",
            #         "value" : out
            #     }
            #     await websocket.send(json.dumps(ji))

            # if conection == "true":
            #     print("Connection Established")
            #     devices = json.loads(response["devices"])
            #     print(f"Mobile: {devices['mobile']}\tPC: {devices['pc']}\tESP: {devices['ESP']}")

            # elif conection == "newConnection" or conection == "lost":
            #     devices = json.loads(response["devices"])
            #     print(f"Mobile: {devices['mobile']}\tPC: {devices['pc']}\tESP: {devices['ESP']}")
                
            # elif conection == "alreadyConected":
            #     break
            
            # elif conection == "message":
            #     pass
                # print("response")
                # if response["message"] == "teamviewer":
                #     os.startfile("C:\\Program Files\\TeamViewer\\TeamViewer.exe")
                # elif response["message"] == "jupyternotebook":
                #     os.startfile("C:\\Users\\Madwesh\\PycharmProjects\\pysocket\\jupyter.bat")


loop = asyncio.get_event_loop()

x = loop.run_until_complete(test(loop))