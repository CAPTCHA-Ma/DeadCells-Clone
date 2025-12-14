#pragma once
#include "cocos2d.h"

class Resprite : public cocos2d::Sprite
{
public:
    
    static Resprite* create(const std::string& filename,
        const std::string& paletteFile,
        const std::string& normalMapFile); 

    bool initWithFileAndPalette(const std::string& filename,
        const std::string& paletteFile,
        const std::string& normalMapFile);

    void setPalette(const std::string& paletteFile);
    void setNormalMap(const std::string& normalMapFile); 
    void initShader();

protected:
    cocos2d::Texture2D* _paletteTexture = nullptr;
    cocos2d::Texture2D* _normalTexture = nullptr; 
};