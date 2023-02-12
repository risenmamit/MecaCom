const WebSocket = require('ws');

const wss = new WebSocket.Server({
  port: 3000,
  host: '192.168.0.108'
});

const clients = new Map();

var mobile = "false";
var ESP = "false";
var pc = "false";


wss.setMaxListeners(0);
wss.on('connection', (ws) => {
  var copy = false;
  const id = uuidv4();
  var device;
  //console.log("Connected");

  ws.on('message', (messageAsString) => {
    const message = JSON.parse(messageAsString);
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

        ws.send(JSON.stringify({
          conection: "true",
          devices: JSON.stringify({
            mobile: mobile,
            ESP: ESP,
            pc: pc
          })
        }));
        console.log(device);
      }
    }
    else if(message.isfirst == "false"){
      if(message.conection == "close"){
        ws.send(JSON.stringify({
          conection: "close",
          devices: JSON.stringify({
            mobile: mobile,
            ESP: ESP,
            pc: pc
          })
        }));
        ws.close();
      }
      else if(message.device == "mobile" || message.device == "pc"){
        clients.forEach((value, key, map) => {
          if(value.device == message.target){
            key.send(JSON.stringify({
              conection:"message",
              message:message.message
            }));
            ws.send(JSON.stringify({
              conection: "Sending",
              devices: JSON.stringify({
                mobile: mobile,
                ESP: ESP,
                pc: pc
              })
            }));
          }
            
        })
      }
    }
    console.log(message);
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
  ws.onerror = function(error) {
    console.error("WebSocket error: ", error);
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
    console.log(device + " lost conection due to an error");
  };
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