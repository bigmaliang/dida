// ==UserScript==
// @name          edeng spider
// @namespace     http://www.imdida.org/
// @description   get edeng's pinche imformation
// @include       http://*.edeng.cn/13/pinche/*
// @include       http://*.edeng.cn/jiedaoxinxi/*
// @include       http://*.edeng.cn/xinxi/*
// @exclude       http://diveintogreasemonkey.org/*
// @exclude       http://www.diveintogreasemonkey.org/*
// ==/UserScript==

var e = document.createElement("script");

e.src = 'http://www.imdida.org/js/b/admin/spd/edeng.js';
e.type="text/javascript";
document.getElementsByTagName("head")[0].appendChild(e);
