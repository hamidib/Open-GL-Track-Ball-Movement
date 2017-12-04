// 
// Michael Shafae
// mshafae at fullerton.edu
// 
// A toy program which renders a teapot and two light sources. 
//
//

#include <tuple>
#include <cstdlib>
#include <cstdio>
#include <sys/time.h>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_inverse.hpp>

#include "GLFWApp.h"
#include "GLSLShader.h"
#include "glut_teapot.h"

#include "SpinningLight.h"
#include "Camera.h"
#include "UtahTeapot.h"

#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

void msglVersion(void){
  fprintf(stderr, "OpenGL Version Information:\n");
  fprintf(stderr, "\tVendor: %s\n", glGetString(GL_VENDOR));
  fprintf(stderr, "\tRenderer: %s\n", glGetString(GL_RENDERER));
  fprintf(stderr, "\tOpenGL Version: %s\n", glGetString(GL_VERSION));
  fprintf(stderr, "\tGLSL Version: %s\n",
          glGetString(GL_SHADING_LANGUAGE_VERSION));
}


class TeapotTrackballApp : public GLFWApp{
private:
  float rotationDelta;

  glm::vec3 centerPosition;
  Camera mainCamera;

  glm::mat4 modelViewMatrix;
  glm::mat4 projectionMatrix;
  glm::mat4 normalMatrix;
  
  GLSLProgram shaderProgram;

  SpinningLight light0;
  SpinningLight light1; 

  UtahTeapot teapot;

  // Variables to set uniform params for lighting fragment shader 
  unsigned int uModelViewMatrix;
  unsigned int uProjectionMatrix;
  unsigned int uNormalMatrix;
  unsigned int uLight0_position;
  unsigned int uLight0_color;
  unsigned int uLight1_position;
  unsigned int uLight1_color;
  unsigned int uAmbient;
  unsigned int uDiffuse;
  unsigned int uSpecular;
  unsigned int uShininess;
  
public:
  TeapotTrackballApp(int argc, char* argv[]) :
    GLFWApp(argc, argv, std::string("Teapot Vision").c_str( ), 
            600, 600){ }
  
  void initCenterPosition( ){
    centerPosition = glm::vec3(0.0, 0.0, 0.0);
  }
  
  void initTeapot( ){
    Material* m = new Material(glm::vec4(0.2, 0.2, 0.2, 1.0), glm::vec4(0.0, 1.0, 1.0, 1.0), glm::vec4(1.0, 1.0, 1.0, 1.0), 100.0);
    teapot = UtahTeapot(centerPosition, 1.0, m);
  }

