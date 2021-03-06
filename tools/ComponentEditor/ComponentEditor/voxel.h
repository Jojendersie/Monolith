#include <string>
#include <vector>

enum AttributeType
{
	String,
	Int,
	Float,
	Bool
};

struct Attribute
{
	Attribute(){};
	Attribute(const std::string& _name, float _val)
		: name(_name),
		value(_val),
		type(Float)
	{};
	Attribute(const std::string& _name, std::string& _val)
		: name(_name),
		strValue(_val),
		type(String)
	{};
	Attribute(const std::string& _name, bool _val)
		: name(_name),
		value(_val ? 1.f : 0.f),
		type(Bool)
	{};

	AttributeType type;
	
	std::string name;

	float value;
	std::string strValue;
};

struct Voxel
{
	Voxel();
	~Voxel();
	float mass;					///< Mass of a voxel
	float thresholdEnergy;		///< Energy required to destroy this voxel in Joule
	float reactionEnergy;		///< Energy released during destruction in Joule

	int hydrogen;				///< Amount of resource "hydrogen" required to build this voxel or dropped by mining.
	int carbon;					///< Amount of resource "carbon" required to build this voxel or dropped by mining.
	int metals;					///< Amount of resource "metal" required to build this voxel or dropped by mining.
	int rareEarthElements;		///< Amount of resource "rare earth element" required to build this voxel or dropped by mining.
	int semiconductors;			///< Amount of resource "semiconductor" required to build this voxel or dropped by mining.
	int heisenbergium;			///< Amount of resource "heisenbergium" required to build this voxel or dropped by mining.

	int textureResolution;		///< Number of pixels in one dimension of the volume "texture"
	int numMipMaps;				///< Number of generated mip maps
	bool isSolid;

	int colCount;
	std::vector<unsigned int> colors;	
	//the values are 3d arrays acsessed with [x + y*res + z*res*res]; Each element is an YCbrCr color value
	unsigned int* texture;			///< Main texture used for sampling of the material-sub-voxels
	unsigned int* borderTexture;

	std::string name;

	std::vector< Attribute > m_attributes;
};

