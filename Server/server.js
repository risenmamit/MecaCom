const WebSocket = require('ws');

const wss = new WebSocket.Server({
  port: 3000,
  host: '192.168.0.107'
});

const clients = new Map();

var mobile = "false";
var ESP = "false";
var pc = "false";



wss.on('connection', (ws) => {
  var copy = false;
  const id = uuidv4();
  var device;
  console.log("Connected");

  ws.on('message', (messageAsString) => {
    const message = JSON.parse(messageAsString);
    console.log(message);
    if (message.isfirst == "true") {
      if (ispresent(message.device) && message.device != "ESP") {
        ws.send(JSON.stringify({
          conection: "alreadyConected",
          devices: JSON.stringify({
            mobile: mobile,
            ESP: ESP,
            pc: pc
          })
        }));
        ws.close();
        copy = true;
      }else {
        device = message.device;
        const metadata = {
          id,
          device
        };
        if (device == "mobile") {
          mobile = "true";

        } else if (device == "ESP") {
          ESP = "true";

        } else if (device == "pc") {
          pc = "true";
        }
        [...clients.keys()].forEach((client) => {
          client.send(JSON.stringify({
    
            conection: "newConnection",
    
            devices: JSON.stringify({
              mobile: mobile,
              ESP: ESP,
              pc: pc
            })
    
          }));
        });
        clients.set(ws, metadata);
        console.log(device);

        ws.send(JSON.stringify({
          conection: "true",
          devices: JSON.stringify({
            mobile: mobile,
            ESP: ESP,
            pc: pc
          })
        }));
      }
    }
    else if(message.isfirst == "false"){
      if(message.device == "mobile" || message.device == "pc"){
        clients.forEach((value, key, map) => {
          if(value.device == message.target){
            key.send(JSON.stringify({
              conection:"message",
              message:message.message
            }));
          }
          if(value.device == message.device){
            key.send(JSON.stringify({
              conection:"message",
              message:"Sent"
            }));
          }
        })
      }
    }
  });

  ws.on("close", () => {
    if(!copy){
      if (device == "mobile") {
        mobile = "false";

      } 
      else if (device == "pc") {
        pc = "false";

      }
      clients.delete(ws);
      [...clients.keys()].forEach((client) => {
        client.send(JSON.stringify({

          conection: "lost",

          devices: JSON.stringify({
            mobile: mobile,
            ESP: ESP,
            pc: pc
        })

      }));
    });
    console.log(device + " lost conection");
  }else{
    copy = false;
  }
  });
  ws.onerror = function () {
    console.log("Some Error occurred");
}
})

function uuidv4() {
  return 'xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx'.replace(/[xy]/g, function (c) {
    var r = Math.random() * 16 | 0,
      v = c == 'x' ? r : (r & 0x3 | 0x8);
    return v.toString(16);
  });
}

function ispresent(dev){
  if(dev == "pc"){
    return pc == "true";
  }
  if(dev == "mobile"){
    return mobile == "true";
  }
  return false;
}

console.log("wss up");