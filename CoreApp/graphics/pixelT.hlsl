// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>}Y{,01.11.2019</created>
/// <changed>}Y{,02.11.2019</changed>
// ********************************************************************************

Texture2D shaderTexture; // texture resource
SamplerState samplerType; // how the pixels are written on the polygon face

// texture pixel shader input type
struct Pixel
{
  float4 position : SV_POSITION;
  float2 tex : TEXCOORD0;
};


// texture pixel shader: sampling from the texture draws each pixel on the polygons that will be rendered to the screen.
float4 main( Pixel input ) : SV_TARGET // called by GPU when the output of vertex shader is ready
{
  
  // SV_TARGET: the semantic indicates that the return value of texture pixel shader should match the render target format
  
  float4 textureColour;

  // using sampler, sample the pixel colour from the texture at this texture coordinate location.
  textureColour = shaderTexture.Sample(samplerType, input.tex);
  
  return textureColour; // from texture sampled colour
  
}
