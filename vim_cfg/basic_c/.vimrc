" 设置文字编码
set fenc=utf-8
set fencs=utf-8,gb18030,gbk,gb2312
" 打开行号
set number
" 关闭兼容老版本的vim，如果兼容老版本vim，则方向键不能使用，所以最好关闭
set nocompatible
" 语法着色打开
syntax on

" 打开自动缩进
set autoindent
set smartindent
set cindent
" 缩进宽度
set tabstop=4
set softtabstop=4
set shiftwidth=4
"建议开启expandtab选项，会自动将tab扩展很空格，代码缩进会更美观
set expandtab

" 输入括号引号等符号时显示配对的那一半
set showmatch
" 让输入的命令显示出来。
set showcmd
" 显示vim当前处于的模式
set showmode
" 增量搜索，就是搜索时不需要回车就开始进行查找
set incsearch

" 最后这个分号不能少。有特殊意义。
set tags=tags;
set autochdir


let Tlist_Show_One_File=1
let Tlist_Exit_OnlyWindow=1
let Tlist_Ctags_Cmd="/usr/bin/ctags"
nnoremap <silent><F4> :TlistToggle<CR>

set tabstop=4
set mouse=a
set noswapfile
set completeopt=menu,menuone  
let OmniCpp_MayCompleteDot=1    "  打开  . 操作符
let OmniCpp_MayCompleteArrow=1  "打开 -> 操作符
let OmniCpp_MayCompleteScope=1  "打开 :: 操作符
let OmniCpp_NamespaceSearch=1   "打开命名空间
let OmniCpp_GlobalScopeSearch=1  
let OmniCpp_DefaultNamespace=["std"]  
let OmniCpp_ShowPrototypeInAbbr=1  "打开显示函数原型
let OmniCpp_SelectFirstItem = 2"自动弹出时自动跳至第一个

set tags+=~/.vim/systags
filetype plugin on

