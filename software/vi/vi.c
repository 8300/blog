//--------------------- vi.c --------------------------
//---------------------vi
vi ~/.vimrc
{
set nu
set ts=2
set hlsearch
set backspace=2
set autoindent
syntax on
//cut
dd -> p
v j/k/h/l -> d p
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
gg->dG //clear all
u //undo
9,15 co 17 //copy
<x>yy //copy
p //paste
}


:vimgrep /classA/gj **/*.*
:[find range]s[target]/[replace]/g	eg:	:%s/foo/bar/g
//two windows
:sp	:vsp
ctrl + w -> w//switch windows
yy -> p
//batch indentation
way1:
[linenumberbegin],[linenumberend]>
[linenumberbegin],[linenumberend]<
way2:
shift + v ->  down
shift + > / shift + <
//file browse
:Sex -> i
//history
q:
q/
//
//--------------------- end ---------------------------
