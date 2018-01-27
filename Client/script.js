var socket = new WebSocket("ws://localhost:1234");

function addline(str) {
  document.getElementById('console-5').innerHTML = document.getElementById('console-4').innerHTML;
  document.getElementById('console-4').innerHTML = document.getElementById('console-3').innerHTML;
  document.getElementById('console-3').innerHTML = document.getElementById('console-2').innerHTML;
  document.getElementById('console-2').innerHTML = document.getElementById('console-1').innerHTML;
  document.getElementById('console-1').innerHTML = str;
}

socket.onopen = function() {
  addline("connection has been established");
};

socket.onclose = function(event) {
  if (event.wasClean) {
    addline("the onnection was closed, event code: " + "<b>" + event.code + "</b>" + ". reason: " + "<b>" + event.reason + "</b>");
  } else {
    // the server process was killed or something like this
    addline("the connection was aborted, event code: " + "<b>" + event.code + "</b>" + ". reason: " + "<b>" + event.reason + "</b>");
  }
};

socket.onmessage = function(event) {
  addline("received data: " + "<b>" + event.data + "</b>");
};

socket.onerror = function(error) {
  addline("an error occured: " + "<b>" + event.message + "</b>")
};

document.getElementById("button").onclick = function() {
  addline("sending data: " + "<b>" + document.getElementById("text_area").value + "</b>");
  socket.send(document.getElementById("text_area").value);
}
