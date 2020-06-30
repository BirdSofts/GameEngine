// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>}Y{,27.06.2020</created>
/// <state></state>
// ********************************************************************************

// pixel (input type)
struct Vertex
{
  float4 position : SV_POSITION;
  float4 colour : COLOR;
};


// pixel shader: draws each pixel on the polygons that will be rendered to the screen.
float4 main( Vertex input ) : SV_TARGET // called by GPU when the output of vertex shader is ready
//float4 main ( float4 pos : SV_POSITION, float4 col : COLOR  ) : SV_TARGET
{
  
  // SV_TARGET: the semantic indicates that the return value of pixel shader should match the render target format
  
  //input.colour *= 0.5f; // output each pixel half bright
  return input.colour; // colour
  
}
