#include <graphics/shape.h>
#include "engine/engine.h"
#include "graphics/sprite.h"
#include "graphics/texture.h"
#include "engine/transform.h"
#include "physics/physics.h"
#include "engine/entity.h"
#include "utilities/time_utility.h"
#include "engine/input.h"


struct PlayerData
{
    neko::Entity playerEntity;
    int contactNmb;
    b2Body* playerBody = nullptr;
};

class PlatformerEngine : public neko::MainEngine
{
public:
    void Init() override
    {
        config.bgColor = sf::Color::White;
        MainEngine::Init();
        physicsManager.Init();
        //Player Initialization
        {
            playerData.playerEntity = 1;
            const auto playerPos = sf::Vector2f(200, 200);

            transformManager.positions.push_back(playerPos);
            transformManager.scales.emplace_back(1,1);
            transformManager.angles.push_back(0.0f);

            auto* texture = textureManager.LoadTexture("data/sprites/hero/jump/hero1.png");
            spriteManager.AddSprite(texture);

            const auto physicsSize = neko::pixel2meter(sf::Vector2f(texture->getSize()));
            b2BodyDef bodyDef;
            bodyDef.position = neko::pixel2meter(playerPos);
            bodyDef.fixedRotation = true;
            bodyDef.type = b2_dynamicBody;

            b2PolygonShape playerBox[2];
            playerBox[0].SetAsBox(physicsSize.x/2.0f, physicsSize.y/2.0f);

            const float footOffset = 5.0f;
            const b2Vec2 footPoints[3] = {
                    b2Vec2(0,physicsSize.y/2.0f),
                    b2Vec2(neko::pixel2meter(footOffset),physicsSize.y/2.0f + neko::pixel2meter(footOffset)),
                    b2Vec2(neko::pixel2meter(-footOffset),physicsSize.y/2.0f + neko::pixel2meter(footOffset)),
            };
            playerBox[1].Set(footPoints, 3);

            b2FixtureDef fixtureDef[2];

            neko::Collider mainPlayerCollider{};
#ifdef __neko_dbg__
            neko::ShapeDef shapeDef;
			shapeDef.fillColor = sf::Color::Transparent;
			shapeDef.outlineColor = sf::Color::Green;
			shapeDef.outlineThickness = 1.0f;
			auto mainPlayerShapeIndex = shapeManager.AddBox(playerPos, neko::meter2pixel(b2Vec2(physicsSize.x/2.0f, physicsSize.y/2.0f)), shapeDef);
			mainPlayerCollider.shapeIndex = mainPlayerShapeIndex;
#endif
            fixtureDef[0].shape = &playerBox[0];
            mainPlayerCollider.entity = playerData.playerEntity;
            physicsManager.colliders.push_back(mainPlayerCollider);

            neko::Collider footCollider{};
            footCollider.entity = playerData.playerEntity;
            fixtureDef[1].shape = &playerBox[1];
            fixtureDef[1].isSensor = true;
#ifdef __neko_dbg__

            sf::Vector2f points[3];
			for(int i = 0; i < 3; i++)
            {
			    points[i] = neko::meter2pixel(footPoints[i]);
            }

            //auto footShapeIndex = shapeManager.AddPolygon(neko::meter2pixel(footPoints[0]), points, 3, shapeDef);
            //footCollider.shapeIndex = footShapeIndex;
#endif
            physicsManager.colliders.push_back(footCollider);

            playerData.playerBody = physicsManager.CreateBody(bodyDef, fixtureDef, 2);
        }

        physicsTimer.period = config.physicsTimeStep;
        physicsTimer.Reset();

        tiledMap.Init("data/tilemap/platformer.json", textureManager, &physicsManager);
    }




    void Update() override
    {
        MainEngine::Update();
        keyboardManager.Update();
        physicsTimer.Update(dt.asSeconds());
        if(physicsTimer.IsOver())
        {
            physicsManager.Update();
            physicsTimer.time += physicsTimer.period;
        }
        tiledMap.PushCommand(graphicsManager);
        //Player management
        {
            //Jumping
            if(playerData.contactNmb > 0 && keyboardManager.IsKeyDown(sf::Keyboard::Space))
            {
                const auto playerVelocity = playerData.playerBody->GetLinearVelocity();
                playerData.playerBody->SetLinearVelocity(b2Vec2(playerVelocity.x, -jumpVelocity));
            }
            //Move horizontal
            {
                int move = 0;
                move -= keyboardManager.IsKeyHeld(sf::Keyboard::Left);
                move += keyboardManager.IsKeyHeld(sf::Keyboard::Right);
                const auto playerVelocity = playerData.playerBody->GetLinearVelocity();
                playerData.playerBody->SetLinearVelocity(b2Vec2(move*moveVelocity, playerVelocity.y));
            }

            transformManager.CopyPositionsFromPhysics2d(physicsManager, 0, 1);
            spriteManager.CopyTransformPosition(transformManager, 0, 1);
            spriteManager.PushCommands(graphicsManager, 0, 1);
        }

    }

    void Destroy() override
    {
        physicsManager.Destroy();
        MainEngine::Destroy();
    }

    void OnBeginContact(const neko::Collider* colliderA, const neko::Collider* colliderB) override
    {
        if(colliderA->entity == playerData.playerEntity && colliderA->fixture->IsSensor())
        {
            playerData.contactNmb++;
        }
        if (colliderB->entity == playerData.playerEntity && colliderB->fixture->IsSensor())
        {
            playerData.contactNmb++;
        }
    }
    void OnEndContact(const neko::Collider* colliderA, const neko::Collider* colliderB) override
    {
        if (colliderA->entity == playerData.playerEntity && colliderA->fixture->IsSensor())
        {
            playerData.contactNmb--;
        }
        if (colliderB->entity == playerData.playerEntity && colliderB->fixture->IsSensor())
        {
            playerData.contactNmb--;
        }
    }
protected:
    neko::Timer physicsTimer{0.0f,0.0f};
    neko::SpriteManager spriteManager;
    neko::TextureManager textureManager;
    neko::Transform2dManager transformManager;
    neko::Physics2dManager physicsManager;
    neko::KeyboardManager keyboardManager;
    neko::ShapeManager shapeManager;
    neko::TiledMap tiledMap;
    PlayerData playerData = {};
    static const size_t platformsNmb = 3;
    const float jumpVelocity = 8.0f;
    const float moveVelocity = 2.0f;
};

int main(int argc, char** argv)
{
    PlatformerEngine engine;
    engine.Init();
    engine.EngineLoop();
    return EXIT_SUCCESS;
}