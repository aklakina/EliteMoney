# EliteMoney
Mission overview program for Elite Dangerous massacre
# Usage
# Adding missions
On the first tab you can add missions to the data.
By default after loaded the system you wish to use the program selects the first options in each sub window. You can select different ones by clicking on them.
After you have selected the correct faction set the required kill and credit reward and click add mission. This will refresh the whole data as well as the display at the bottom.
For removing mission you need to click on the mission you wish to remove first and after that you need to click remove mission button.
# Loading system config file
On this tab you need to load the correct file you wish to use. These files are completely editable json files found under ./System
Once loaded switching is just as easy. The current loaded config file is displayed as green.
# Editing config file
By default this window is empty. As soon as you load a config file it fills up with it's hierarchy.
You can edit each node by double clicking on it. After editing it is ***highly recommended to save and restart the program***.
To add a subItem to a node you first have to click on a node you wish to add it for. By default the first node should be the name of the config file.
***Remove currently selected item can mass your missions if you delete a faction that is present in multiple stations and you delete it from only one of them. The issue is that it will delete all the missions from that faction.***
Furthuremore Saving will overwrite configs without question and delete will delete it without warning.
There is and example hierarchy in the ./System folder and the program would crash if it got any deeper or any less deeper config file.
You need to add the factions correct name (it handles faction1 at station1 and station2 as one faction).
# The buttons at the top
They are used to select the next or previous system config file.
