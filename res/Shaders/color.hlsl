//***************************************************************************************
// color.hlsl by Frank Luna (C) 2015 All Rights Reserved.
//
// Transforms and colors geometry.
//***************************************************************************************
Texture2D gDiffuseMap : register(t0);
SamplerState gsamLinear : register(s0);
cbuffer cbPerObject : register(b0)
{
    float4x4 gWorldViewProj;
    float4x4 gWorld;
    float4x4 gWorldInvTranspose;
    int gUseTexture;
    int gUseLight;
};

cbuffer cbLight : register(b1)
{
    float3 gLightDirection;
    float3 gLightStrength;
    float4 gLightColor;
};

struct VertexIn
{
    float3 PosL : POSITION;
    float4 Color : COLOR;
    float3 Normal : NORMAL;
    float2 TexC : TEXCOORD;
};

struct VertexOut
{
    float4 PosH : SV_POSITION;
    float4 Color : COLOR;
    float3 Normal : NORMAL;
    float2 TexC : TEXCOORD;
    float3 PosW : TEXCOORD1;
};

VertexOut VS(VertexIn vin)
{
    VertexOut vout;
    vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj);

    float4 posW4 = mul(float4(vin.PosL, 1.0f), gWorld);
    vout.PosW = posW4.xyz / posW4.w;

    vout.Color = vin.Color;
    
    float3 n = mul((float3x3) gWorldInvTranspose, vin.Normal);
    vout.Normal = normalize(n);
    vout.TexC = vin.TexC;
    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
    //float4 baseColor = pin.Color;

    //if (gUseTexture != 0)
    //{
    //    baseColor = gDiffuseMap.Sample(gsamLinear, pin.TexC);
    //}
    
    //float3 N = normalize(pin.Normal);
    //float3 L = normalize(-gLightDirection);
    //float NdotL = saturate(max(dot(N, L), 0.0f));

    //float3 ambient = gLightStrength * 0.2f;
    //float3 diffuse = gLightStrength * NdotL;
    //float3 lighting = saturate(ambient + diffuse);

    //float3 result = baseColor.rgb * lighting * gLightColor.xyz;
    //if (gUseLight == 0)
    //{
    //    return float4(baseColor.rgb, baseColor.a);
    //}
    //else
    //{
    //    return float4(result, baseColor.a * gLightColor.w);
    //}
    return float4(pin.TexC, 0, 1);
}