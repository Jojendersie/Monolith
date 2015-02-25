// Returns diffuse and specular coefficient for modified Ashikmin-Shirley-BRDF
vec2 AshikminShirleyMod(vec3 _normal, vec3 _view, vec3 _light, float _shininess, float _power)
{
	vec2 ds;

	vec3 H = normalize(_light + _view);
	float NdotL = min(1, max(0, dot(_normal, _light)));
	float NdotV = min(1, max(0, dot(_normal, _view)));
	float HdotL = min(1, max(0, dot(H, _light)));
	float NdotH = min(1, max(0, dot(_normal, H)));

	// Modified Ashikhmin-Shirley diffuse part
	ds.x = 3.14 *  0.387507688 * (1.0 - pow(1.0 - NdotL * 0.5, 5.0)) * (1.0 - pow(1.0 - NdotV * 0.5, 5.0));
	// (1-_shininess) *

	// Modified Ashikhmin-Shirley specular part
	ds.y = (_power+1) * pow(NdotH, _power) / (25.132741229 * HdotL * max(NdotL, NdotV) + 0.001);
	//co.y = (_power+1) * pow(NdotH, _power) / 25.132741229;
	ds.y *= 0.1 * (_shininess + (1-_shininess) * (pow(1.0 - HdotL, 5.0)));

	return ds;
}

// Returns diffuse and specular coefficient for small Blinn-Phong with Fresnel
vec2 BlinnPhongFresnel(vec3 _normal, vec3 _view, vec3 _light, float _shininess, float _power)
{
	vec2 ds;	

	vec3 H = normalize(_light + _view);
	float NdotL = min(1, max(0, dot(_normal, _light)));
	float NdotH = min(1, max(0, dot(_normal, H)));
	float HdotV = min(1, max(0, dot(H, _view)));
	
	ds.x = NdotL;
	ds.y = (_power+1) * pow(NdotH, _power) / 120.0;
	/*float fresnel = _shininess + (1-_shininess) * pow(1-HdotV, 5.0);
	ds.y *= fresnel;//*/

	return ds;
}

// Compute the lighting for all existing lights
vec3 Lightning(/*vec3 _position, */vec3 _normal, vec3 _viewDir, float _shininess, float _power, vec3 _color, float _emissive)
{
	// Test with 
	vec3 light = normalize((vec4(0.13557,0.96596,0.2203,0)).xyz);

	// Compute BRDF for this light
	vec2 ds = BlinnPhongFresnel(_normal, _viewDir, light, _shininess, _power);
	// Combine lighting
	// TODO: Light color
	return min(vec3(1,1,1), max(vec3(0,0,0), (ds.x + _emissive + 0.4) * _color + ds.yyy));
}
