#include "smjs.object-collab/include/CustomObject.hpp"
#include <Geode/Geode.hpp>
#include <Geode/binding/PlayerObject.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <smjs.object-collab/include/object_collab.hpp>

using namespace geode::prelude;
using namespace object_collab::prelude;

class $object(PlayerSpeedTrigger, EffectGameObject) {
public:
    static PlayerSpeedTrigger* create(ObjectInfo* info) {
        // The mod will internally handle auto releasing and calling init.
        return new PlayerSpeedTrigger(info);
    }

    static PopupOptions getEditSpecialConfig(const Selected& selected) {
        // Sets up a UI with a size of 270x120 with a standard no multi activate toggle and a single toggle menu to switch active on and off.
        return PopupConfig::builder()
            .width(270)
            .height(120)
            .title("Player Speed Trigger")
            .info(InfoPopup::builder()
                .title("Auto Jump UFO Portal Help")
                .description("")
                .build())
            .noMultiActivateToggle(true)
             .menu(AxisLayoutMenu::builder()
            .axis(Axis::Row)
            .gap(20)
            .crossAxisAlignment(CrossAxisAlignment::Center)
            .menu(NumericMenu::builder()
                .id("Player-Speed"_spr)
                .title("Player Speed")
                .inputType(NumericMenu::InputType::Slider)
                .min(0.0)
                .max(10.0)
                .precision(4)
                 .stepSize(0.1)
                .onValue([](const float value, const Selected& selected, Popup* popup) {
                applyValueToSelected(selected, &PlayerSpeedTrigger::m_playerspeed, value);
                })
                .currentValue([](const Selected& selected, Popup* popup) {
                    return getCommonValueOrDefault(selected, &PlayerSpeedTrigger::m_playerspeed);
                })
                .build())
             .build())
            .build();
    }
    bool m_active;
    float m_playerspeed;

    PlayerSpeedTrigger(ObjectInfo* info): CustomObject(info, ObjectTraits::builder()
        .gameObjectType(GameObjectType::Modifier)
        .build()) { }

    std::vector<std::string> getObjectDetails() override {
        return DetailsBuilder::builder()
            // Since this is a bool it will automatically translate the value to either "Yes" or "No"
            .field("Active", m_active)
            .field("Player Speed",m_playerspeed)
            .build();
    }


void triggerObject(GJBaseGameLayer* layer, const int uniqueID, const gd::vector<int>* remapKeys) {
    if (m_active) {
        CustomObject::triggerObject(layer, m_uniqueID, remapKeys);
        layer->m_player1->m_playerSpeed = m_playerspeed;
        layer->m_player2->m_playerSpeed = m_playerspeed;
      }
    }
};
class $modify(PlayerObject){

  void updateTimeMod(float speed, bool noParticle) {
       m_playerSpeed = speed;
    if (speed == 1.0f) {
        m_yStart = 11.1800318;
        m_gravity = 0.958199024;
        m_speedMultiplier = 5.77000189;
   }     else if (speed  == 1.2f) {
        m_yStart = 11.420032;
       m_gravity = 0.957199;
        m_speedMultiplier = 5.870002;
   } else if (speed == 1.4f || speed == 1.5f) {
        m_yStart = 11.230032;
        m_gravity = 0.961199;
        m_speedMultiplier = 6.000002;
   } else if (speed > 1.6f) {
        m_yStart = 11.210032;
        m_gravity = 0.970099;
        m_speedMultiplier = 6.000002;
  }
    return PlayerObject::updateTimeMod(speed,noParticle);
  }
};

$on_mod(Loaded) {
    ObjectAPI::registerObject(ObjectInfo::builder()
        .id("Player Speed Trigger"_spr)
        .sprite("Player-Speed-Trigger.png"_spr)
        .editorTab(EditorTab::Triggers)
        .construction(ComplexObject::builder()
            .factory(PlayerSpeedTrigger::create)
            // Sets the custom property on key 150 (Which is not by default saved by EffectGameObject) with a default value of true.
            // These properties will automate saving, loading & updating values and handling their defaults when no common value can be found in a selection.
            .customProperties({
                PropertyInterface::from(150, &PlayerSpeedTrigger::m_active, true),
                PropertyInterface::from(300, &PlayerSpeedTrigger::m_playerspeed, 0),
            })
            .build())
        .editSpecial(PlayerSpeedTrigger::getEditSpecialConfig)
        .build());
        ObjectAPI::registerObject(ObjectInfo::builder()
        .id("Fake Speed Portal"_spr)
        .sprite("boost.png"_spr)  
        .construction(QuickObject::builder() // Optional
            .objectType(GameObjectType::Decoration) // Optional
            .defaultZLayer(ZLayer::Default) // Optional
            .defaultZOrder(2) // Optional
            .build())
        .editorTab(EditorTab::Decorations) // Optional
        .build());
}