import asyncio
# import ssl

import websockets
import json
#import os
import datetime
#import time

async def test(loop):
    async with websockets.connect('ws://192.168.116.42:3000') as websocket:
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
            out = input("Enter\n 1:Forward\n 2:Reverce\n 3:Stop\n 4:Right\n 5:Left\n 6:fr\n 7:fl\n 8:rr\n 9:rl\n")
            if out == "1":
                out = 8
            elif out == "2":
                out = 2
            elif out == "3":
                out = 5
            elif out == "4":
                out = 6
            elif out == "5":
                out = 4
            elif out == "6":
                out = 9
            elif out == "7":
                out = 7
            elif out == "8":
                out = 3
            elif out == "9":
                out = 1
            else:
                out = 5
            ji = {
                "isfirst": "false",
                "device": "pc",
                "target":"ESP",
                "conection":"message",
                "message" : out
            }
            print(datetime.datetime.now())
            await websocket.send(json.dumps(ji))
            #time.sleep(0.5)
            response = await websocket.recv()
            response = json.loads(response)
            conection = response["conection"]

            if conection == "true":
                print("Connection Established")
                devices = json.loads(response["devices"])
                print(f"Mobile: {devices['mobile']}\tPC: {devices['pc']}\tESP: {devices['ESP']}")

            elif conection == "newConnection" or conection == "lost":
                devices = json.loads(response["devices"])
                print(f"Mobile: {devices['mobile']}\tPC: {devices['pc']}\tESP: {devices['ESP']}")
                
            elif conection == "alreadyConected":
                break


loop = asyncio.get_event_loop()

x = loop.run_until_complete(test(loop))