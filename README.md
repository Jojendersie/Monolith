# Setting up the config file #
Manual changes of the config file are not required after release but the only way until a menu exists.

## Assigning keys ##

The folling attributes are key mappings: MoveCamera, RotateCamera, Zoom.

Each of this attributes is an array of integer attributes. No integer value should be used more than once. Each attribute needs at least one value. More than one value means that multiple keys from mouse/... are mapped to the same action. Which integer key is which button can be found in following table:
 
0 - Mouse Left   
1 - Mouse Right   
2 - Mouse Middle    
65 - 90 a-z   

The mapping might change (Currently there are buttons Mouse <-> Joystick <-> Keyboard which map on the same key.