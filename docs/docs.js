var _lb = null;
var _base_y = 0;
var _timeout = null;

function prepare_leftbar ()
{
	_lb = document.getElementById ( "leftbar" );
	
	gather_links ();

	_lb.style.position = "absolute";
	_base_y = _lb.offsetTop;
}

function window_scrolled ()
{
	if ( _timeout ) clearTimeout ( _timeout );
	_timeout = setTimeout ( move_leftbar, 200 );
}

function gather_links ()
{
	var links = document.getElementsByTagName ( 'a' );
	var l, t, lnk;
	var s = '';
	var linked = {};

	l = links.length;

	for ( t = 0; t < l; t ++ )
	{
		lnk = links [ t ];
		if ( ! lnk.title ) continue;
		if ( linked [ lnk.name ] ) continue;

		linked [ lnk.name ] = 1;

		s += '<a href="#' + lnk.name + '">' + lnk.title + '<\/a><br \/>';
	}

	_lb.innerHTML = s;
}

function move_leftbar ()
{
	if ( ! _lb ) return;

	var delta = 8;
	var v = Math.abs (  ( window.pageYOffset + _base_y ) - _lb.offsetTop );

	if ( v == 0 ) return;

	if ( v < 80 ) delta = 4;
	if ( v < 60 ) delta = 2;
	if ( v < 30 ) delta = 1;

	if ( window.pageYOffset + _base_y == _lb.offsetTop ) return;

	if ( window.pageYOffset + _base_y > _lb.offsetTop )
		_lb.style.top = ( _lb.offsetTop + delta ) + "px";
	else
		_lb.style.top = ( _lb.offsetTop - delta ) + "px";

	_timeout = setTimeout ( move_leftbar, 2 );
}

window.onscroll = window_scrolled;
