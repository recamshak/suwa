var exec = require('child_process').exec;

function execCallback(res, error, stdout, stderr) {
    if (error) {
      res.json({
        success: false,
        error: stdout
      });
    } else {
      res.json({
        success: true
      });
    }
}

exports.fill = function(req, res) {
  exec('sudo ../../scripts/bathtub.py fill', function (error, stdout, stderr) {
    execCallback(res, error, stdout, stderr);
  });
};


exports.stop = function(req, res) {
  exec('sudo ../../scripts/bathtub.py stop', function (error, stdout, stderr) {
    execCallback(res, error, stdout, stderr);
  });
};