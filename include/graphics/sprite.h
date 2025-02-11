#pragma once

/*
 MIT License

 Copyright (c) 2017 SAE Institute Switzerland AG

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 */

#include <vector>
#include <SFML/Graphics/Sprite.hpp>
#include "engine/globals.h"


namespace neko
{
class Transform2dManager;

class GraphicsManager;

class SpriteManager
{
public:
    SpriteManager();

    int AddSprite(const sf::Texture* texture);

    sf::Sprite* GetSpriteAt(int spriteIndex);

    void CopyTransformPosition(Transform2dManager& transformManager, size_t start = 0, size_t length = InitEntityNmb);

    void CopyTransformScales(Transform2dManager& transformManager, size_t start = 0, size_t length = InitEntityNmb);

    void CopyTransformAngles(Transform2dManager& transformManager, size_t start = 0, size_t length = InitEntityNmb);

    void PushCommands(GraphicsManager* graphicsManager, size_t start = 0, size_t length = InitEntityNmb);

private:
    std::vector<sf::Sprite> sprites[2];
};
}
