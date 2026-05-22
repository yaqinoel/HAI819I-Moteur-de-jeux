#pragma once


#include <iostream>
#include <string>
#include <array>
#include <vector>
#include "external/stb_image/stb_image.h"

#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <cstring>



struct Color {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    Color(unsigned char a, unsigned char b, unsigned char c)
        : r(a),
        g(b),
        b(c)
        {}
};

class Texture {
protected:

    std::vector< unsigned char > data;
    GLuint _texture_id;
    bool __synchronized = false;

    Color getPixel(size_t u, size_t v) const;

    void unsynchronize(){
        
        if (!__synchronized) return;

        glDeleteTextures(1, &_texture_id);

        __synchronized = false;
    }

public:

    int width, height;
    int nbChannels;
    bool sharp = false;
    float scale = 1.0f;
    std::string path;

    Texture(const std::string path, bool isPixelArt = false){
        sharp = isPixelArt;
        loadTexture(path);
    }

    Texture(const Texture & other) : data(other.data),
    width(other.width),
    height(other.height),
    nbChannels(other.nbChannels)
    {
        sharp = other.sharp;
        __synchronized = false;
        if (!data.empty())
            synchronize();
    }

    Texture() = default;
    Texture(Texture &&) = default;

    Texture& operator=(const Texture & other) {
        if (this != &other) {
            if (__synchronized) unsynchronize();
            data = other.data;
            width = other.width;
            height = other.height;
            nbChannels = other.nbChannels;
            sharp = other.sharp;
            scale = other.scale;
            __synchronized = false;
            if (!data.empty())
                synchronize();
        }
        return *this;
    }
    
    ~Texture(){if (__synchronized) unsynchronize();}

    void loadTexture(const std::string path){
        this->path = path;
        unsigned char* new_data = stbi_load(path.c_str(), &width, &height, &nbChannels, 0);
        if (!new_data){
            std::cout << "[Texture] Problème du chargement de la texture à " << path << " (impossible de charger les données)" << std::endl;
            return;
        }

        data.resize(width * height * nbChannels);
        memcpy(data.data(), new_data, width * height * nbChannels);
        stbi_image_free(new_data);
        synchronize();
    }

    Color getPixelSafe(size_t u, size_t v) const;

    Color getPixelSafe(float x, float y) const;

    Color operator() (size_t i, size_t j) {

        return getPixelSafe(i, j);
    }

    void setPixelArt(bool p){
        sharp = p;
    }

    static Texture createFlatColorTexture(const Color & val){
        Texture t;
        t.data = {val.r, val.g, val.b};
        t.width = t.height = 1;
        t.nbChannels = 3;
        t.synchronize();
        return t;
    }

    GLuint getTextureId() const{ return _texture_id;};


    void synchronize(){
        
        if (__synchronized) return;
        glGenTextures(1, &_texture_id);
        glBindTexture(GL_TEXTURE_2D, _texture_id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        if(sharp){
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        }
        else{
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        }
        GLenum format = GL_RGB;

        if (nbChannels == 4)
            format = GL_RGBA;
        else if (nbChannels == 3)
            format = GL_RGB;
        else if (nbChannels == 1)
            format = GL_RED;

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data.data());
        glGenerateMipmap(GL_TEXTURE_2D); 
    }


    void bind(GLuint slot = 0) const {
        glActiveTexture(GL_TEXTURE0+slot);
        glBindTexture(GL_TEXTURE_2D, _texture_id);
    }
};
