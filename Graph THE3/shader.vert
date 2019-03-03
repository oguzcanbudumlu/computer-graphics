#version 410

layout(location = 0) in vec3 position;

// Data from CPU
uniform mat4 MVP; // ModelViewProjection Matrix
uniform mat4 MV; // ModelView idMVPMatrix
uniform mat4 inverseTransposeV;
uniform vec4 cameraPosition;
uniform vec4 lightPosition;
uniform float heightFactor;

// Texture-related data
uniform sampler2D rgbTexture;
uniform int widthTexture;
uniform int heightTexture;


// Output to Fragment Shader
out vec2 textureCoordinate; // For texture-color
out vec3 vertexNormal; // For Lighting computation
out vec3 ToLightVector; // Vector from Vertex to Light;
out vec3 ToCameraVector; // Vector from Vertex to Camera;


vec2 findTC(vec4 coordinate){
    float xInTexture = 1 - ((coordinate.x) / (widthTexture+1));
    float yInTexture = 1 - ((coordinate.z) / (heightTexture+1));
    vec2 tc =  vec2(xInTexture, yInTexture);
    return tc;
}

vec2 findTC(vec3 coordinate){
    float xInTexture = 1 - ((coordinate.x) / (widthTexture+1));
    float yInTexture = 1 - ((coordinate.z) / (heightTexture+1));
    vec2 tc =  vec2(xInTexture, yInTexture);
    return tc;
}


float findHeight(vec4 coordinate){
    vec2 tc = findTC(coordinate);
    vec4 textureColor = texture(rgbTexture, tc);
    float y = 0.2126 * textureColor.r + 0.7152 * textureColor.g + 0.0722 * textureColor.b;
    y = y * heightFactor;
    return y;
}

vec4 findCoordinate(vec4 vertex){
    vec4 coordinate = vec4(vertex.x, 0, vertex.z, 1);

    float height = findHeight(coordinate);
    coordinate.y = height;
    return coordinate;
}

vec4 findCoordinate(vec3 vertex){
    vec4 coordinate = vec4(vertex.x, 0, vertex.z, 1);

    float height = findHeight(coordinate);
    coordinate.y = height;
    return coordinate;
}



void findLightAndCameraVectors(vec4 v){
    ToLightVector = normalize(vec3(MV * vec4(lightPosition.xyz - v.xyz,0)));
    ToCameraVector = normalize(vec3(MV * vec4(cameraPosition.xyz - v.xyz,0)));
}

void calculateNormal(int caseNumber, vec4 neighbourVertex[6], vec4 v){

    vec3 normal = vec3(0,0,0);
    if(caseNumber == 0){
        normal = normal +  cross(vec3(neighbourVertex[2] - v), vec3(neighbourVertex[3] - v));
        normal = normal +  cross(vec3(neighbourVertex[3] - v), vec3(neighbourVertex[4] - v));
    }   
    else if(caseNumber == 1){   
        normal = normal +  cross(vec3(neighbourVertex[4] - v), vec3(neighbourVertex[5] - v));
    }   
    else if(caseNumber == 2){   
        normal = normal +  cross(vec3(neighbourVertex[5] - v), vec3(neighbourVertex[0] - v));
        normal = normal +  cross(vec3(neighbourVertex[0] - v), vec3(neighbourVertex[1] - v));
    }   
    else if(caseNumber == 3){   
        normal = normal +  cross(vec3(neighbourVertex[1] - v), vec3(neighbourVertex[2] - v));
    }       
    else if(caseNumber == 4){   
        normal = normal +  cross(vec3(neighbourVertex[2] - v), vec3(neighbourVertex[3] - v));
        normal = normal +  cross(vec3(neighbourVertex[3] - v), vec3(neighbourVertex[4] - v));
        normal = normal +  cross(vec3(neighbourVertex[4] - v), vec3(neighbourVertex[5] - v));
    }   
    else if(caseNumber == 5){   
        normal = normal +  cross(vec3(neighbourVertex[4] - v), vec3(neighbourVertex[5] - v));
        normal = normal +  cross(vec3(neighbourVertex[5] - v), vec3(neighbourVertex[0] - v));
        normal = normal +  cross(vec3(neighbourVertex[0] - v), vec3(neighbourVertex[1] - v));
    }
    else if(caseNumber == 6){
        normal = normal +  cross(vec3(neighbourVertex[5] - v), vec3(neighbourVertex[0] - v));
        normal = normal +  cross(vec3(neighbourVertex[0] - v), vec3(neighbourVertex[1] - v));
        normal = normal +  cross(vec3(neighbourVertex[1] - v), vec3(neighbourVertex[2] - v));
    }
    else if(caseNumber == 7){
        normal = normal +  cross(vec3(neighbourVertex[1] - v), vec3(neighbourVertex[2] - v));
        normal = normal +  cross(vec3(neighbourVertex[2] - v), vec3(neighbourVertex[3] - v));
        normal = normal +  cross(vec3(neighbourVertex[3] - v), vec3(neighbourVertex[4] - v));
    }    
    else if(caseNumber == 8){
        normal = normal +  cross(vec3(neighbourVertex[0] - v), vec3(neighbourVertex[1] - v));
        normal = normal +  cross(vec3(neighbourVertex[1] - v), vec3(neighbourVertex[2] - v));
        normal = normal +  cross(vec3(neighbourVertex[2] - v), vec3(neighbourVertex[3] - v));    
        normal = normal +  cross(vec3(neighbourVertex[3] - v), vec3(neighbourVertex[4] - v));    
        normal = normal +  cross(vec3(neighbourVertex[4] - v), vec3(neighbourVertex[5] - v));    
    }


    normal = normalize(normal);
    vertexNormal = normalize(vec3( transpose(inverse(MV)) * vec4(normal,0) ));
    
}



