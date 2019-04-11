var server = require('http').createServer();
var io = require('socket.io')(server);
var FS = require('fs');
var Async = require('async');
var Path = require('path');

var DESKTOP_DIR = '/Users/ruslanmatveev/';

io.on('connection', function(socket) {


	FS.readdir(DESKTOP_DIR, function(error, files) {
		files = files.filter(file => file[0] !== '.');

		var response = [];
		Async.each(files, function(fileName, next) {
			var filePath = Path.resolve(DESKTOP_DIR, fileName);
			response.push({name: fileName, path: filePath});
			next();
		}, function() {
			socket.emit('renderDesktop', response);
		});

	});
	

});

server.listen(9999);