# dotfiles
My Dotfiles :)
run ./setup_ubuntu20p04_server.sh

That will get you everything for the vmware server version.
The only thing to you need to do to have this work is be running in vmware and have git as your folder name for your shared folder.

Note that the ./install_and_setup_texlive.sh portion takes a very long time (30
      minutes or so) so plan for that.

## Notes on using vim markdown

- Make sure that all of the letters in your latex text in your drawio images
fit within the box they are bound within or else the latex will have ... in the 
text field and it and will not compile
- In addition the rendered latex will not wrap due to the lack svg 1.1 support
perhaps or just due to the way that tex renders but either way you need to have
excess width on each side of the tex so that it renders fully and does not
render to myequat...
   - It will render to the center of the box though and will not clash or
   overshadow the other text it overlaps in the program
