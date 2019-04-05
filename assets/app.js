var Electron = require('electron'),
	remote = Electron.remote,
	ipcRenderer = Electron.ipcRenderer,
	isOSX = (process.platform === 'darwin');

var win = remote.getCurrentWindow()
var nodeConsole = require('console');
console = myConsole = new nodeConsole.Console(process.stdout, process.stderr);

// alert(document.body.style.width)

var documentRef = $(document);




documentRef.on('mousedown', function(event) {

	if (event.which !== 1) return;

	var canvas = $('#canvas');
	var sx = event.pageX, sy = event.pageY;


	documentRef.on('mousemove.something', function(event) {

		var x = Math.min(event.pageX, sx),
			y = Math.min(event.pageY, sy),
			w = Math.abs(event.pageX - sx),
			h = Math.abs(event.pageY - sy);



		canvas.show().css({
			left: x,
			top: y,
			width: w,
			height: h
		})

	// 	console.info(event.pageX, event.pageY)

	// 	ctx.beginPath();
	// 	ctx.lineWidth = 1;
	// 	ctx.strokeStyle = "red";
	// 	ctx.rect(5, 5, 290, 140); 
	// 	ctx.stroke();
	// });

		return false;
	});

	documentRef.one('mouseup.something', function() {
		canvas.hide();
		documentRef.off('.something');

	});

});

ipcRenderer.on('renderDesktop', function(event, files) {

	var wrapper = $('.wrapper');

	//wrapper.html(JSON.stringify(files));

	for (var c = 0; c < files.length; c++) {
		

		
		var el = $('<div/>')

		.addClass('icon')

		.data('path', files[c].path)

		.attr('draggable', true)

		.css({
		})

		.html(`
			<img src="${files[c].icon}" />
			<div>${files[c].name}</div>
		`)

		.appendTo(wrapper);
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



// documentRef.on('dragenter', function(event) {
	
// 	var target = $(event.target);
	
// 	if (!target.is('.icon')) {
// 		target = target.closest('.icon');
// 	}

// 	if (target.length) {
// 		target.css('background-color', 'red');
// 	}
// });

// documentRef.on('dragleave', function(event) {
// 	$( event.target).css('background-color', '');
// });


var currentTarget;


documentRef.on('dragover', function(event) {

	if (currentTarget) {
		currentTarget.css('background-color', '');
		console.info(1);
	}
	
	event = event.originalEvent;
	event.dataTransfer.dropEffect = 'none';
	
	var target = $(event.target);
	
	if (!target.is('.icon')) {
		target = target.closest('.icon');
	}

	if (target.length) {
		currentTarget = target;
		event.dataTransfer.dropEffect = 'copy';
		target.css('background-color', 'red');
	}


	event.preventDefault();
	event.stopPropagation();
	return false;



	// myConsole.info(event.target.className);

	// event.dataTransfer.dropEffect = 'move'

	// myConsole.info(
	// 	event.dataTransfer.files.length
	// );


	// event.preventDefault();
});

documentRef.on('drop', '.icon', function(event) {
	event = event.originalEvent;
	myConsole.info(
		event.dataTransfer.files[0].path,
		'->',
		$(this).data('path')
	);
});


documentRef.on('dragcancel', function() {
	myConsole.info('aa');
	// if (currentTarget) {
	// 	currentTarget.css('background-color', '');
	// 	currentTarget = null;
	// }
})


documentRef.on('dragend', function() {
	myConsole.info('x');
	// if (currentTarget) {
	// 	currentTarget.css('background-color', '');
	// 	currentTarget = null;
	// }
});