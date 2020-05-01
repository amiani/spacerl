#include "systems/ship_system.h"
#include "src/control/controller.h"
#include "src/utils.h"
#include "components/goal_component.h"
#include "components/weapon_component.h"
#include "components/observer_component.h"
#include "components/ship_control_component.h"
#include "components/weapon_control_component.h"
#include "components/sprite_component.h"
#include "components/health_component.h"
#include "components/background_camera_component.h"
#include "components/body_component.h"
#include "components/detector_component.h"
#include "components/hardpoints_component.h"
#include "components/shape_component.h"
#include "components/cooldown_component.h"
#include "halley/src/engine/entity/include/halley/entity/components/transform_2d_component.h"

using namespace Halley;

class ShipSystem final : public ShipSystemBase<ShipSystem> {
public:
  void init(){

    auto player1 = spawnPlayerShip(shipService->getShip("large_grey"), true, true);
    auto player2 = spawnPlayerShip(shipService->getShip("large_grey"), true, false);
  }

  void update(Time t) {}

  EntityRef spawnShip(const ShipConfig& config) {
    auto& world = getWorld();

    auto& laserNode = getResources().get<ConfigFile>("gameplay/weapons")->getRoot()["weapons"].asSequence()[0];
    auto laserConfig = WeaponConfig();
    laserConfig.load(laserNode);
    auto laser = world.createEntity()
      .addComponent(WeaponControlComponent())
      .addComponent(CooldownComponent(laserConfig.cooldown, 0))
      .addComponent(WeaponComponent(laserConfig));

    cp::Float moment = cp::momentForCircle(config.mass, 0, config.radius);
    auto body = std::make_shared<cp::Body>(config.mass, moment);
    auto randx = 1920 * ((double)rand()/RAND_MAX*2.0-1.0);
    auto randy = 1080 * ((double)rand()/RAND_MAX*2.0-1.0);
    body->setPosition(cp::Vect(randx, randy));
    auto shape = std::make_shared<cp::CircleShape>(body, config.radius);
    shape->setFilter({
      .categories = PLAYERHULL,
      .mask = GOAL | ASTEROID });
    shape->setCollisionType(PLAYERSHIPBODY);
    body->setVelocityUpdateFunc(velocityUpdate);
    auto detector = std::make_shared<cp::CircleShape>(body, config.detectorRadius);
    detector->setFilter({
      .categories = PLAYERDETECTOR,
      .mask = GOAL | ASTEROID });
    detector->setCollisionType(DETECTORBODY);
    detector->setSensor(true);
    auto sprite = Sprite()
      .setImage(getResources(), config.image)
      .setPivot(Vector2f(.5f, .5f));
    auto h = Hardpoint();
    h.weaponId = laser.getEntityId();
    h.offset = Vector2f(25, 0);
    auto& ship = world.createEntity()
      .addComponent(BodyComponent(body))
      .addComponent(ShapeComponent(shape))
      .addComponent(DetectorComponent(detector, std::vector<EntityId>()))
      .addComponent(ObserverComponent(0, 0))
      .addComponent(HealthComponent(100))
      .addComponent(SpriteComponent(sprite, 0, 1))
      .addComponent(HardpointsComponent(std::vector<Hardpoint>{h}))
      .addComponent(Transform2DComponent());

    laser.addComponent(Transform2DComponent(ship.getComponent<Transform2DComponent>(), Vector2f(25, 0)));
    return ship;
  }

  std::function<void(cp::Body&, cp::Vect, cp::Float, cp::Float)> velocityUpdate = [](cp::Body& body, cp::Vect gravity, cp::Float damping, cp::Float dt) {
    body.updateVelocity(gravity, .99, dt);
  };

  EntityRef spawnPlayerShip(const ShipConfig& config, bool isRL, bool withCamera) {
    auto ship = spawnShip(config);
    if (withCamera) {
      auto bgSprite = Sprite().setImage(getResources(), "goldstartile.jpg");
      ship.addComponent(BackgroundCameraComponent(1, Colour4f(0.0f, 0.0f, 0.0f), 1, 0, bgSprite));
    }
    if (isRL) {
      auto c = controllerService->makeRLController();
      ship.addComponent(ShipControlComponent(c));
    } else {
      auto& device = inputService->getInput();
      auto& transform = ship.getComponent<Transform2DComponent>();
      auto c = controllerService->makeInputController(device, transform);
      ship.addComponent(ShipControlComponent(c));
    }
    return ship;
  }

  EntityRef spawnGoal(cp::Vect& playerPosition) {
    double randx, randy;
    cp::Vect position;
    do {
      randx = 1920 * (((double)rand()/RAND_MAX)*2.0-1.0);
      randy = 1080 * (((double)rand()/RAND_MAX)*2.0-1.0);
      position = cp::Vect(randx, randy);
    } while(cp::Vect::dist(position, playerPosition) < 100 || cp::Vect::dist(position, playerPosition) > 400);
    auto body = std::make_shared<cp::Body>(1, 1);
    body->setPosition(position);
    auto shape = std::make_shared<cp::CircleShape>(body, 50);
    shape->setSensor(true);
    shape->setFilter({
      .categories = GOAL,
      .mask = PLAYERDETECTOR | PLAYERHULL
    });
    shape->setCollisionType(GOALBODY);
    auto sprite = Sprite()
      .setImage(getResources(), "target.png") .setPivot(Halley::Vector2f(.5, .5))
      .scaleTo(Halley::Vector2f(100, 100));
    return getWorld().createEntity("goal")
      .addComponent(BodyComponent(body))
      .addComponent(Transform2DComponent())
      .addComponent(ShapeComponent(shape))
      .addComponent(SpriteComponent(sprite, 0, 1));
  }
};

REGISTER_SYSTEM(ShipSystem)