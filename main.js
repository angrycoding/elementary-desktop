var FS = require('fs'),
	Path = require('path'),
	Async = require('async'),
	Electron = require('electron'),
	isOSX = (process.platform === 'darwin'),
	{
		app,
		BrowserWindow,
		ipcMain,
		shell
	} = Electron;

if (!app.requestSingleInstanceLock()) return app.quit();


// var DESKTOP_DIR = '/home/ruslan/Qt';
var DESKTOP_DIR = app.getPath('home');
var THEME_DIR = process.argv[2] || '';
var FOLDER_ICON = Path.resolve(THEME_DIR, 'places/64/folder.svg');




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
					isDirectory: isDirectory
				});
				next();
			})



		}, function() {
			ret(null, resultFiles);
		});
	});
}

function createWindow(ret) {
	//setTimeout(createWindow, 300)
	app.on('ready', function() {

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
				defaultFontSize: 13,
				defaultFontFamily: {
					standard: 'Apple Garamond Regular'
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

	});
}


Async.parallel({
	files: readDesktop,
	mainWindow: createWindow
}, function(error, result) {
	result.mainWindow.webContents.send('renderDesktop', result.files);
});

ipcMain.on('openItem', function(event, fullPath) {
	shell.openItem(fullPath);
});