void findVertexNormal(vec4 v){
   /**
    *     2  3
    *  1  v  4
    *  0  5  
    *
    * 
    **/
    
    vec4 neighbourVertex[6];
    neighbourVertex[0] = vec4(v.x - 1, 0, v.z - 1, 1);
    neighbourVertex[0] = findCoordinate(neighbourVertex[0]);
    
    neighbourVertex[1] = vec4(v.x - 1, 0, v.z, 1);
    neighbourVertex[1] = findCoordinate(neighbourVertex[1]);

    neighbourVertex[2] = vec4(v.x, 0, v.z + 1, 1);
    neighbourVertex[2] = findCoordinate(neighbourVertex[2]);

    neighbourVertex[3] = vec4(v.x + 1, 0, v.z + 1, 1);
    neighbourVertex[3] = findCoordinate(neighbourVertex[3]);

    neighbourVertex[4] = vec4(v.x + 1, 0, v.z, 1);
    neighbourVertex[4] = findCoordinate(neighbourVertex[4]);

    neighbourVertex[5] = vec4(v.x, 0, v.z - 1, 1);
    neighbourVertex[5] = findCoordinate(neighbourVertex[5]);

    
    if(v.x == 0 && v.y == 0){ // caseNumber 0
        calculateNormal(0, neighbourVertex, v);
    }
    else if(v.x == 0 && v.z == heightTexture){ // caseNumber 1
        calculateNormal(1, neighbourVertex, v);
    }
    else if(v.x == widthTexture && v.z == heightTexture){ // caseNumber 2 
        calculateNormal(2, neighbourVertex, v);
    }
    else if(v.x == widthTexture && v.z == 0){ // caseNumber 3
        calculateNormal(3, neighbourVertex, v);
    }
    else if(v.x == 0){ // caseNumber 4
        //2, 3, 4, 5
        calculateNormal(4, neighbourVertex, v);
    }
    else if(v.z == heightTexture){ // caseNumber 5
        //1,4,5,0
        calculateNormal(5, neighbourVertex, v);        
    }
    else if(v.x == widthTexture){ // caseNumber 6
        //5,0,1,2
        calculateNormal(6, neighbourVertex, v);        
    }
    else if(v.z == 0){ // caseNumber 7
        //1,2,3,4
        calculateNormal(7, neighbourVertex, v);        
    }
    else{ // caseNumber 8
        //0,1,2,3,4,5
        calculateNormal(8, neighbourVertex, v);
    }   
    

}

void main()
{

    textureCoordinate = findTC(position);

    vec4 v = findCoordinate(position);

    findVertexNormal(v);


    // compute toLight vector & toCamera vector vertex coordinate in VCS
    findLightAndCameraVectors(v);


    // set gl_Position variable correctly to give the transformed vertex position
    gl_Position = MVP * v;

}
