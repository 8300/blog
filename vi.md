
<!DOCTYPE html>
<head>
    <meta charset="UTF-8">
</head>
<h1>gcc_note.html</h1>
<p style="white-space:pre-wrap">

#	vi-8300.github.io

## 常用命令
:vimgrep /classA/gj **/*.*
:[find range]s[target]/[replace]/g	eg:	:%s/foo/bar/g
//two windows
:sp	:vsp
ctrl + w -&gt//switch windows
yy -&gt
//batch indentation
way1:
[linenumberbegin],[linenumberend]&gtg:	:%s/foo/bar/g
[linenumberbegin],[linenumberend]&ltg:	:%s/foo/bar/g<br&gtr>
way2:
shift + v -&gtdown
shift + &gt shift + &lt//file browse
:Sex -&gt
//history
q:
q/

## 进阶命令

## 永久设置
vi ~/.vimrc
{
	set nu
	set ts=2
	set hlsearch
	set backspace=2
	set autoindent
	syntax on
	//cut
	dd -&gt
	v j/k/h/l -&gt p
	:n	// jump to n line
	set nu	//show line number
	ctrl d	//scroll down half a page
	ctrl u	//scroll up half a page
	u	//undo
	ctrl r	//to resume the cancellation
	ctrl 	insert	//copy
	shift	insert	//paste
	zz 让光标所杂的行居屏幕中央
	zt 让光标所杂的行居屏幕最上一行 t=top
	zb 让光标所杂的行居屏幕最下一行 b=bottom
	   //~/.bashrc the vim configuration file permanent
	gg-&gt //clear all
	u //undo
	9,15 co 17 //copy
	&ltyy //copy
	p //paste
}


//--------------------- end ---------------------------
</p>
