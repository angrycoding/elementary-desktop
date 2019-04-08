var Electron = require('electron'),
	remote = Electron.remote,
	ipcRenderer = Electron.ipcRenderer,
	isOSX = (process.platform === 'darwin');

var win = remote.getCurrentWindow()
var nodeConsole = require('console');
console = new nodeConsole.Console(process.stdout, process.stderr);

var documentRef = $(document);



documentRef.on('mousedown', '.icon:not(.selected)', function(event) {
	$('.icon.selected').removeClass('selected');
	$(this).addClass('selected');
});

win.on('focus', function() {
	$(document.documentElement).addClass('focused');
});

win.on('blur', function() {
	$(document.documentElement).removeClass('focused');
});


/*
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
*/


var c = document.createElement('canvas');
var ctx = c.getContext("2d");
ctx.font = (
	window
	.getComputedStyle(document.documentElement, null)
	.fontSize + ' monospace'
);


function splitText(text, maxWidth) {
	var firstLine = '';
	text = text.split('');
	while (text.length) {
		var size = ctx.measureText(firstLine + text[0]).width;
		if (size > maxWidth) break;
		firstLine += text.shift();
	}
	return [firstLine, text.join('')];
}

function fitText(text, maxWidth) {
	
	var result = splitText(text, maxWidth);
	var firstLine = result[0];
	var secondLine = result[1];
	if (!secondLine.length) return [firstLine, ''];

	var firstLineSize = ctx.measureText(firstLine).width;
	var secondLineSize = ctx.measureText(secondLine).width;

	
	if (secondLineSize <= firstLineSize / 100 * 90)
		return [firstLine, secondLine];

	if (secondLineSize <= firstLineSize)
		return [firstLine, '', secondLine];

	var rest = '...' + secondLine.slice(-5);
	var restSize = ctx.measureText(rest).width;

	rest = splitText(secondLine, firstLineSize - restSize)[0] + rest;

	return [firstLine, '', rest]
}




function makeIcon(fileName, icon, target, fullPath) {

	fileName = fitText(fileName, 100);

	var item = $(`

		<div class="icon" draggable="true" data-path="${fullPath}">

			<img src="${icon}" />

			<div class="line firstLine">
				<div class="firstLineContent">${fileName[0]}</div>
				<div class="thirdLineContent">${fileName[2] || ''}</div>
			</div>
			<div class="secondLineWrapper">
				<div  class="line secondLineContent">${fileName[1] || ''}</div>
				<div class="borderSegment left"></div>
				<div class="borderSegment right"></div>
			</div>
		</div>

	`).appendTo(target)
	

}

document.addEventListener('dragstart', function (evt) {
	evt.stopPropagation();
	// evt.preventDefault();
  	return false;
});


ipcRenderer.on('renderDesktop', function(event, files) {

	var wrapper = $('.wrapper');

	for (var c = 0; c < files.length; c++) {
		makeIcon(files[c].name, files[c].icon, wrapper, files[c].path)
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

documentRef.on('_dragstart', '.icon', function(event) {
	// event = event.originalEvent;
	// event.dataTransfer.setDragImage($('img', this)[0], 0, 0);
})


documentRef.on('_dragover', function(event) {

	// if (currentTarget) {
	// 	// currentTarget.css('background-color', '');
	// 	console.info(1);
	// }
	
	event = event.originalEvent;
	event.dataTransfer.dropEffect = 'none';
	
	var target = $(event.target);
	
	// if (!target.is('.icon')) {
		// target = target.closest('.icon');
	// }

	if (target.length) {
		currentTarget = target;
		event.dataTransfer.dropEffect = 'copy';
		// target.css('background-color', 'red');
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

documentRef.on('_drop', '.icon', function(event) {
	event = event.originalEvent;
	myConsole.info(
		event.dataTransfer.files[0].path,
		'->',
		$(this).data('path')
	);
});


documentRef.on('_dragcancel', function() {
	myConsole.info('aa');
	// if (currentTarget) {
	// 	currentTarget.css('background-color', '');
	// 	currentTarget = null;
	// }
})


documentRef.on('_dragend', function() {
	myConsole.info('x');
	// if (currentTarget) {
	// 	currentTarget.css('background-color', '');
	// 	currentTarget = null;
	// }
});
