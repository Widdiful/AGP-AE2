cbuffer CB0 {
	matrix WVPMatrix; // 64 bytes
	float4 directional_light_vector; // 16 bytes
	float4 directional_light_colour; // 16 bytes
	float4 ambient_light_colour; // 16 bytes
}; // total = 112 bytes

Texture2D		texture0;
SamplerState	sampler0;

struct VOut {
	float4 position	: SV_POSITION;
	float4 color	: COLOR;
	float2 texcoord	: TEXCOORD;
};

VOut ModelVS(float4 position : POSITION, float2 texcoord : TEXCOORD, float3 normal : NORMAL) {
	VOut output;

	float4 default_color = { 1.0, 1.0, 1.0, 1.0 };
	output.position = mul(WVPMatrix, position);
	output.texcoord = texcoord;

	float diffuse_amount = dot(directional_light_vector, normal);
	diffuse_amount = saturate(diffuse_amount);
	output.color = ambient_light_colour + (directional_light_colour * diffuse_amount);

	output.color *= saturate(dot(normal, directional_light_vector)) * directional_light_colour + ambient_light_colour;

	return output;
}

float4 ModelPSCel(float4 position : SV_POSITION, float4 color : COLOR, float2 texcoord : TEXCOORD) : SV_TARGET{
	return texture0.Sample(sampler0, texcoord) * color;
}

float4 ModelPS(float4 position : SV_POSITION, float4 color : COLOR, float2 texcoord : TEXCOORD) : SV_TARGET{
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



	return texture0.Sample(sampler0, texcoord) * (color + float4(0.5, 0.5, 0.5, 1.0)) * celShading;
}