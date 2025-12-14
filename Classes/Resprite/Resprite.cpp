#include "Resprite.h"
#include "cocos2d.h"
#include "renderer/backend/Device.h" 

USING_NS_CC;

Resprite* Resprite::create(const std::string& filename, const std::string& paletteFile, const std::string& normalMapFile)
{
    auto sprite = new (std::nothrow) Resprite();
    if (sprite && sprite->initWithFile(filename)) {
        
        if (sprite->initWithFileAndPalette(filename, paletteFile, normalMapFile)) {
            sprite->autorelease();
            return sprite;
        }
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

bool Resprite::initWithFileAndPalette(const std::string& filename, const std::string& paletteFile, const std::string& normalMapFile)
{
    setPalette(paletteFile);

    setNormalMap(normalMapFile);

    initShader();

    return true;
}

void Resprite::setNormalMap(const std::string& normalMapFile)
{
    _normalTexture = Director::getInstance()->getTextureCache()->addImage(normalMapFile);
    if (!_normalTexture) {
        CCLOG("Error: Cannot load normal map %s", normalMapFile.c_str());
        return;
    }

    Texture2D::TexParams params;
    params.minFilter = backend::SamplerFilter::NEAREST;
    params.magFilter = backend::SamplerFilter::NEAREST;
    params.sAddressMode = backend::SamplerAddressMode::CLAMP_TO_EDGE;
    params.tAddressMode = backend::SamplerAddressMode::CLAMP_TO_EDGE;
    _normalTexture->setTexParameters(params);
}

void Resprite::initShader()
{
    auto fileUtils = FileUtils::getInstance();
    std::string vertSource = fileUtils->getStringFromFile("Shaders/DeadCells.vert");
    std::string fragSource = fileUtils->getStringFromFile("Shaders/DeadCells.frag");

    auto device = backend::Device::getInstance();
    auto program = device->newProgram(vertSource, fragSource);
    auto programState = new (std::nothrow) backend::ProgramState(program);

    Texture2D::TexParams mainParams;
    mainParams.minFilter = backend::SamplerFilter::NEAREST;
    mainParams.magFilter = backend::SamplerFilter::NEAREST;
    mainParams.sAddressMode = backend::SamplerAddressMode::CLAMP_TO_EDGE;
    mainParams.tAddressMode = backend::SamplerAddressMode::CLAMP_TO_EDGE;
    if (this->getTexture()) {
        this->getTexture()->setTexParameters(mainParams);
    }

    if (_paletteTexture) {
        auto loc = programState->getUniformLocation("u_paletteTexture");
        programState->setTexture(loc, 1, _paletteTexture->getBackendTexture());
    }

    if (_normalTexture) {
        auto loc = programState->getUniformLocation("u_normalMap");
        programState->setTexture(loc, 2, _normalTexture->getBackendTexture());
    }

    setProgramState(programState);
    CC_SAFE_RELEASE(programState);
    CC_SAFE_RELEASE(program);
}

void Resprite::setPalette(const std::string& paletteFile)
{
    _paletteTexture = Director::getInstance()->getTextureCache()->addImage(paletteFile);
    if (!_paletteTexture) {
        CCLOG("Error: Cannot load palette texture %s", paletteFile.c_str());
        return;
    }

    Texture2D::TexParams params;
    params.minFilter = backend::SamplerFilter::NEAREST;
    params.magFilter = backend::SamplerFilter::NEAREST;
    params.sAddressMode = backend::SamplerAddressMode::CLAMP_TO_EDGE;
    params.tAddressMode = backend::SamplerAddressMode::CLAMP_TO_EDGE;
    _paletteTexture->setTexParameters(params);

    auto programState = getProgramState();
    if (programState) {
        auto paletteLocation = programState->getUniformLocation("u_paletteTexture");

        programState->setTexture(paletteLocation, 1, _paletteTexture->getBackendTexture());
    }
}