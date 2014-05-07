; var bmoon = bmoon || {};
bmoon.skeleton = {
    version: '1.0',

    init: function() {
        var o = bmoon.skeleton;
        if (o.inited) return o;

        o.e_content = $('#bd-content');
        
        o.inited = true;
        return o;
    },
    
    onready: function() {
        var o = bmoon.skeleton.init();

        o.bindClick();
    },

    bindClick: function() {
        var o = bmoon.skeleton.init();

    }
};

$(document).ready(bmoon.skeleton.onready);
