
float4 AmbientColor  = { 0.3, 0.3, 0.3, 1 };
float4 DiffuseColor  = { 1, 1, 1, 1 };

float4 SpecularColor = { 1, 1, 1, 1 };


float  SpecularPower = 16.0;

float4 EyePos        = {0,0,0,0};
float4 lightpos      = { .707*40.0, 0.707*40.0, -.707*40.0, 1};	//vector pointing away from light

float4x4 wvp;
float4x4 wm;
float4x4 vp;
float4x4 wv;
float4x4 proj;

texture DiffuseMap    : DIFFUSEMAP;

struct VS_INPUT {
  float4 Pos      : POSITION;
  float3 Normal   : NORMAL;
  float2 TexCoord : TEXCOORD0;
  float4 Diffuse  : COLOR;
};


struct VS_OUTPUT {
  float4 Pos       : POSITION;
  float3 TexCoord  : TEXCOORD0;
  float4 TexCoord1 : TEXCOORD1;
  float4 TexCoord2 : TEXCOORD2;
  float3 TexCoord3 : TEXCOORD3;
  float3 TexCoord4 : TEXCOORD4;
  float3 TexCoord5 : TEXCOORD5;
  float4 Diffuse   : COLOR;
};

sampler2D Sampler1 = sampler_state
{
  Texture   = (DiffuseMap);
  MipFilter = LINEAR;
  MinFilter = LINEAR;
  MagFilter = LINEAR;
};


VS_OUTPUT SoftBodyVertexShader(VS_INPUT In,uniform float3 ldir)
{
  VS_OUTPUT Out = (VS_OUTPUT)0;

  float4 opos = mul(In.Pos,wm);
  Out.Pos     = mul(opos,vp);

  Out.TexCoord = float3(In.TexCoord,0);

  float3 normal = normalize(In.Normal);

  float3 newnorm = mul(normal,(float3x3)wm);

  float NdotL = dot(newnorm,ldir);

  Out.Diffuse = (DiffuseColor*clamp(NdotL,0,1)) + AmbientColor;

  // ok.compute environment map texture coords
  //
  // Compute normal in camera space
  //
  float3 vN = mul(normal, wv );
  vN = normalize( vN );

	Out.TexCoord1.xyz = vN;

  //
  // Obtain the reverse eye vector
  //
  float3 vEyeR = -normalize( opos );

  //
  // Compute the reflection vector
  //
  float3 vRef = 2 * dot( vEyeR, vN ) * vN - vEyeR;

  //
  // Store the reflection vector in texcoord1
  //
  Out.TexCoord2.xyz = vRef;

  return Out;
}


VS_OUTPUT SoftBodyVertexShaderWireFrame(VS_INPUT In)
{
  VS_OUTPUT Out = (VS_OUTPUT)0;

  float4 opos = mul(In.Pos,wm);
  Out.Pos     = mul(opos,vp);
  Out.Diffuse = In.Diffuse;
  return Out;
}


float4 SoftBodyPixelShader(VS_OUTPUT In,uniform float sPower) : COLOR
{
  float4 outColor = tex2D(Sampler1,In.TexCoord);

  // Compute normal dot half for specular light
  float3 vHalf = normalize( normalize( -EyePos ) + normalize( lightpos - EyePos ) );
  float4 fSpecular = pow( saturate( dot( vHalf, normalize( In.TexCoord1.xyz ) ) ) * SpecularColor, sPower );

//  float4 envMap = texCUBE(SamplerNormalMap,In.TexCoord2)*fSpecular;

  return (outColor*In.Diffuse)+fSpecular;

}

float4 SoftBodyPixelShaderWireFrame(VS_OUTPUT In,uniform float4 wcolor) : COLOR
{
  return wcolor*In.Diffuse;
}


float3 softLdir       = { .707, .707, -.707 };	//vector pointing away from light

technique SoftBodyShader
{
  pass P0
  {
    CullMode = Ccw;
    VertexShader = compile vs_2_0 SoftBodyVertexShader(softLdir);
    PixelShader  = compile ps_2_0 SoftBodyPixelShader(SpecularPower);
  }
}

technique SoftBodyWireFrameShader
{
  pass P0
  {
    FillMode = WIREFRAME;
    CullMode = Ccw;
    VertexShader = compile vs_2_0 SoftBodyVertexShaderWireFrame();
    PixelShader  = compile ps_2_0 SoftBodyPixelShaderWireFrame(DiffuseColor);
  }
}

