RemaPSP v1~
Coded by danzel (danzelatlocalhostdotgeekdotnz), Using NID overloading thanks to MPHs example :))))

Tested by Lordsturm and macca. THANKS GUYS!
Launcher image by Haeal and Ayb4btu. THANKS GUYS TOO!

RemaPSP is a program to remap the PSP controls. It also supports turbo (Rapid Fire!) and Macros.
It is a 3rd party playstation controller you can play with, outside.

############# Install Instructions #############

  ########### For 1.5 Homebrew ###########
   copy remaps and psp folder to memory stick.
   run remapspboot from the game menu.

  ######### For use with DevHook #########
   Copy dh folder to memory stick.
   Edit dh/271/flash0/kd/pspbtcnf_game.txt to load the prxs by adding the lines: (Replace 271 with the firmware version you are emulating)
    ms0:/dh/kd/nkthread.prx
    ms0:/dh/kd/remapsp.prx
   after the line "ms0:/dh/kd/umdciso.prx".
   Put the remaps dir straight on your memstick.

These can both be installed at the same time.
I have also included a compiled version of the controller test from the pspsdk for testing your remap files.

################# And then/Usage ##################
Fire up devhook and run a game or use remapspboot and chose a homebrew from the list.
While in a game press Volume+ and Volume- at the same time to fire up the menu to change which control set is active (X to select, /\ to cancel), then press Note to enable/disable it.

############### Directory Structure ###############
remaps/ <- This folder contains the actual remap files.
remaps/auto <- This folder contains remaps to autoload based on the inserted UMD (Devhook only)
               Put files in here named "UMDID.txt" to have them automatically load and be enabled when you load a game.
               There is an example included: UCES-00304.txt which is the European version of Loco Roco.
remaps/macros <- This folder contains the macros to be loaded by remaps.

################# Notes ################
The nkthread.prx is the same as the one included with the screenshot prx (THANKS MAN IT IS AWESOME :)))) )
Speaking of screenshot, you will need to disable it as we share hotkeys.

When running a macro you cannot press any buttons, this is by design.

########### Included Remaps ############
defaults         - Sets all the buttons to themself (Not useful, but read it first!)
backwards        - Reverses the directions of the digital and analog sticks
macro_test       - Same as defaults but with 2 included macros, press LTrigger+RTrigger to run one or X+O to run the other.
swap_digi_ana    - Swaps the digital and analog sticks, quite useful in some games!
turbo_test       - Makes all the face buttons (X, O, [], /\) be turbo! Fairly useless in tekken but still funny ;) Good in Gradius!
locoremoto       - Play LocoRoco with the psp remote! Next/Prev track to go left/right, play = O to split/join :)
astonishia_story - Press /\ in Battle to skip turn, Press [] in Battle to attack.
auto/UCES-00304.txt - This is locoremoto with locolaunch, it is automatically loaded when you start the European version of LocoRoco in devhook.
                      Chose your flag on the start screen and press [] to have remapsp automatically load your save game and run the level you are up to :)

You can create your own remaps (Please do!), read howtoRemap.txt for info!

############## Known Bugs ##############
 - The 1.5 Launcher can't launch all homebrew, not sure why. maybe a shell coder could share some information?
 - The menu doesn't always render correctly, there are undrawn lines in the text.
 - RemaPSP uses up some cpu to work, so it may make programs run slower when in use. (Not really a bug, lol)
   I have heard that some of the emulators are affected, but have not experienced any slowdown with retail games.
 - When rendering notices while a game is running the text flickers, I don't think there is anything much I can do about this.
 - If the game switches rendering mode (resolution) while RemaPSP is drawing some text, it will sometimes lock the psp. Be careful of enabling/disabling near movies as they usually switch resolution.

######### Potential todo list ##########
More default remaps (Mail me any good ones you make :) I'll credit ya for them )
Allow more remaps (current max 15 on the menu)
Change note to something else I think.... - What about pressing (RTrigger or LTrigger) and (Vol+ or Vol-) at the same time?
Fix minor rendering errors. (Unlikely :( )