The component editor allows to create and alter voxels/components. 
To prevent buggs and unintended or unexpected behavior its important to follow the usage instructions below.

GENERAL USAGE

A session should start with loading the voxel.json because merging afterwards is only possible manual.
The voxel UNDEFINED should be ignored aswell as the color 0.
The colors are processed as YCbCr and hex values and should be created with the colorbuilder.
Any visual represenations show only the RGB parts, yet the full information is preserved. 

After creating a Voxel atleast one color has to be added. 
Then one cube of this color is spawned in a edge.

Cubes can be added with rightclick and removed with leftclick. 
When the mouse gets released focusing another cube the operation is applied on the volume
defined by the first and last selected cube.

When adding additional attributes make shure to apply changes to a cell pressing enter or entering
another cell before closing the window.

Attributes can be of 3 different types: float, string, bool.
The values "true" and "false" (case sensitive) are evaluated as bool, anythink starting with a digit a number
and everythink else as a string.

ADDITIONAL COMMANDS

cntrl + z 	Undo the latest change.

n		Create a 2x2x2 block in the center of the current component using the currently selected color.

arrow-up	Shifts every cube in direction of the current operation axis.

arrow-down	Shifts every cube in oposite direction of the current operation axis.

r		Rotates every cube around the current operation axis.

x		Sets the current operation axis to x.

y		Sets the current operation axis to y.

z		Sets the current operation axis to z.
