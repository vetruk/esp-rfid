var websock = null;
var recordstorestore = 0;
var slot = 0;
var userdata = [];
var completed = false;
var activePage = 2; // 1: logs 2: users 3 :settings
var wsUri;
var updateDeviceTime = undefined;
function showSettings() {
  activePage = 3;
	document.getElementById('settingsPanel').style.display = 'block';
	document.getElementById('usersPanel').style.display = 'none';
	document.getElementById('logsPanel').style.display = 'none';
	document.getElementById('navSettings').classList.add('active');
	document.getElementById('navUsers').classList.remove('active');
	document.getElementById('navLogs').classList.remove('active');
}

function showUsers() {
  activePage = 2;
	document.getElementById('usersPanel').style.display = 'block';
	document.getElementById('settingsPanel').style.display = 'none';
	document.getElementById('logsPanel').style.display = 'none';
	document.getElementById('navSettings').classList.remove('active');
	document.getElementById('navUsers').classList.add('active');
	document.getElementById('navLogs').classList.remove('active');
}

function showLogs() {
  activePage = 1;
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

function socketOpenListener(evt) {
	document.getElementById("settingsFieldset1").disabled = false;
	document.getElementById("settingsFieldset2").disabled = false;
	document.getElementById("usersFieldset").disabled = false;
	document.getElementById("logsFieldset").disabled = false;
}

function socketMessageListener(evt) {
	var obj = JSON.parse(evt.data);
	console.log(obj);
  if (obj.command === "piccscan") {
    if (activePage === 2) {
      listSCAN(obj);
    }
    if (obj.known === 1) {
      // update logs
      var newLogEntry = {};
      newLogEntry.uid = obj.uid;
      newLogEntry.username = obj.user;
      newLogEntry.timestamp = obj.timestamp;
      logTable.rows.add(newLogEntry);
    }
    
  } else if (obj.command === "ssidlist") {
		listSSID(obj);
	} else if (obj.command === "configfile") {
		document.getElementById("settings-loading-img").style.display = "none";
		document.getElementById("settingsFieldset1").disabled = false;
		document.getElementById("settingsFieldset2").disabled = false;
    document.getElementById('navSettings').disabled = false;
    if (obj.result == false) {
      timezone = 0;
    } else {
    	timezone = obj.timezone;
			listCONF(obj);
    }
		websock.send("{\"command\":\"latestlog\"}");
	} else if (obj.command === "gettime") {
	  if (updateDeviceTime === undefined) {
	    updateDeviceTime = setInterval(deviceTime, 1000);
	  }

		utcSeconds = obj.epoch;
	} else if (obj.command === "userlist") {
	  if (activePage == 3) {
  		document.getElementById("settings-loading-img").style.display = "none";
  		settings_haspages = obj.haspages;
  		settings_builduserdata(obj);
      if (settings_haspages === 0) {
        document.getElementById("users-loading-img").style.display = "none";
        initUsersTable();
        $(".footable-show").click();
        $(".fooicon-remove").click();
      }
	  }
	  if (activePage == 2) {
  		document.getElementById("users-loading-img").style.display = "none";
  		document.getElementById("usersFieldset").disabled = false;
  		haspages = obj.haspages;
  		builduserdata(obj);
      if (haspages === 0) {
        document.getElementById("users-loading-img").style.display = "none";
        initUsersTable();
        $(".footable-show").click();
        $(".fooicon-remove").click();
      }
  		websock.send("{\"command\":\"getconf\"}");
	  }
	} else if (obj.type === "latestlog") {
		document.getElementById("logs-loading-img").style.display = "none";
		document.getElementById("logsFieldset").disabled = false;
	  document.getElementById('navLogs').disabled = false;
		if (obj.result == true) {
		  logdata = obj.list;
  	  initLogsTable();
      $(".footable-show").click();
		}
	  websock.send("{\"command\":\"gettime\"}");
	  setInterval(browserTime, 1000);
	} else if (obj.command === "status") {
		listStats(obj);
	} else if (obj.command === "result") {
		if (obj.resultof === "userfile") {
		  if (activePage == 3) {
  			if (!completed && obj.result === true) {
  				restore1by1(slot, recordstorestore, userdata);
  			}
		  }
		} else if (obj.resultof === "userlist") {
		  if (activePage == 3) {
  			if (page < settings_haspages && obj.result === true) {
  				settings_getnextpage(page);
  			} else if (page === settings_haspages) {
  				file.type = "esp-rfid-userbackup";
  				file.version = "v0.4";
  				file.list = userdata;
  				piccBackup(file);
  			}
		  }
		  if (activePage == 2) {
        if (page < haspages && obj.result === true) {
          getnextpage();
        } else if (page === haspages) {
          initUsersTable();
          document.getElementById("users-loading-img").style.display = "none";
          $(".footable-show").click();
          $(".fooicon-remove").click();
        }
		  }
		}
	}
}

function socketCloseListener(evt) {
	document.getElementById("settingsFieldset1").disabled = true;
	document.getElementById("settingsFieldset2").disabled = true;
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
		websock.send("{\"command\":\"userlist\", \"page\":" + page + "}");
		document.getElementById("settingsFieldset1").disabled = true;
		document.getElementById("settingsFieldset2").disabled = true;
		document.getElementById("usersFieldset").disabled = true;
		document.getElementById("logsFieldset").disabled = true;
	};
}


$(document).ready(function () {
	$('[data-toggle="tooltip"]').tooltip();
});
