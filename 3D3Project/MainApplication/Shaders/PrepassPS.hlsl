Texture2D ObjTex : register(t0);
Texture2D GlowTex : register(t5);
Texture2D SpecTex : register(t6);

SamplerState sampAni : register(s0);

cbuffer OBJColourType : register(b0)
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

struct PS_OUT
{
	float4 Normal : SV_Target0;
	float4 DiffAlbedo : SV_Target1;
	float4 SpecAlbedo : SV_Target2;
	float4 Pos : SV_Target3;
	float4 Glow : SV_Target4;
};

PS_OUT PS_main(PS_IN input)
{
	PS_OUT output = (PS_OUT)0;

	float _depth = (input.PosInW.y + 2) / 4;

	output.Normal = input.NormalInW;
	output.DiffAlbedo = (float4(Diffuse, 1) * _depth) + (ObjTex.Sample(sampAni, input.Tex) * _depth);
	output.SpecAlbedo = SpecTex.Sample(sampAni, input.Tex);
	output.Pos = input.PosInW;
	output.Glow = ((float4(Diffuse, 1) * _depth) + (ObjTex.Sample(sampAni, input.Tex) * _depth))  * float4(GlowTex.Sample(sampAni, input.Tex).xyz, 1);

	return output;
}