// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>}Y{,30.08.2020</created>
/// <state></state>
// ********************************************************************************

// global declarations
// buffer object type, containing three matrices, updated on each execution
cbuffer MatrixBuffer
{
  matrix worldMatrix;
  matrix viewMatrix;
  matrix projectionMatrix;
};


// type declarations
// colour vertex shader input type
struct Vertex
{
  float4 position : POSITION; // vertex shaders
  float4 colour : COLOR;
  float3 normal : NORMAL;
};

// colour pixel shader input type
struct Pixel
{
  float4 position : SV_POSITION; // pixel shaders
  float4 colour : COLOR;
  float3 normal : NORMAL;
};


// colour vertex shader: calculate the vertex location by matrices and prepare the output for colour pixel shader
Pixel main(Vertex input) // called by GPU when processing data from vertex buffer
{
  
  // input: vertex position and colour defined by six floats
  // indicated by the POSITION and NORMAL semantic
  
  // process:
  // vertex position calculation: (against world, view, and projection matrices)
  
  // output:
  //-- passing colour of each position to pixel shader
  
  Pixel output; // output vertex structure
  
  // change the position vector to 4 units (proper matrix calculation)
  input.position.w = 1.0f;
  // manipulation of input vertex through world, view and projection matrices,
  // resulting to the correct vertex location for 3D rendering, and then onto the 2D screen
  output.position = mul(input.position, worldMatrix);
  output.position = mul(output.position, viewMatrix);
  output.position = mul(output.position, projectionMatrix);
  
  // normal vector calculation against world matrix
  output.normal = mul(input.normal, (float3x3) worldMatrix);
  
  output.normal = normalize(output.normal); // normalization
  
  // additionally store input colour (for pixel shader)
  output.colour = input.colour;

  return output;
  
};
