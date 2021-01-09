#include "gl_util.h"

#include <iostream>

void print_log(GLuint object){
    GLint log_length = 0;
    if(glIsShader(object)){
        glGetShaderiv(object, GL_INFO_LOG_LENGTH, &log_length);
    }else if(glIsProgram(object)){
        glGetProgramiv(object, GL_INFO_LOG_LENGTH, &log_length);
    }else {
        std::cerr << "printlog: Not a shader or a program" << std::endl;
        return;
    }

    std::string log(log_length, '\0');

    if(glIsShader(object)){
        glGetShaderInfoLog(object, log_length, NULL, &log[0]);
    }else if(glIsProgram(object)){
        glGetProgramInfoLog(object, log_length, NULL, &log[0]);
    }
    std::cerr << "log from object " << object << ":" << std::endl;
    std::cerr << log << std::endl;
}
