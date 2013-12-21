var Probe = require('../models/probe').Probe;

exports.getLastN = function(req, res) {
  var probe = new Probe(req.params.name),
      n = req.params.n;

  probe.getLastN(n, function(err, rows) {
    res.json({
      success: true,
      data: rows
    });
  });
};


exports.getAfter = function(req, res) {
  var probe = new Probe(req.params.name),
      timestamp = req.params.timestamp;

  probe.getAfter(timestamp, function(err, rows) {
    res.json({
      success: true,
      data: rows
    });
  });
};

exports.push = function(req, res) {
  var probe = new Probe(req.params.name),
      value = req.params.value;

  probe.push(value);

  res.json({
      success: true,
  });
};