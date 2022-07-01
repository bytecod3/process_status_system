var gateway = `ws://${window.location.hostname}/ws`;

var websocket;

// listen for web page event
window.addEventListener('load', onLoad);

// initialize web socket on web page load
function onLoad(event) {
    initWebSocket();
}

// send current values on initial page load
function getCurrentServoAngle(){
    websocket.send("get_servo_angle");
}

// initialize web socket function
function initWebSocket() {
    console.log('Opening WebSocket connection...');
    websocket = new WebSocket(gateway);
    websocket.onopen    = onOpen;
    websocket.onclose   = onClose;
    websocket.onmessage = onMessage;
}

// if a new client connects
function onOpen(event) {
    console.log('Connection opened');
}

// if a client disconnects
function onClose(event) {
    console.log('Connection closed');
    setTimeout(initWebSocket, 2000);
}

// process a newly received message
function onMessage(event) {
    // convert received data to JSON
    console.log(event.data);
    let message_object = JSON.parse(event.data);
    let message_keys = Object.keys(message_object);

    for (let i = 0; i < message_keys.length; i++){
        let key = message_keys[i];

        // get the LED IDs and set the current state value e.g key=>led1 : value=> 1
        document.getElementById(key).innerHTML = message_object[key];
    }

    console.log(message_object);
}

// update servo angle based on user input once the user clicks set
function updateServoAngle(){

    // get the value entered by user
    let angle_value = document.getElementById('servo').value;

    // log for debug
    console.log(angle_value);

    // convert to string because the input type is a number
    websocket.send("servo"+angle_value.toString());
}

/*
    LED controls
 */

// led 1 on
function led1on(){
    websocket.send('led1on');
}

// led 1 off
function led1off(){
    websocket.send('led1off');
}

// led 2 on
function led2on(){
    websocket.send('led2on');
}

// led 2 off
function led2off(){
    websocket.send('led2off');
}
