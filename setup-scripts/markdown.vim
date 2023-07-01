"cat >> ~/.vim/ftplugin/markdown.vim << 'EOF'
" markdown specific settings

" folding view saving specific stuff
" " Turn on autosaving of the view as I like code folding
let g:autosave_view = 0

" here we actually set this filetype to vim so that when the view loads the .md
" file below we don't have two md files but just the one and that one is the
" last one loaded which is the one from the view autocommand
"set filetype=vim

augroup AutoView
    autocmd!
    " Autosave & Load Views.
    autocmd BufWritePre,BufWinLeave * execute "mkview! " . expand('<afile>:p:h') . "/." . expand('<afile>:t') . ".view"
    autocmd BufWinEnter * execute "silent! source " . expand('%:p:h') . "/." . expand('%:t') . ".view"
"    autocmd BufWinEnter * execute "silent! source " . expand('%:p:h') . "/." . expand('%:t') . ".view" " set filetype=vim"
augroup END

" Required to let YCM work on markdown
let g:ycm_filetype_blacklist = {}

" Crucial to allow ycm and ultisnips to work together
" cant tab complete ultisnips if you don't do this
" free per https://vim.fandom.com/wiki/Unused_keys
let g:ycm_key_list_select_completion=['<C-N>']
let g:ycm_key_list_previous_completion=['<C-H>']

" Load YouCompleteMe
packadd YouCompleteMe

" Full Compilation switch
let full_compilation_mode = 0

" Saves as soon as you make the file so compilation works
autocmd BufNewFile * :write

" I turn on autosave on no activity in 500ms here for insert and non insert mode
" many may not like this but after turning on continuous compilation with \ll
" you get live updates without haveing to save manually in tex so im just putting this
" in the tex specific file configuration
autocmd CursorHold,CursorHoldI * update
set updatetime=500

inoremap <F1> <Esc>: silent exec '.!inkscape-figures markdown-create "'.getline('.').'" "./figures/"'<CR><CR>:w<CR>
nnoremap <F1> : silent exec '!inkscape-figures edit "./figures/" > /dev/null 2>&1 &'<CR><CR>:redraw!<CR>

inoremap <F2> <Esc>: silent exec '.!diagrams-net-figures markdown-create "'.getline('.').'" "./figures/"'<CR><CR>:w<CR>
nnoremap <F2> : silent exec '!diagrams-net-figures edit "./figures/" > /dev/null 2>&1 &'<CR><CR>:redraw!<CR>

" Enable pptx compilation from f3 keypress
nnoremap <F3> : call CompilePptx() <CR>

" Enable pptx preview from f4 keypress
nnoremap <F4> : call PreviewPptx() <CR>

" Hotkey to copy all snippets to local images folder
"nnoremap <F5> : silent exec '![[ $(ls -A ~/Snips/) ]] && cp ~/Snips/* ./images/ && rm ~/Snips/*' <CR><CR>:redraw!<CR>
nnoremap <F5> : silent exec '![[ $(ls -A ~/Snips/) ]] && mv ~/Snips/* ./images/' <CR><CR>:redraw!<CR>

" Enable docx compilation from f6 keypress
nnoremap <F6> : call CompileDocx() <CR>

" Enable docx preview from f7 keypress
nnoremap <F7> : call PreviewDocx() <CR>

" Add a key to toggle fast or full compile modes
nnoremap <F9> : call ToggleCompilationMode() <CR>

" Add key to reopen pdf viewer in the instance of a crash
nnoremap <F10> : silent exec '!zathura ' join([split(expand('%:p'),'/')[-1][0:-4],'.pdf'],'') ' &' <CR><CR>:redraw!<CR>

" Hotkey to make all figures
nnoremap <F11> : call ForceCompileAllFigures() <CR>

" Hotkey to make all plots
nnoremap <F12> : call ForceCompileAllPlots() <CR> 


