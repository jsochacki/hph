# Setup auto google
#cat >> $homedir/.cfiles/Googleit.py << 'EOF'
import os, sys, subprocess

subprocess.Popen(['google-chrome-stable', 'http://www.google.com/search?q='+ 
                 ' '.join(sys.argv[1:]), ' &'])

#EOF
