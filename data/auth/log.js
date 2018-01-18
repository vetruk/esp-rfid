var logdata;
var logRefresh = false;

function twoDigits(value) {
	if (value < 10) {
		return "0" + value;
	}
	return value;
}

function initLogsTable() {
	jQuery(function ($) {
		FooTable.init("#latestlogtable", {
			columns: [
				{
					"name": "timestamp",
					"title": "Date",
					"parser": function (value) {
						var vuepoch = new Date(value * 1000);
						var formatted = vuepoch.getUTCFullYear() + "-" + twoDigits(vuepoch.getUTCMonth() + 1) + "-" + twoDigits(vuepoch.getUTCDate()) + "-" + twoDigits(vuepoch.getUTCHours()) + ":" + twoDigits(vuepoch.getUTCMinutes()) + ":" + twoDigits(vuepoch.getUTCSeconds());
						return formatted;
					},
					"sorted": true,
					"direction": "DESC"
            },
				{
					"name": "uid",
					"title": "UID",
					"type": "text",
          },
				{
					"name": "username",
					"title": "User Name or Label"
          }
        ],
			rows: logdata
		});
	});
}

function loadLogs() {
  logRefresh = true;
	document.getElementById("logsFieldset").disabled = true;
	document.getElementById("logs-loading-img").style.display = "none";
  websock.send("{\"command\":\"latestlog\"}");
}