  void initCamera( ){
    mainCamera = Camera(glm::vec3(0.0, 0.0, 20.0), glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 0.0), 45.0, 1.0, 100.0);
  }

  void initRotationDelta( ){
    rotationDelta = 0.05;
  }
   
  void initLights( ){
    glm::vec3 color0(1.0, 1.0, 1.0);
    glm::vec3 position0(0.0, 50.0, 50.0);
    glm::vec3 color1(1.0, 1.0, 1.0);
    glm::vec3 position1(0.0, 50.0, -50.0);
    light0 = SpinningLight(color0, position0, centerPosition);
    light1 = SpinningLight(color1, position1, centerPosition);
  }

  bool begin( ){
    msglError( );
    initCenterPosition( );
    initTeapot( );
    initCamera( );
    initRotationDelta( );
    initLights( );
    
    // Load shader programs
    const char* vertexShaderSource = "blinn_phong.vert.glsl";
    const char* fragmentShaderSource = "blinn_phong.frag.glsl";
    FragmentShader fragmentShader(fragmentShaderSource);
    VertexShader vertexShader(vertexShaderSource);
    shaderProgram.attach(vertexShader);
    shaderProgram.attach(fragmentShader);
    shaderProgram.link( );
    shaderProgram.activate( );
    
    printf("Shader program built from %s and %s.\n",
           vertexShaderSource, fragmentShaderSource);
    if( shaderProgram.isActive( ) ){
      printf("Shader program is loaded and active with id %d.\n", shaderProgram.id( ) );
    }else{
      printf("Shader program is not active, id: %d\n.", shaderProgram.id( ));
    }
    
    // Set up uniform variables for the shader program
    uModelViewMatrix = glGetUniformLocation(shaderProgram.id( ), "modelViewMatrix");
    uProjectionMatrix = glGetUniformLocation(shaderProgram.id( ), "projectionMatrix");
    uNormalMatrix = glGetUniformLocation(shaderProgram.id( ), "normalMatrix");
    uLight0_position = glGetUniformLocation(shaderProgram.id( ), "light0_position");
    uLight0_color = glGetUniformLocation(shaderProgram.id( ), "light0_color");
    uLight1_position = glGetUniformLocation(shaderProgram.id( ), "light1_position");
    uLight1_color = glGetUniformLocation(shaderProgram.id( ), "light1_color");
    uAmbient = glGetUniformLocation(shaderProgram.id( ), "ambient");
    uDiffuse = glGetUniformLocation(shaderProgram.id( ), "diffuse");
    uSpecular = glGetUniformLocation(shaderProgram.id( ), "specular");
    uShininess = glGetUniformLocation(shaderProgram.id( ), "shininess");

    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    msglVersion( );
    
    return !msglError( );
  }
  
  bool end( ){
    windowShouldClose( );
    return true;
  }
  
  void activateUniforms(glm::vec4& _light0, glm::vec4& _light1, Material* m){
    glUniformMatrix4fv(uModelViewMatrix, 1, false, glm::value_ptr(modelViewMatrix));
    glUniformMatrix4fv(uProjectionMatrix, 1, false, glm::value_ptr(projectionMatrix));
    glUniformMatrix4fv(uNormalMatrix, 1, false, glm::value_ptr(normalMatrix));

    glUniform4fv(uLight0_position, 1, glm::value_ptr(_light0));
    glUniform4fv(uLight0_color, 1, glm::value_ptr(light0.color( )));
    
    glUniform4fv(uLight1_position, 1, glm::value_ptr(_light1));
    glUniform4fv(uLight1_color, 1, glm::value_ptr(light1.color( )));

    glUniform4fv(uAmbient, 1, glm::value_ptr(m->ambient));
    glUniform4fv(uDiffuse, 1, glm::value_ptr(m->diffuse));
    glUniform4fv(uSpecular, 1, glm::value_ptr(m->specular));
    glUniform1f(uShininess, m->shininess);
  }
  
    glm::vec3 shoemake(glm::vec4 wincoords, float teapotSize)
    {
        float xx = wincoords.x * wincoords.x;
        float yy = wincoords.y * wincoords.y;
        float rr = (teapotSize/2) * (teapotSize/2); //either or not /2
        
        if (xx + yy > rr)
        {
             std::cout <<"not on teapot" << std::endl;
            return glm::vec3(wincoords.x, wincoords.y, sqrt(rr-(xx + yy)));
        }else //( xx + yy <= rr)
        {
            std::cout <<"On teapot" << std::endl;
            return glm::vec3((teapotSize/2)/(sqrt(xx+yy)) * glm::vec3(wincoords.x, wincoords.y, 0));
        }
    }
 
    
  bool render( ){
    glm::vec4 _light0;
    glm::vec4 _light1;
    glm::mat4 lookAtMatrix;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    std::tuple<int, int> w = windowSize( );
    double ratio = double(std::get<0>(w)) / double(std::get<1>(w));

    mainCamera.perspectiveMatrix(projectionMatrix, ratio);

    mainCamera.lookAtMatrix(lookAtMatrix);

    // Set light & material properties for the teapot;
    // lights are transformed by current modelview matrix
    // such that they are positioned correctly in the scene.
    _light0 = lookAtMatrix * light0.position( );
    _light1 = lookAtMatrix * light1.position( );
    
    modelViewMatrix = glm::translate(lookAtMatrix, teapot.position);
    normalMatrix = glm::inverseTranspose(modelViewMatrix);
    shaderProgram.activate( );
    activateUniforms(_light0, _light1, teapot.material);
    teapot.draw( );
    
    int mbFlags = mouseButtonFlags( ); //information from mouse (left mouse button by default, other two buttons are diff)
    std::tuple<int, int> mousePosition = mouseCurrentPosition( );
    std::tuple<int, int> prevMousePosition = mousePreviousPosition( );
     
    if(mbFlags == MOUSE_BUTTON_LEFT){ //information code to see if mouse button is down example // mouse position is a tuple use get for 1st and 2nd item in tuple
      std::cerr << "Left mouse button is down" << std::endl;
        
        /////////////////////////////////////////
        float windowsize_width = 600; //float(std::get<0>(w));
        float windowsize_height = 600; //float(std::get<1>(w));
        float mousePosX = std::get<0>(mousePosition);
        float mousePosY = std::get<1>(mousePosition);
        float pmousePosX = std::get<0>(prevMousePosition);
        float pmousePosY = std::get<1>(prevMousePosition);
        
        float winx = (((mousePosX * 2 ) - windowsize_width )/ windowsize_width ) * (2.0 * mainCamera.halfWidth(1));
        float winy = -(((mousePosY * 2 ) - windowsize_height )/ windowsize_height) * (2.0 * mainCamera.halfHeight());
        glm::vec4 wincoords = glm::vec4(winx, winy, mainCamera.near, 0); // image plane coords then go to sphere coord
        glm::mat4 lookAt;
        mainCamera.lookAtMatrix(lookAt);
        glm::vec4 cameraSpaceC = lookAt * wincoords; //Current Coords
        glm::vec3 Pend = normalize(shoemake(wincoords, 1));
        std::cout <<"Winx: "<< winx <<" Winy: " << winy << std::endl;
        std::cout <<"P: " <<to_string(cameraSpaceC) <<"Pend: "<< to_string(Pend) << std::endl;
        
        winx = (((pmousePosX * 2 ) - windowsize_width )/ windowsize_width ) * (2.0 * mainCamera.halfWidth(1));
        winy = -(((pmousePosY * 2 ) - windowsize_height )/ windowsize_height) * (2.0 * mainCamera.halfHeight());
        glm::vec4 pwincoords = glm::vec4(winx, winy, mainCamera.near, 0);
        glm::vec4 cameraSpaceP = lookAt * pwincoords; //Previous Coords
        glm::vec3 Pstart = normalize(shoemake(pwincoords, 1));
        std::cout <<"P: " <<to_string(cameraSpaceP) <<"Pstart: "<< to_string(Pstart) << std::endl;
        std::cout <<"Winx: "<< winx <<" Winy: " << winy << std::endl;
        
        //axis of rotation
        glm::vec3 crossProd = glm::normalize(cross(Pstart, Pend));
        //angle of rotation
        float sDe = dot(Pstart, Pend);//glm::acos(dot(Pstart, Pend));
        std::cout <<"Cross Prod: " << to_string(crossProd) << "Dot Prod: " << sDe << std::endl;
        float magnitude = sqrt(crossProd.x * crossProd.x + crossProd.y * crossProd.y + crossProd.z * crossProd.z);
        float angle = (1.0f/(tan(magnitude/sDe)));
        std::cout << "angle: " << angle << std::endl;
        
        //check when mouse hasn't moved
        if(!isnan(crossProd.x) && !isnan(crossProd.y) && !isnan(crossProd.z))
        {
        //move to origin
        //teapot.position = teapot.position - mainCamera.eyePosition;
        // transform
        modelViewMatrix = glm::rotate(modelViewMatrix, angle, crossProd);
        //glm::mat4 m = glm::rotate(2.0f, crossProd);
        //modelViewMatrix = modelViewMatrix * m;
        //glm::vec4 tmp = m * glm::vec4(teapot.position, 1.0);
        //teapot.position = glm::vec3 (tmp.x, tmp.y, tmp.z);
        // Move everything back
        //teapot.position = teapot.position + mainCamera.eyePosition;
        }
        //modelViewMatrix = glm::rotate(modelViewMatrix, sDe, crossProd);
        //glm::rotate(teapot.position, sDe, crossProd);
        
        //glm::mat3 m = glm::rotate(sDe, crossProd);
        //teapot.position = m * teapot.position;
        
        std::cerr << "Current Teapot position: " << glm::to_string(teapot.position) << std::endl;

      std::cerr << "Current mouse position: " << std::get<0>(mousePosition) << ", " << std::get<1>(mousePosition) << std::endl;
      std::cerr << "Previous mouse position: " << std::get<0>(prevMousePosition) << ", " << std::get<1>(prevMousePosition) << std::endl;
        
    }else if(mbFlags == MOUSE_BUTTON_RIGHT){
      std::cerr << "Right mouse button is down" << std::endl;
      std::cerr << "Current mouse position: " << std::get<0>(mousePosition) << ", " << std::get<1>(mousePosition) << std::endl;
      std::cerr << "Previous mouse position: " << std::get<0>(prevMousePosition) << ", " << std::get<1>(prevMousePosition) << std::endl;
    }

    if(isKeyPressed('Q')){
      end( );      
    }else if(isKeyPressed(GLFW_KEY_EQUAL)){

    }else if(isKeyPressed(GLFW_KEY_MINUS)){

    }else if(isKeyPressed('R')){
      initCamera( );
      initRotationDelta( );
      initLights( );  
      printf("Eye position, up vector and rotation delta reset.\n");
    }else if(isKeyPressed(GLFW_KEY_LEFT)){
      mainCamera.rotateCameraRight( );
    }else if(isKeyPressed(GLFW_KEY_RIGHT)){
      mainCamera.rotateCameraLeft( );
    }else if(isKeyPressed(GLFW_KEY_UP)){
      mainCamera.rotateCameraDown( );
    }else if(isKeyPressed(GLFW_KEY_DOWN)){
      mainCamera.rotateCameraUp( );
    }else if(isKeyPressed('W')){
      light0.rotateUp( );
    }else if(isKeyPressed('S')){
      light0.rotateUp( );
    }else if(isKeyPressed('A')){
      light0.rotateLeft( );
    }else if(isKeyPressed('D')){
      light0.rotateLeft( );
    }else if(isKeyPressed('X')){
      light0.roll( );
    }else if(isKeyPressed('Y')){
      light1.rotateUp( );
    }else if(isKeyPressed('H')){
      light1.rotateUp( );
    }else if(isKeyPressed('G')){
      light1.rotateLeft( );
    }else if(isKeyPressed('J')){
      light1.rotateLeft( );
    }else if(isKeyPressed('N')){
      light1.roll( );
    }else if(isKeyPressed('O')){
      mainCamera.forward( );
    }else if(isKeyPressed('L')){
      mainCamera.backward( );
    }else if(isKeyPressed('K')){
      mainCamera.panLeft( );
    }else if(isKeyPressed(';')){
      mainCamera.panRight( );
    }else if(isKeyPressed('1')){
      light0.toggle( );
    }else if(isKeyPressed('2')){
      light1.toggle( );
    }
    return !msglError( );
  }
    
};


int main(int argc, char* argv[]){
  TeapotTrackballApp app(argc, argv);
  return app();
}



