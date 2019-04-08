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
			<div class="icon" draggable="true" data-text="${text}">
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

	$(document).on('mousedown', '.icon:not(.icon-selected)', function() {
		$('.icon-selected').removeClass('icon-selected');
		$(this).addClass('icon-selected')
	});

	window.addIcon = addIcon;

})();