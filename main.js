var OS = require('os'),
    FS = require('fs'),
	Path = require('path'),
	Async = require('async'),
	Shell = require('shelljs'),
	Electron = require('electron'),
	Mime = require('mime'),
	myEmitter = new (require('events'))(),
	isOSX = (process.platform === 'darwin'),
	{
		app,
		BrowserWindow,
		ipcMain,
		shell
	} = Electron;

if (!app.requestSingleInstanceLock()) return app.quit();


function getFiles(ret) {
	FS.readdir(DESKTOP_DIR, function(error, files) {
		files = files.filter(file => file[0] !== '.');

		var response = [];
		Async.each(files, function(fileName, next) {
			var filePath = Path.resolve(DESKTOP_DIR, fileName);
			response.push({name: fileName, path: filePath});
			next();
		}, function() {
			ret(response);
		});

	});
}

var server = require('http').createServer();
var io = require('socket.io')(server);
io.on('connection', function(socket) {

	socket.on('renderDesktop', function(ret) {
		getFiles(ret);
	});

	// getFiles(function(files) {
	// 	socket.emit('renderDesktop', files);
	// });



	

});

server.listen(9999);



ipcMain.on('ondragstart', function(event, filePath) {
	console.info(arguments);


	// mainWindow.capturePage([rect, ]callback)

  // event.sender.startDrag({
  //   file: filePath,
  //   icon: '/Users/ruslanmatveev/Desktop/68747470733a2f2f637332322e6261627973666572612e72752f622f392f362f622f37343433323039302e3137333639353631312e6a706567.jpg'
  // })
});


// var DESKTOP_DIR = '/home/ruslan/Qt';
var DESKTOP_DIR = app.getPath('home');
var THEME_DIR = process.argv[2] || '';
var FOLDER_ICON = Path.resolve(THEME_DIR, 'places/64/folder.svg');

var GSettings = {};


function getPathIcon(path, ret) {
	var isDirectory = FS.lstatSync(path).isDirectory();
	if (isDirectory) return ret(FOLDER_ICON, true);
	app.getFileIcon(path, {size: 'large'}, function(error, icon) {
		ret(icon.toDataURL(), false);
	});
}

function readDesktop(ret) {

	var resultFiles = [];
	FS.readdir(DESKTOP_DIR, function(error, files) {
		Async.each(files, function(fileName, next) {

			if (fileName[0] === '.') return next();
			var filePath = Path.resolve(DESKTOP_DIR, fileName);


			getPathIcon(filePath, function(icon, isDirectory) {
				resultFiles.push({
					icon: icon,
					path: filePath,
					name: fileName,
					mime: Mime.getType(filePath),
					isDirectory: isDirectory
				});
				next();
			})



		}, function() {
			// console.info(resultFiles)
			ret(null, resultFiles);
		});
	});
}

function createWindow(ret) {
	app.on('ready', function() {

	setTimeout(function() {
			var { width, height } = Electron.screen.getPrimaryDisplay().bounds
			var foo = Electron.screen.getPrimaryDisplay().workArea.y;

			var mainWindow = new BrowserWindow(Object.assign({
				x: 0,
				y: 30,
				
				title: 'xxx-yyy-zzz-foo-bar',
				type: 'desktop',
				show: false,
				frame: false,
				transparent : true,
				skipTaskbar: true,

				webPreferences: {
					webgl: false,
					webaudio: false,
					plugins: false,
					defaultFontSize: 12,
					defaultMonospaceFontSize: 12,
					defaultFontFamily: {
						monospace: 'Open Sans'
					},
					additionalArguments: ' --enable-transparent-visuals --disable-gpu'
				}
			}, isOSX && {
				type: 'normal',
				frame: true,
				transparent: false,
				skipTaskbar: false
			}));

			mainWindow.loadURL(`file://${__dirname}/assets/index.html?width=${width}&height=${height-30}`);

			mainWindow.webContents.on('did-finish-load', function() {
				ret(null, mainWindow);
			});
	}, 500);

	});
}



/*
ipcMain.on('openItem', function(event, fullPath) {
	shell.openItem(fullPath);
});

Async.parallel({
	files: readDesktop,
	mainWindow: createWindow
}, function(error, result) {
	
	result.mainWindow.webContents.send('renderDesktop', result.files);
});
*/


app.on('ready', function() {

	var mainWindow = new BrowserWindow({
		x: 0,
		y: 30,
		show: true,
	});

	mainWindow.loadURL(`file://${__dirname}/assets/test.html`);

});

// mainWindow.webContents.on('did-finish-load', function() {
	// ret(null, mainWindow);
// });

/*
myEmitter.once('gSettingsUpdated', function() {

	Async.parallel({
		files: readDesktop,
		mainWindow: createWindow
	}, function(error, result) {
		
		result.mainWindow.webContents.send('renderDesktop', result.files);
	});

});


Shell.exec(
	'gsettings list-recursively org.gnome.desktop.interface & gsettings monitor org.gnome.desktop.interface',
	{async: true, silent: true}
).stdout.on('data', function(lines) {
	lines = lines.split(/\n+/g);
	while (lines.length) {
		var line = lines.shift();
		line = line.split('org.gnome.desktop.interface');
		line = line.pop().trim();
		line = line.split(/\s/g);
		var key = line.shift().replace(/\:/g, '').trim();
		var value = line.join(' ').trim();
		if (value[0] === "'" && value[value.length - 1] === "'")
			value = value.slice(1, -1);
		if (!key) continue;


		GSettings[key] = value;
	}
	myEmitter.emit('gSettingsUpdated');
});

*/