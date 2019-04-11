// var $ = #require('lib/jQuery.js'),
	// SocketIO = #require('lib/Socket.io.js'),
	// desktopIcon = #require('desktopIcon/desktopIcon.js');

(function() {

	var Electron = require('electron'),
		remote = Electron.remote,
		nativeImage = Electron.nativeImage,
		ipcRenderer = Electron.ipcRenderer,
		currentWindow = remote.getCurrentWindow(),
		webContents = currentWindow.webContents;



		var socket = io('http://127.0.0.1:9999');
		var documentRef = $(document);

		var iconSize = parseInt(getCSSVar('--icon-size'), 10);
		var GRID_SPACING = 10;


		function getCSSVar(varName) {
			return getComputedStyle(document.documentElement).getPropertyValue(varName);
		}

		function generate_random_string(string_length){
			let random_string = '';
			let random_ascii;
			for(let i = 0; i < string_length; i++) {
			random_ascii = Math.floor((Math.random() * 25) + 97);
			random_string += String.fromCharCode(random_ascii)
			}
			return random_string
		}

		var hash = function(s) {
		    /* Simple hash function. */
		    var a = 1, c = 0, h, o;
		    if (s) {
		        a = 0;
		        /*jshint plusplus:false bitwise:false*/
		        for (h = s.length - 1; h >= 0; h--) {
		            o = s.charCodeAt(h);
		            a = (a<<6&268435455) + o + (o<<14);
		            c = a & 266338304;
		            a = c!==0?a^c>>21:a;
		        }
		    }
		    return 'UID_' + String(a);
		};


		documentRef.on('dragover dragenter', event => event.preventDefault());


		documentRef.on('drop', function(event) {
			event = event.originalEvent;
			event.preventDefault();

			var existingIcon = $('.icon-selected');

			





			if (event.dataTransfer.files.length) {
				for (var c = 0; c < event.dataTransfer.files.length; c++) {
					var path = event.dataTransfer.files[c].path;
					var existingIcon = document.getElementById(hash(path));
					if (existingIcon) {

						$(existingIcon).css({
							left: Math.floor(event.pageX / iconSize) * iconSize,
							top: Math.floor(event.pageY / iconSize) * iconSize
						});

					} else {
						console.info('COPY???');
					}
				}
			}




			else if (existingIcon.length) {

				$(existingIcon).css({
					left: Math.floor(event.pageX / iconSize) * iconSize,
					top: Math.floor(event.pageY / iconSize) * iconSize
				});

			}



			return false;
		});


		documentRef.on('dragover', function(event) {
			event = event.originalEvent;
			event.dataTransfer.dropEffect = 'copy'
		})

		documentRef.on('dragstart', '.dragmebitch', function(event) {

			event.originalEvent.dataTransfer.effectAllowed = 'copy'

			var icons = $('.icon-selected');
			var paths = [];
			
			for (var c = 0; c < icons.length; c++) {
				paths.push($(icons[c]).data('fullpath'));
			}
			


			console.info(paths)
			// console.info(.length)

			// var fullPath = $(this).data('fullpath');


			currentWindow.capturePage({x: 0, y: 0, width: 0, height: 0}, function(image) {
				webContents.startDrag({files: paths, icon: image});
			});
		});




		socket.emit('renderDesktop', function(files) {

			document.body.innerHTML = '';

			var row = 0, col = 0;

			for (var c = 0; c < files.length; c++) {

				var icon = (Math.floor(Math.random() * 6) + 1) + '.png';
				var iconEl = createDesktopIcon(files[c].name, icon);
				iconEl.appendTo(document.body);

				var path = files[c].path;

				iconEl.attr('id', hash(path));


				iconEl.css({
					position: 'absolute',
					zIndex: 5000,
					top: row * (iconSize + GRID_SPACING),
					left: col * (iconSize + GRID_SPACING)
				});

				iconEl.data('fullpath', path)


				col ++;
				if (col > 3) {
					row++;
					col = 0;
				}

				// console.info(files[c].name)


			}


		});





		$(document).on('keydown', function(event) {


			// top
			if (event.which === 38) {

			}

			// right
			else if (event.which === 39) {
			}

			// bottom
			else if (event.which === 40) {

			}

			// left
			else if (event.which === 37) {
			}


		})


})();