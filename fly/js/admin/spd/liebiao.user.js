// ==UserScript==
// @name          liebiao spider
// @namespace     http://www.imdida.org/
// @description   get liebiao's pinche imformation
// @include       http://*.liebiao.com/pinche/*
// @exclude       http://diveintogreasemonkey.org/*
// @exclude       http://www.diveintogreasemonkey.org/*
// ==/UserScript==

var e = document.createElement("script");

e.src = 'http://www.imdida.org/js/b/admin/spd/liebiao.js';
e.type="text/javascript";
document.getElementsByTagName("head")[0].appendChild(e);
