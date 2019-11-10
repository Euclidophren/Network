var express = require('express');

var app = express();

app.use(express.json());
app.use(express.urlencoded({ extended: true }));
app.use(express.static(__dirname + '/static'));
 
app.get('/', function (req, res) {
	var options = {
		root: __dirname + '/static',
		dotfiles: 'deny',
		headers: {
			'x-timestamp': Date.now(),
			'x-sent': true
		}
	};
	
	var fileName = 'html/index.html';
	res.sendFile(fileName, options, function (err) {
		if (err) {
		  console.log(err);
		  res.status(err.status).end();
		}
		else {
		  console.log('Sent:', fileName);
		}
	});
});

app.get('/png', function(req, res) {
	var options = {
		root: __dirname + '/static',
		dotfiles: 'deny',
		headers: {
			'x-timestamp': Date.now(),
			'x-sent': true
		}
	};
	
	var fileName = 'png/image.png';
	res.sendFile(fileName, options, function (err) {
		if (err) {
		  console.log(err);
		  res.status(err.status).end();
		}
		else {
		  console.log('Sent:', fileName);
		}
	});
});

app.get('/paragraph', function (req, res) {
	var options = {
		root: __dirname + '/static',
		dotfiles: 'deny',
		headers: {
			'x-timestamp': Date.now(),
			'x-sent': true
		}
	};
	
	var fileName = 'html/paragraph.html';
	res.sendFile(fileName, options, function (err) {
		if (err) {
		  console.log(err);
		  res.status(err.status).end();
		}
		else {
		  console.log('Sent:', fileName);
		}
	});
});

app.get('/example', function (req, res) {
	var options = {
		root: __dirname + '/static',
		dotfiles: 'deny',
		headers: {
			'x-timestamp': Date.now(),
			'x-sent': true
		}
	};
	
	var fileName = 'html/example.html';
	res.sendFile(fileName, options, function (err) {
		if (err) {
		  console.log(err);
		  res.status(err.status).end();
		}
		else {
		  console.log('Sent:', fileName);
		}
	});
});

app.use(function(req, res, next) {
	res.status(404).send('404 Not found!');
});

var server = app.listen(process.argv[2], function () {  
	var host = server.address().address;
	var port = server.address().port;
	console.log('Example app listening at http://%s:%s', host, port)
});
