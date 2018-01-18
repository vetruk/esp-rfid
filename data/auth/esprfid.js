var websock = null;
var utcSeconds;
var recordstorestore = 0;
var slot = 0;
var userdata = [];
var completed = false;

function showSettings() {
	document.getElementById('settingsPanel').style.display = 'block';
	document.getElementById('usersPanel').style.display = 'none';
	document.getElementById('logsPanel').style.display = 'none';
	document.getElementById('navSettings').classList.add('active');
	document.getElementById('navUsers').classList.remove('active');
	document.getElementById('navLogs').classList.remove('active');
}

function showUsers() {
	document.getElementById('usersPanel').style.display = 'block';
	document.getElementById('settingsPanel').style.display = 'none';
	document.getElementById('logsPanel').style.display = 'none';
	document.getElementById('navSettings').classList.remove('active');
	document.getElementById('navUsers').classList.add('active');
	document.getElementById('navLogs').classList.remove('active');
}

function showLogs() {
	document.getElementById('logsPanel').style.display = 'block';
	document.getElementById('usersPanel').style.display = 'none';
	document.getElementById('settingsPanel').style.display = 'none';
	document.getElementById('navSettings').classList.remove('active');
	document.getElementById('navUsers').classList.remove('active');
	document.getElementById('navLogs').classList.add('active');
}

function mouseoverPass(name) {
	var obj = document.getElementById(name);
	obj.type = "text";
}

function mouseoutPass(name) {
	var obj = document.getElementById(name);
	obj.type = "password";
}


var wsUri;

function socketOpenListener(evt) {
	document.getElementById("settingsFieldset").disabled = false;
	document.getElementById("usersFieldset").disabled = false;
	document.getElementById("logsFieldset").disabled = false;
}

function socketMessageListener(evt) {
	var obj = JSON.parse(evt.data);
	console.log(obj);
	if (obj.command === "ssidlist") {
		listSSID(obj);
	} else if (obj.command === "configfile") {
		document.getElementById("settings-loading-img").style.display = "none";
		document.getElementById("settingsFieldset").disabled = false;
		listCONF(obj);
		websock.send("{\"command\":\"userlist\", \"page\":" + page + "}");
	} else if (obj.command === "gettime") {
		utcSeconds = obj.epoch;
	} else if (obj.command === "userlist") {
		document.getElementById("users-loading-img").style.display = "none";
		document.getElementById("usersFieldset").disabled = false;
		settings_haspages = obj.haspages;
		settings_builduserdata(obj);
    if (settings_haspages === 0) {
      document.getElementById("users-loading-img").style.display = "none";
      initUsersTable();
      $(".footable-show").click();
      $(".fooicon-remove").click();
    }
		websock.send("{\"command\":\"latestlog\"}");
	} else if (obj.type === "latestlog") {
		document.getElementById("logs-loading-img").style.display = "none";
		document.getElementById("logsFieldset").disabled = false;
		if (obj.result == true) {
		  logdata = obj.list;
		}
	  initLogsTable();
    $(".footable-show").click();
		websock.send("{\"command\":\"gettime\"}");
	} else if (obj.command === "status") {
		listStats(obj);
	} else if (obj.command === "result") {
		if (obj.resultof === "userfile") {
			if (!completed && obj.result === true) {
				restore1by1(slot, recordstorestore, userdata);
			}
		} else if (obj.resultof === "userlist") {
			if (page < settings_haspages && obj.result === true) {
				settings_getnextpage(page);
			} else if (page === settings_haspages) {
				file.type = "esp-rfid-userbackup";
				file.version = "v0.4";
				file.list = userdata;
				piccBackup(file);
			}
		}
	}
}


function socketCloseListener(evt) {
	console.log('socket closed');
	document.getElementById("settingsFieldset").disabled = true;
	document.getElementById("usersFieldset").disabled = true;
	document.getElementById("logsFieldset").disabled = true;
	websock = new WebSocket(wsUri);
	websock.addEventListener('open', socketOpenListener);
	websock.addEventListener('message', socketMessageListener);
	websock.addEventListener('close', socketCloseListener);
	websock.addEventListener('error', socketErrorListener);
}

function socketErrorListener(evt) {
	console.log('socket error');
	console.log(evt);
}

function start() {
	var protocol = 'ws://';
	if (window.location.protocol === 'https:') {
		protocol = 'wss://';
	}
	wsUri = protocol + window.location.hostname + "/ws";
	websock = new WebSocket(wsUri);
	websock.addEventListener('message', socketMessageListener);
	websock.addEventListener('error', socketErrorListener);
	websock.addEventListener('close', socketCloseListener);

	websock.onopen = function (evt) {
		console.log('websocket on open');
		websock.send("{\"command\":\"getconf\"}");
		document.getElementById("settingsFieldset").disabled = true;
		document.getElementById("usersFieldset").disabled = true;
		document.getElementById("logsFieldset").disabled = true;
	};
}


$(document).ready(function () {
	$('[data-toggle="tooltip"]').tooltip();
});
