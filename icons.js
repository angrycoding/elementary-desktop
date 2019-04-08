var OS = require('os');

// Home directory
var HOME_DIR = OS.homedir();
var INI = require('ini');
var FS = require('fs');
var Path = require('path');
var Async = require('async');

// List of possible icons paths
var ICON_PATHS = [
  '/usr/share/icons/',
  '/usr/local/share/icons/',
  '/usr/share/pixmaps/',
  '/usr/local/share/pixmaps/',
  Path.join(HOME_DIR, '.icons/'),
  Path.join(HOME_DIR, '.local/share/icons/'),
  Path.join(HOME_DIR, '.local/share/pixmaps/'),
  Path.join(HOME_DIR, '.pixmaps/')
];


var iconThemeName = 'elementary';


Async.each(ICON_PATHS, function(themeDir, ret) {
	var themeFile = Path.resolve(themeDir, `${iconThemeName}/index.theme`);
	FS.readFile(themeFile, 'UTF-8', function(error, theme) {
		if (error) return ret(false);
		theme = INI.parse(theme);
		console.info(theme);
		console.info(Object.keys(theme));
		console.info(themeFile)
	});
	// readINI(themeFile, function(error, result) {
		// ret(error ? false : result);
	// });
}, function(result) {

	// '/usr/share/icons/elementary/mimes/128'

	console.info(result);


});