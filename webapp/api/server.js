var express = require('express'),
    sqlite3 = require('sqlite3'),

    room = require('./controllers/room'),
    bathtub = require('./controllers/bathtub'),
    probe = require('./controllers/probe'),

    app = express();

app.post('/api/room/warmup', room.warmUp);
app.post('/api/room/cooldown', room.coolDown);
app.post('/api/room/stop', room.stop);

app.get('/api/bathtub/fill', bathtub.fill);
app.get('/api/bathtub/stop', bathtub.stop);

app.get('/api/probe/:name/last/:n', probe.getLastN);
app.get('/api/probe/:name/after/:timestamp', probe.getAfter);
app.post('/api/probe/:name/push/:value', probe.push);

app.listen(3000);