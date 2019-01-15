cbuffer CB0 {
	matrix WVPMatrix; // 64 bytes
	float4 directional_light_vector; // 16 bytes
	float4 directional_light_colour; // 16 bytes
	float4 ambient_light_colour; // 16 bytes
	float4 camera_position; // 16 bytes
}; // total = 124 bytes

Texture2D		texture0;
Texture2D		texture1;
SamplerState	sampler0;

struct VOut {
	float4 position	: SV_POSITION;
	float4 color	: COLOR;
	float2 texcoord	: TEXCOORD;
	float3 normal	: NORMAL;
};

VOut ModelVS(float4 position : POSITION, float2 texcoord : TEXCOORD, float3 normal : NORMAL) {
	VOut output;

	float4 default_color = { 1.0, 1.0, 1.0, 1.0 };
	output.position = mul(WVPMatrix, position);
	output.texcoord = texcoord;

	float diffuse_amount = dot(directional_light_vector, normal);
	diffuse_amount = saturate(diffuse_amount);
	output.color = ambient_light_colour + (directional_light_colour * diffuse_amount);
	output.normal = normalize(mul(WVPMatrix, normal));

	//output.color *= saturate(dot(normal, directional_light_vector)) * directional_light_colour + ambient_light_colour;

	return output;
}

float4 ModelPS(float4 position : SV_POSITION, float4 color : COLOR, float2 texcoord : TEXCOORD, float3 normal : NORMAL) : SV_TARGET{
	float4 finalColor = color;
	float3 toCamera = camera_position.xyz - position.xyz;
	toCamera = normalize(toCamera);

	// specular
	float3 toLight = directional_light_vector.xyz - toCamera;
	float NDotL = dot(toLight, normal);
	finalColor += color * saturate(NDotL);
	float3 halfWay = normalize(toLight - toCamera);
	float NDotH = saturate(dot(halfWay, normal));
	finalColor += color * pow(NDotH, 0.001) * 0.0001;

	float intensity = (color.x + color.y + color.z + color.w) / 4.0;
	float4 celShading;

	if (intensity >= 1)
		celShading = float4(1.25, 1.25, 1.25, 1.0);
	else if (intensity > 0.85)
		celShading = float4(1.0, 1.0, 1.0, 1.0);
	else if (intensity > 0.5)
		celShading = float4(0.7, 0.7, 0.7, 1.0);
	else if (intensity > 0.03)
		celShading = float4(0.5, 0.5, 0.5, 1.0);
	else
		celShading = float4(0.3, 0.3, 0.3, 1.0);


	float4 tex = texture0.Sample(sampler0, texcoord);
	if (texture1.Sample(sampler0, texcoord).w != 0) tex *= texture1.Sample(sampler0, texcoord);
	//return tex * (color + float4(0.5, 0.5, 0.5, 1.0)) * celShading;
	return tex *  finalColor * celShading;
}