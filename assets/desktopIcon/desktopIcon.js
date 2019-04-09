(function() {
	
	var ctx = document.createElement('canvas').getContext('2d');
	var fontSize = parseInt(getCSSVar('--icon-font-size'), 10);
	ctx.font = [getCSSVar('--icon-font-size'), getCSSVar('--icon-font-family')].join(' ');
	var borderLeftRight = parseInt(getCSSVar('--border-size'), 10) * 4;
	var maxTextWidth = parseInt(getCSSVar('--icon-size'), 10) - parseInt(getCSSVar('--padding-left-right'), 10) * 2;

	function getCSSVar(varName) {
		return getComputedStyle(document.documentElement).getPropertyValue(varName);
	}

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

	function fitText(text) {
		
		var result = splitText(text, maxTextWidth);
		var firstLine = result[0];
		var secondLine = result[1];
		if (!secondLine.length) return [firstLine, ''];

		var firstLineWidth = ctx.measureText(firstLine).width;
		var secondLineWidth = ctx.measureText(secondLine).width;

		if (secondLineWidth + borderLeftRight <= firstLineWidth)
			return [firstLine, secondLine];

		if (secondLineWidth <= firstLineWidth)
			return [firstLine, '', secondLine];

		var xStr = '...' + secondLine.slice(-6);
		var xWidth = ctx.measureText(xStr).width;


		return [firstLine, '', splitText(secondLine.slice(0, -6), firstLineWidth - xWidth)[0] + xStr];
	}

	function addIcon(text, iconUrl) {
		var splittedText = fitText(text);

		var el = $(`
			<div class="icon" _draggable="true" data-text="${text}">
				<div class="icon-image">
					<img src="data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAAEAAAABCAQAAAC1HAwCAAAAC0lEQVR42mNkYAAAAAYAAjCB0C8AAAAASUVORK5CYII="
					style="background-image: url('${iconUrl}');" />
				</div>
				<div class="icon-label">

					<div class="icon-lines">
						<div class="icon-line1">
							<div class="icon-firstLine"></div>
							<div class="icon-thirdLine"></div>
						</div>
						<div class="icon-line2">
							<div class="icon-secondLine"></div>
							<div class="icon-borderSegment icon-borderSegmentLeft"></div>
							<div class="icon-borderSegment icon-borderSegmentRight"></div>
						</div>
					</div>
				</div>
			</div>
		`);

		$('.icon-firstLine', el).html(splittedText[0]);
		$('.icon-secondLine', el).html(splittedText[1]);
		$('.icon-thirdLine', el).html(splittedText[2]);

		return el;
	}

	$(window).on('focus', function() {
		$(document.documentElement).removeClass('inactive');
	});

	$(window).on('blur', function() {
		$(document.documentElement).addClass('inactive');
	});

	var iel;



	function deselectAll() {
		$('.icon-selected')
		.removeClass('icon-selected')
		.appendTo(document.body);
	}

	$(document).on('mousedown', function(event) {

		var target = $(event.target);
		var icon = target.closest('.icon');
		var shiftKeyDown = event.shiftKey;

		if (!icon.length) {
			deselectAll();
		}

		else if (!icon.is('.icon-selected')) {

			if (!iel) {
				iel = $('<div />')
				.addClass('dragmebitch')
				.attr('draggable', true)
				.css({
					position: 'absolute',
					top: 0,
					left: 0,
					right: 0,
					bottom: 0,
					zIndex: 4000,
					// backgroundColor: 'gray'
					// pointerEvents: 'none'
				}).appendTo(document.body)
			}

			icon.appendTo(iel);

			if (!shiftKeyDown) deselectAll();

			icon.addClass('icon-selected')


		}


		else if (shiftKeyDown) {
			icon.removeClass('icon-selected')
			.appendTo(document.body);
		}

	});



	window.createDesktopIcon = addIcon;

})();