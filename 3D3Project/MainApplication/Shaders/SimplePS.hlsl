Texture2D ObjTex : register(t0);

SamplerState sampAni : register(s0);

cbuffer OBJColourType : register(b2)
{
	float3 Diffuse;
	float3 Albi;
	float3 Tf;
	float3 Ni;
};

struct PS_IN
{
	float4 Pos : SV_POSITION;
	float4 PosInW : WORLDPOS;
	float4 NormalInW : NORMALINW;
	float2 Tex : TEXCOORD;
};

float4 PS_main(PS_IN input) : SV_TARGET
{
	return /*float4(Diffuse, 1) +*/ float4(ObjTex.Sample(sampAni, input.Tex));
}