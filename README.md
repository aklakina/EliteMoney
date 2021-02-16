# EliteMoney
Mission overview program for Elite Dangerous massacre
# Usage
You need to open this application first and after this open Elite in order so it can gather the required data. (Only needed to run first if you are docked to a station where you want to pickup missions from)
## Adding missions
The app handles it automaticaly
## Loading system config file
On this tab you need to load the correct file you wish to use. These files are completely editable json files found under ./System
Once loaded switching is just as easy. The current loaded config file is displayed as green.
## Editing config file
By default this window is empty. As soon as you load a config file it fills up with it's hierarchy.
You can edit each node by double clicking on it. After editing it is ***highly recommended to save and restart the program***.
To add a subItem to a node you first have to click on a node you wish to add it for. By default the first node should be the name of the config file.
***Remove currently selected item can mass your missions if you delete a faction that is present in multiple stations and you delete it from only one of them. The issue is that it will delete all the missions from that faction.***
Furthuremore Saving will overwrite configs without question and delete will delete it without warning.
There is and example hierarchy in the ./System folder and the program would crash if it got any deeper or any less deeper config file.
You need to add the factions correct name (it handles faction1 at station1 and station2 as one faction).
## The buttons at the top
They are used to select the next or previous system config file.
# Zip file download link:
http://users.itk.ppke.hu/~paple/EliteMoney/compiled.zip
