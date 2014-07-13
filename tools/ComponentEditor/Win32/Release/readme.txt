The component editor allows to create and alter voxels/components. 
To prevent buggs and unintended or unexpected behavior its important to follow the usage instructions below.

A session should start with loading the voxel.json because merging afterwards is only possible manualy.
The voxel UNDEFINED should be ignored aswell as the color 0.
The colors are processed as YCbCr and hex values and should be created with the colorbuilder.
Any visual represenations show only the RGB parts, yet the full information is preserved. 

After creating a Voxel atleast one color has to be added. 
Then one cube of this color is spawned in a edge.