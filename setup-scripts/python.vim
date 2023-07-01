"cat >> ~/.vim/ftplugin/python.vim << 'EOF'
" Python3 specific settings

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
  \   'python': ['(','->', '.', '::']
  \ }
let g:ycm_auto_trigger = 1

" intelligent comments (cant figure out right now)
" set comments=sl:\"\"\",mb:\ ,elx:\ \"\"\"

" Load YouCompleteMe
packadd YouCompleteMe

" ycm python Setup
let g:ycm_python_interpreter_path = ''
let g:ycm_python_sys_path = []
let g:ycm_extra_conf_vim_data = [
  \  'g:ycm_python_interpreter_path',
  \  'g:ycm_python_sys_path'
  \]
" Set path to global conf file
let g:ycm_global_ycm_extra_conf = '~/.pyfiles/.ycm_extra_conf.py'

" Add mappings for YCM
" S is a synonim for s and s is a synonim for cl
" but we do use s from time to time so use x as that is a synonim for dl
nnoremap S :YcmCompleter GoTo<CR>
nnoremap <C-J> :YcmCompleter GetType<CR>
nnoremap _ :YcmCompleter RefactorRename 
nnoremap Y :YcmCompleter GetDoc<CR>

" Activate this with K (shift-k)
command -nargs=1 Googleit :!python3 ~/.pyfiles/Googleit.py <args>
set keywordprg=:Googleit

"EOF
