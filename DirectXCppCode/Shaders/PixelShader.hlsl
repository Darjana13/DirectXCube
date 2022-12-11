//--------------------------------------------------------------------------------------
struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
	float4 Normal: NORMAL;
	float4 color : COLOR0;
};
Texture2D  gTexture : register(t0);
SamplerState TextureSampler
{
	Filter = MIN_MAG_MIP_LINEAR;
};
//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 main( VS_OUTPUT input ) : SV_Target
{
	float intens=mul(input.Normal,float4(0,-1,0,0));
	/*float4 color_from_pos;
	color_from_pos[0] = input.Pos[0];
	color_from_pos[1] = input.Pos[1];
	color_from_pos[2] = input.Pos[2];
	color_from_pos[3] = 1.0;*/

	/*float2 texturecoor;
	texturecoor[0] = input.color[0];
	texturecoor[1] = 1-input.color[1];
	return gTexture.Sample(TextureSampler, texturecoor);*/
    return input.color;
}
