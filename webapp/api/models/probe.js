var sqlite3 = require('sqlite3');
var db = new sqlite3.Database('../db.sqlite');

db.run("CREATE TABLE IF NOT EXISTS probe (id INTEGER PRIMARY KEY AUTOINCREMENT, name VARCHAR(20), timestamp TIMESTAMP, value FLOAT)");


function Probe(name) {
  this.name = name;
}

/**
 * record a new value
 */
Probe.prototype.push = function(value) {
  db.run("INSERT INTO probe VALUES (NULL, ?, ?, ?)",
    this.name,
    new Date(),
    value);
};

/**
 * remove all values older than 'timestamp'
 */
Probe.prototype.purgeOld = function(timestamp) {
  db.run("DELETE FROM probe WHERE name = ? AND timestamp < ?",
    this.name,
    timestamp);
};

/**
 * get the last 'n' values
 */
Probe.prototype.getLastN = function(n, success) {
  db.all("SELECT * FROM probe WHERE name = ? ORDER BY timestamp DESC LIMIT ?",
    this.name,
    n,
    success);
};

/**
 * get all the values that are newer than 'timeStamp'
 */
Probe.prototype.getAfter = function(timestamp, success) {
  db.all("SELECT * FROM probe WHERE name = ? AND timestamp > ? ORDER BY timestamp DESC",
    this.name,
    timestamp,
    success);
};

exports.Probe = Probe;