" Autodetects if template is there in root or not and compiles with it if
" available
function CompilePptx()
   if filereadable("template.pptx")
      silent exec '!run_pandoc_commands.sh ' expand('%:p:h') ' -mdtpptx ' expand('%:r') ' > /dev/null 2>&1 &'
   else
      silent exec '!run_pandoc_commands.sh ' expand('%:p:h') ' -mdtpptxnt ' expand('%:r') ' > /dev/null 2>&1 &'
   endif
   :redraw!
endfunction

" Preview the pptx if it exists
function PreviewPptx()
   if filereadable(join([split(expand('%:p'),'/')[-1][0:-4],'.pptx'],''))
      silent exec '!libreoffice ' join([split(expand('%:p'),'/')[-1][0:-4],'.pptx'],'') ' &'
   endif
   :redraw!
endfunction

" Autodetects if template is there in root or not and compiles with it if
" available
function CompileDocx()
   if filereadable("template.docx")
      silent exec '!run_pandoc_commands.sh ' expand('%:p:h') ' -mdtdocxtl ' expand('%:r') ' > /dev/null 2>&1 &'
   else
      silent exec '!run_pandoc_commands.sh ' expand('%:p:h') ' -mdtdocxtlnt ' expand('%:r') ' > /dev/null 2>&1 &'
   endif
   :redraw!
endfunction

" Preview the pptx if it exists
function PreviewDocx()
   if filereadable(join([split(expand('%:p'),'/')[-1][0:-4],'.docx'],''))
      silent exec '!libreoffice ' join([split(expand('%:p'),'/')[-1][0:-4],'.docx'],'') ' &'
   endif
   :redraw!
endfunction

"This is required to get actual auto figure insertion
"along with the installation of the update_tex_figures.sh script
autocmd CursorHold * call RecompileMarkdown()
autocmd VimEnter * call StartupFunctions()

function RecompileMarkdown()
  silent exec '!update_tex_figures.sh ' expand('%:p:h') ' -m > /dev/null 2>&1 &'
  :redraw!
  silent exec '!make_gnuplots.sh ' expand('%:p:h') ' -m > /dev/null 2>&1 &'
  :redraw!
  if g:full_compilation_mode == 1
    silent exec '!run_pandoc_commands.sh ' expand('%:p:h') ' -mdttexwtfp ' expand('%:r') ' > /dev/null 2>&1 &'
    call CompilePptx()
    call CompileDocx()
  elseif g:full_compilation_mode == 0
    silent exec '!run_pandoc_commands.sh ' expand('%:p:h') ' -mdtpdf ' expand('%:r') ' > /dev/null 2>&1 &'
  elseif g:full_compilation_mode == 2
    silent exec '!run_pandoc_commands.sh ' expand('%:p:h') ' -mdtpdfwtfp ' expand('%:r') ' > /dev/null 2>&1 &'
  endif
  :redraw!
endfunction

function ForceCompileAllFigures()
  silent exec '!update_tex_figures.sh ' expand('%:p:h') ' -f > /dev/null 2>&1 &'
  :redraw!
endfunction

function ForceCompileAllPlots()
  silent exec '!make_gnuplots.sh ' expand('%:p:h') ' -f > /dev/null 2>&1 &'
  :redraw!
endfunction

function StartupFunctions()
  call RecompileMarkdown()
  silent exec '!zathura ' join([split(expand('%:p'),'/')[-1][0:-4],'.pdf'],'') ' &'
endfunction

function ToggleCompilationMode()
   if g:full_compilation_mode == 1
      let g:full_compilation_mode = 2
      echom 'Going to mermaid compilation mode'
   elseif g:full_compilation_mode == 2
      let g:full_compilation_mode = 0
      echom 'Going to fast compilation mode'
   elseif g:full_compilation_mode == 0
      let g:full_compilation_mode = 1
      echom 'Going to full compilation mode'
   endif
endfunction

" Activate this with K (shift-k)
"command -nargs=1 Googleit :!python3 ~/.vimfiles/Googleit.py <args>
"set keywordprg=:Googleit

"EOF
