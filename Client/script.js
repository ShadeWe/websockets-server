window.onload = function() {

  var socket;

  document.getElementById("connect-button").onclick = function() {
      ﻿socket = new WebSocket("ws://localhost:1234");

      socket.onopen = whenOpen;
      socket.onclose = whenClose;
      socket.onmessage = whenMessage;
      socket.onerror = whenError;
  };

  // HERE COME WEBSOCKET FUNCTIONS

  function whenOpen() {
    addline("connection has been established");
  }

  function whenClose(event) {
    if (event.wasClean) {
      addline("the onnection was closed, event code: " + "<b>" + event.code + "</b>" + ". reason: " + "<b>" + event.reason + "</b>");
    } else {
      // the server process was killed or something like this
      addline("the connection was aborted, event code: " + "<b>" + event.code + "</b>" + ". reason: " + "<b>" + event.reason + "</b>");
    }
  }

  function whenMessage() {
    console.log('darova');
    addline("received data: " + "<b>" + event.data + "</b>");
  }

  function whenError() {
    addline("an error occured: " + "<b>" + event.message + "</b>");
  }


  // HERE COMES OTHER FUNCTIONS

  // the console at the bottom left corner
  function addline(str) {
    document.getElementById('console-5').innerHTML = document.getElementById('console-4').innerHTML;
    document.getElementById('console-4').innerHTML = document.getElementById('console-3').innerHTML;
    document.getElementById('console-3').innerHTML = document.getElementById('console-2').innerHTML;
    document.getElementById('console-2').innerHTML = document.getElementById('console-1').innerHTML;
    document.getElementById('console-1').innerHTML = str;
  }

  // when clicked to submit
  document.getElementById("button").onclick = function() {
    addline("sending data: " + "<b>" + document.getElementById("text_area").value + "</b>");
    appendMessageFromClient("It's me", document.getElementById("text_area").value);
    socket.send(document.getElementById("text_area").value);
  }

  function appendMessageFromServer(text) {

    let wrap = document.createElement("div");
    wrap.className = 'message';
    let client_name = document.createElement("div");
    client_name.className = 'client-name-from-server';
    client_name.textContent = 'Client 1';
    wrap.appendChild(client_name);

    let messageBox = document.createElement('div');
    messageBox.className = 'message-box';
    messageBox.textContent = '';

    wrap.appendChild(messageBox);

    document.getElementById('dialog-box').appendChild(wrap);

  }

  function appendMessageFromClient(name, text) {

    let wrap = document.createElement("div");
    wrap.className = 'message';
    let client_name = document.createElement("div");
    client_name.className = 'client-name-from-client';
    client_name.textContent = name;
    wrap.appendChild(client_name);

    let messageBox = document.createElement('div');
    messageBox.className = 'message-box';
    messageBox.textContent = text;

    wrap.appendChild(messageBox);

    document.getElementById('dialog-box').appendChild(wrap);

  }



  // debug buttons
  document.getElementById('debug').onclick = function() {
    appendMessageFromServer('Client 1');
  }
  document.getElementById('debug2').onclick = function() {
    appendMessageFromClient('Client 1');
  }





}
