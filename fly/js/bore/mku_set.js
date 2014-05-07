// ----------------------------------------------------------------------------
// markItUp!
// ----------------------------------------------------------------------------
// Copyright (C) 2008 Jay Salvat
// http://markitup.jaysalvat.com/
// ----------------------------------------------------------------------------
myMarkdownSettings = {
    nameSpace:          'markdown', // Useful to prevent multi-instances CSS conflict
    previewPosition: 'before',
    //previewInWindow: 'width=800, height=600, resizable=yes, scrollbars=yes',
    previewParserPath:  '/bore/preview',
    previewScroll: 'bottom',
    previewParserVar:   's',
    onShiftEnter:       {keepDefault:false, openWith:'\n\n'},
    markupSet: [
        {name:'First Level Heading', key:"1", placeHolder:'这里写标题内容...', closeWith:function(markItUp) { return miu.markdownTitle(markItUp, '=') } },
        {name:'Second Level Heading', key:"2", placeHolder:'这里写标题内容...', closeWith:function(markItUp) { return miu.markdownTitle(markItUp, '-') } },
        {name:'Heading 3', key:"3", openWith:'### ', placeHolder:'这里写标题内容...' },
        {name:'Heading 4', key:"4", openWith:'#### ', placeHolder:'这里写标题内容...' },
        {name:'Heading 5', key:"5", openWith:'##### ', placeHolder:'这里写标题内容...' },
        {name:'Heading 6', key:"6", openWith:'###### ', placeHolder:'这里写标题内容...' },
        {separator:'---------------' },        
        {name:'Bold', key:"B", openWith:'**', closeWith:'**'},
        {name:'Italic', key:"I", openWith:'_', closeWith:'_'},
        {separator:'---------------' },
        {name:'Bulleted List', openWith:'- ' },
        {name:'Numeric List', openWith:function(markItUp) {
            return markItUp.line+'. ';
        }},
        {separator:'---------------' },
        //{name:'Picture', key:"P", replaceWith:'![[![图片描述]!]]([![图片连接:!:http://]!] "[![图片标题]!]")'},
        {name:'Picture', key:"p", replaceWith:'![aaa](bbb "ccc")'},
        //{name:'Link', key:"L", openWith:'[', closeWith:']([![连接地址:!:http://]!] "[![连接标题]!]")', placeHolder:'这里写连接的说明文字...' },
        {name:'Link', key:"L", openWith:'[连接内容](http://', closeWith:' "说明文字")', placeHolder:'www.' },
        {separator:'---------------'},    
        {name:'Quotes', openWith:'> '},
        {name:'Code Block / Code', openWith:'(!(\t|!|`)!)', closeWith:'(!(`)!)'},
        {separator:'---------------'},
        {name:'Preview', call:'preview', className:"preview"}
    ]
}

// mIu nameSpace to avoid conflict.
miu = {
    markdownTitle: function(markItUp, char) {
        heading = '';
        n = $.trim(markItUp.selection||markItUp.placeHolder).length;
        for(i = 0; i < n; i++) {
            heading += char;
        }
        return '\n'+heading+'\n';
    }
}
