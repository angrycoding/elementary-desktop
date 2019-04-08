var Electron = require('electron'),
	remote = Electron.remote,
	ipcRenderer = Electron.ipcRenderer,
	isOSX = (process.platform === 'darwin');

var win = remote.getCurrentWindow()
var nodeConsole = require('console');
console = new nodeConsole.Console(process.stdout, process.stderr);

var documentRef = $(document);

ipcRenderer.on('renderDesktop', function(event, files) {

	var wrapper = $('.wrapper');

	for (var c = 0; c < files.length; c++) {
		addIcon(files[c].name, files[c].icon)
		.data('path', files[c].path)
		.appendTo(wrapper)
	}


	if (!isOSX) {
		var f = window.location.search.split(/[?&=]/g);
		document.body.style.width = f[2] + 'px';
		document.body.style.height = f[4] + 'px';
	}

	(isOSX ? win.show() : win.showInactive());
	if (!isOSX) setTimeout(() => win.setResizable(false), 500);

});


documentRef.on('click', '.quitButton', function() {
	win.close();
});

documentRef.on('dblclick', '.icon', function() {
	ipcRenderer.send('openItem', $(this).data('path'));
});


