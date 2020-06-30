// ********************************************************************************
/// <summary>
/// 
/// </summary>
/// <created>}Y{,27.06.2020</created>
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
// vertex (input type)
struct Vertex
{
  float4 position : POSITION; // vertex shaders
  float4 colour : COLOR;
  // note that using numbers, more semantics of the same type is definable
};


// pixel (output type)
struct Pixel
{
  float4 position : SV_POSITION; // pixel shaders
  float4 colour : COLOR;
};


// vertex shader: calculate the vertex location by matrices and prepare the output for pixel shader
Pixel main( Vertex input ) // called by GPU when processing data from vertex buffer
{
  
  // input: vertex position and colour defined by seven floats
  // indicated by the POSITION and COLOR semantic
  
  ////xx process:
  //-- transformation of position into homogenous coordinates (projective geometry),
  
  // process:
  // vertex position calculation: (against world, view, and projection matrices)
  
  // output:
  //-- passing the colour of each position to pixel shader
  
  Pixel output; // output vertex structure
  
  //float4 outputPos = { input.position.x, input.position.y, input.position.z, input.position.w };
  //output.position = outputPos;
  //float4 outputCol = { input.colour.x, input.colour.y, input.colour.z, input.colour.w };
  //output.colour = outputCol;
  
  // change the position vector to 4 units (proper matrix calculation)
  input.position.w = 1.0f;
  // manipulation of input vertex through world, view and projection matrices,
  // resulting to the correct vertex location for 3D rendering, and then onto the 2D screen
  output.position = mul(input.position, worldMatrix);
  output.position = mul(output.position, viewMatrix);
  output.position = mul(output.position, projectionMatrix);
  // additionally store input colour (for pixel shader)
  output.colour = input.colour;

  return output;
  
};
