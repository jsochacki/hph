"cat >> ~/.vim/ftplugin/c.vim << 'EOF'
" C/C++ specific settings

" folding view saving specific stuff
" " Turn on autosaving of the view as I like code folding
let g:autosave_view = 0

augroup AutoView
    autocmd!
    " Autosave & Load Views.
    autocmd BufWritePre,BufWinLeave * execute "mkview! " . expand('<afile>:p:h') . "/." . expand('<afile>:t') . ".view"
    autocmd BufWinEnter * execute "silent! source " . expand('%:p:h') . "/." . expand('%:t') . ".view"
augroup END

" Crucial to allow ycm and ultisnips to work together
" cant tab complete ultisnips if you don't do this
" free per https://vim.fandom.com/wiki/Unused_keys
let g:ycm_key_list_select_completion=['<C-N>']
let g:ycm_key_list_previous_completion=['<C-H>']

" Ensure automatic semantic completion
let g:ycm_disable_signature_help = 0
let g:ycm_min_num_of_chars_for_completion = 1
let g:ycm_semantic_triggers =  {
  \   'c': ['(','->', '.'],
  \   'cpp,cuda,objcpp': ['(','->', '.', '::']
  \ }
let g:ycm_auto_trigger = 1

" intelligent comments
" Cant use these as they insert extra * when entering math expressions and using
" clang to reformat, all of a sudden you will have all sorts of extra * so do
" not try to use even though they are very nice
" set comments=sl:/*,mb:\ *,elx:\ */

" Load YouCompleteMe
packadd YouCompleteMe

" Clangd Setup
" Let clangd fully control code completion
let g:ycm_clangd_uses_ycmd_caching = 0
" Use installed clangd, not YCM-bundled clangd which doesn't get updates.
let g:ycm_clangd_binary_path = exepath("clangd")
let g:ycm_clangd_args = ['-log=verbose', '-pretty']
" Set path to global conf file
let g:ycm_global_ycm_extra_conf = '~/.cfiles/.ycm_extra_conf.py'

" Add mappings for YCM
" S is a synonim for s and s is a synonim for cl
" but we do use s from time to time so use x as that is a synonim for dl
nnoremap S :YcmCompleter GoTo<CR>
nnoremap <C-J> :YcmCompleter GetType<CR>
nnoremap _ :YcmCompleter RefactorRename 
nnoremap Y :YcmCompleter GetDoc<CR>

" Activate this with K (shift-k)
command -nargs=1 Googleit :!python3 ~/.cfiles/Googleit.py <args>
set keywordprg=:Googleit

" Go with the less obtrusive option just so that you don't have any catastrophic
" file write errors with auto formatting
nnoremap <C-K> :py3f ~/.local/bin/clang-format.py<CR>
"autocmd CursorHold * :py3f ~/.local/bin/clang-format.py
"autocmd BufWrite * :py3f ~/.local/bin/clang-format.py


" Saves as soon as you make the file so compilation works
autocmd BufNewFile * :write

" I turn on autosave on no activity in 500ms here for insert and non insert mode
" many may not like this but after turning on continuous compilation with \ll
" you get live updates without haveing to save manually in tex so im just putting this
" in the tex specific file configuration
autocmd CursorHold,CursorHoldI * update
set updatetime=500

"CURRENTLY I NO LONGER PLAN TO DO THIS BUT RATHER TO JUST RUN A SCRIPT FROM THE
"ROOT CODE DIR AS THAT IS EASIER TO SUPPORT AND VERY EASY TO DO
" Add key to build the docs
"nnoremap <F3> : call BuildTheDocs() <CR>

" Add key to view the docs
"nnoremap <F4> : call ViewThePdfDocs() <CR>
"nnoremap <F5> : call ViewTheHtmlDocs() <CR>


" Force Build Full Docs
"function BuildTheDocs()
"   silent exec '!run_doxygen_commands.sh ' expand('%:p:h') ' -fbf > /dev/null 2>&1 &'
"   :redraw!
"endfunction

" Preview the pdf if it exists
"function ViewThePdfDocs()
"   silent exec '!run_doxygen_commands.sh ' expand('%:p:h') ' -vtpdfd > /dev/null 2>&1 &'
"   :redraw!
"endfunction

" Preview the html documentation if it exists
"function ViewTheHtmlDocs()
"   silent exec '!run_doxygen_commands.sh ' expand('%:p:h') ' -vthtmld > /dev/null 2>&1 &'
"   :redraw!
"endfunction

"EOF
