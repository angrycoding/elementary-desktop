var electron = require('electron');
const {
	app,
	BrowserWindow,
	ipcMain,
	shell
} = electron;

var FS = require('fs'),
	Path = require('path'),
	Async = require('async');

var DESKTOP_DIR = '/home/ruslan/Qt';
// var DESKTOP_DIR = app.getPath('home');
var THEME_DIR = process.argv[2];
var FOLDER_ICON = Path.resolve(THEME_DIR, 'places/64/folder.svg');


if (!app.requestSingleInstanceLock()) return app.quit();


function createWindow () {

	 var { width, height } = electron.screen.getPrimaryDisplay().bounds
	 var foo = electron.screen.getPrimaryDisplay().workArea.y;
	 console.info()

	var mainWindow = new BrowserWindow({
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
	});


	mainWindow.loadURL(`file://${__dirname}/index.html?width=${width}&height=${height-30}`)
    
    ipcMain.on('openItem', function(event, fullPath) {
    	shell.openItem(fullPath);
	});

	function getPathIcon(path, ret) {
		var isDirectory = FS.lstatSync(path).isDirectory();
		if (isDirectory) return ret(FOLDER_ICON, true);
		app.getFileIcon(path, {size: 'large'}, function(error, icon) {
			ret(icon.toDataURL(), false);
		});

	}


    mainWindow.webContents.on('did-finish-load', function() {
    	FS.readdir(DESKTOP_DIR, function(error, files) {
    		var resultFiles = [];
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
    			mainWindow.webContents.send('renderDesktop', resultFiles);
    		});
    	});
        


    });

}

app.on('ready', () => setTimeout(createWindow, 300));