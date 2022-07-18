var gateway = `ws://${window.location.hostname}/ws`;

var websocket;

// listen for web page event
window.addEventListener('load', onLoad);

// initialize web socket on web page load
function onLoad(event) {
    initWebSocket();
}

// initialize web socket function
function initWebSocket() {
    console.log('Opening WebSocket connection...');
    websocket = new WebSocket(gateway);
    websocket.onopen = onOpen;
    websocket.onclose = onClose;
    websocket.onmessage = onMessage;
}

// if a message is received
function onMessage(event){
    
    // parse JSON
    let json_message = JSON.parse(event.data);
    let keys = Object.keys(json_message);
    
    // update web page
    
    document.getElementById(keys[0]).innerHTML = json_message[keys[0]];
    document.getElementById(keys[1]).innerHTML = json_message[keys[1]];
    document.getElementById(keys[2]).innerHTML = json_message[keys[2]];
    document.getElementById(keys[3]).innerHTML = json_message[keys[3]];
    document.getElementById(keys[4]).innerHTML = json_message[keys[4]];
    document.getElementById(keys[5]).innerHTML = json_message[keys[5]];
    
    
    
}
// if a new client connects
function onOpen(event) {
    console.log('Connection established');
}

// if a client disconnects
function onClose(event) {
    console.log('Connection closed');
    setTimeout(initWebSocket, 3000);
}

/*
    LED controls
 */

// led 1 on
function led1on() {
    websocket.send('led1on');
}

// led 1 off
function led1off() {
    websocket.send('led1off');
}

// led 2 on
function led2on() {
    websocket.send('led2on');
}

// led 2 off
function led2off() {
    websocket.send('led2off');
}

// led 3 on
function led3on() {
    websocket.send('led3on');
}

// led 3 off
function led3off() {
    websocket.send('led3off');
}

// led 4 on
function led4on() {
    websocket.send('led4on');
}

// led 4 off
function led4off() {
    websocket.send('led4off');
}

// update servo angle based on user input once the user clicks set
function updateServoAngle(){

    // get the value entered by user
    let angle_value = document.getElementById('servo').value;

    // convert to string because the input type is a number
    websocket.send("servo"+angle_value.toString());
}

// get light at interval of 5 seconds
setInterval(function(){
    
    websocket.send('get-ambient-light');
    
}, 5000);

setInterval(function(){
    
    websocket.send('get-servo-angle');
    
}, 1000);


