# EliteMoney
Mission overview program for Elite Dangerous massacre
# Usage
You need to open Elite Dangerous first, sign in and connect to a session (open/private/solo). After this open this program. Go back to elite and start doing mission pickups and killing. After a jump and a dock it should synchronize everything automatically and no user interaction needed. When you are done with playing go to the config tab and click save so your progress will be saved and you can reload it later.
# Reloading 
To reload progress you need to go to the third tab there select a system. With this method it does not matter when you start elite it will work without elite.
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
# Please do not forget to support me if you liked what I've created
Click on the support button on the top to do so.
