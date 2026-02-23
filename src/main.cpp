#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/LevelSelectLayer.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <Geode/modify/CreatorLayer.hpp>
#include <Geode/modify/LevelBrowserLayer.hpp>
#include <Geode/modify/EditorPauseLayer.hpp>
#include <Geode/modify/LevelSearchLayer.hpp>
#include <Geode/modify/EditLevelLayer.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>
#include <Geode/modify/CCEGLView.hpp>
#include <imgui-cocos.hpp>
#ifdef GEODE_IS_WINDOWS
#include "gui.hpp"
#endif
#include "gui_mobile.hpp"
#include "config.hpp"
#include "hacks.hpp"

void CheckDir(const std::filesystem::path &path)
{
    if (!std::filesystem::is_directory(path) || !std::filesystem::exists(path))
    {
        std::filesystem::create_directory(path);
    }
}

$execute {
    auto *mod = geode::Mod::get();
    ImGuiCocos::get().setForceLegacy(mod->getSettingValue<bool>("legacy-render"));

	auto& config = Config::get();
    config.load(fileDataPath);

    float speedhack_value = config.get<float>("speedhack_value", 1.f);
    speedhack_value = std::clamp(speedhack_value, 0.25f, 500.f);
    config.set<float>("speedhack_value", speedhack_value);

    float tps_value = config.get<float>("tps_value", 60.f);
    tps_value = std::clamp(tps_value, 30.f, 240.f);
    config.set<float>("tps_value", tps_value);

    CheckDir(folderMacroPath);
    CheckDir(Config::get().get<std::filesystem::path>("showcases_path", folderPath / "Showcases"));
}

static bool inited = false;

void setupButton(cocos2d::CCNode* parent, const cocos2d::CCPoint& position, const std::string& buttonID, const std::string& childID = "") {
    auto sprite = cocos2d::CCSprite::create(Config::get().get<bool>("invisible_ui_button", false) ? "GDH_buttonInvisible.png"_spr : "GDH_buttonUI.png"_spr);
    sprite->setScale(0.85f);
    auto myButton = geode::prelude::CCMenuItemExt::createSpriteExtra(sprite, [](CCMenuItemSpriteExtra* sender) {
        auto& config = Config::get();
        if (!config.get<bool>("license_accepted", false)) {
            FLAlertLayer::create("GDH Beta Testing", "Note: GDH is currently in beta testing, so some elements may be unstable/unfinished\n\nIn case of any issues/crashes, please report the problem in the GDH issues section on GitHub", "OK")->show();
            config.set<bool>("license_accepted", true);
            return;
        }
        HacksLayer::create()->show();
    });
    myButton->setID(childID);

    if (childID.empty()) {
        myButton->setPosition(position);
        
        auto menu = cocos2d::CCMenu::create();
        menu->setPosition(0, 0);
        menu->addChild(myButton);
        parent->addChild(menu);
    }
    else {
        auto menu = parent->getChildByID(childID);
        if (menu) {
            menu->addChild(myButton);
            menu->updateLayout();
        }
    }

}

class $modify(MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;
            
        if (!inited) {
            auto& hacks = Hacks::get();
            hacks.Init();
            hacks.loadKeybinds();

            #ifdef GEODE_IS_WINDOWS
            auto& gui = Gui::get();
            ImGuiCocos::get().setup([&gui] {
                gui.Init();
            }).draw([&gui] {
                gui.Render();
            });
            #endif

            inited = true;
        }

        #ifdef GEODE_IS_ANDROID

        setupButton(this, {0, 0}, "hacks-button"_spr, "bottom-menu");

        #endif

        return true;
    }
};

#ifdef GEODE_IS_ANDROID

class $modify(PauseLayer) {
    void customSetup() {
        PauseLayer::customSetup();

        static geode::Mod* nodeId = geode::Loader::get()->getLoadedMod("geode.node-ids");
        static bool hasNodeIdMod = nodeId != nullptr && nodeId->isEnabled();

        auto top = cocos2d::CCDirector::sharedDirector()->getScreenTop();
        if (hasNodeIdMod)
            setupButton(this, {40, top - 45.f}, "hacks-button"_spr, "left-button-menu");
        else
            setupButton(this, {40, top - 45.f}, "hacks-button"_spr);
    }
};

class $modify(LevelInfoLayer) {
    bool init(GJGameLevel *level, bool challenge) {
        if (!LevelInfoLayer::init(level, challenge)) return false;

        static geode::Mod* nodeId = geode::Loader::get()->getLoadedMod("geode.node-ids");
        static bool hasNodeIdMod = nodeId != nullptr && nodeId->isEnabled();

        auto top = cocos2d::CCDirector::sharedDirector()->getScreenTop();
        if (hasNodeIdMod) 
            setupButton(this, {0, 0}, "hacks-button"_spr, "left-side-menu");

        return true;
    }
};

class $modify(LevelSelectLayer) {
    bool init(int page) {
        if (!LevelSelectLayer::init(page)) return false;
        setupButton(this, {30, cocos2d::CCDirector::sharedDirector()->getScreenTop() - 70.f}, "hacks-button"_spr);
        return true;
    }
};

class $modify(CreatorLayer) {
    bool init() {
        if (!CreatorLayer::init()) return false;

        static geode::Mod* nodeId = geode::Loader::get()->getLoadedMod("geode.node-ids");
        static bool hasNodeIdMod = nodeId != nullptr && nodeId->isEnabled();

        auto top = cocos2d::CCDirector::sharedDirector()->getScreenTop();
        if (hasNodeIdMod) 
            setupButton(this, {0, 0}, "hacks-button"_spr, "bottom-left-menu");
        else 
            setupButton(this, {30, cocos2d::CCDirector::sharedDirector()->getWinSize().height / 2}, "hacks-button"_spr);
        
        return true;
    }
};

class $modify(LevelBrowserLayer) {
    bool init(GJSearchObject* p0) {
        if (!LevelBrowserLayer::init(p0)) return false;
        setupButton(this, {30, (cocos2d::CCDirector::sharedDirector()->getWinSize().height / 2) - 45.f}, "hacks-button"_spr);
        return true;
    }
};

class $modify(EditorPauseLayer) {
    bool init(LevelEditorLayer* p0) {
        if (!EditorPauseLayer::init(p0)) return false;

        static geode::Mod* nodeId = geode::Loader::get()->getLoadedMod("geode.node-ids");
        static bool hasNodeIdMod = nodeId != nullptr && nodeId->isEnabled();

        auto top = cocos2d::CCDirector::sharedDirector()->getScreenTop();
        if (hasNodeIdMod) 
            setupButton(this, {0, 0}, "hacks-button"_spr, "guidelines-menu");
        else 
            setupButton(this, {cocos2d::CCDirector::sharedDirector()->getScreenRight() - 25.f, 
                                cocos2d::CCDirector::sharedDirector()->getScreenTop() - 25.f}, "hacks-button"_spr);
        return true;
    }
};

class $modify(LevelSearchLayer) {
    bool init(int p0) {
        if (!LevelSearchLayer::init(p0)) return false;
        setupButton(this, {30, cocos2d::CCDirector::sharedDirector()->getWinSize().height / 2}, "hacks-button"_spr);
        return true;
    }
};


class $modify(EditLevelLayer) {
    bool init(GJGameLevel *p0) {
        if (!EditLevelLayer::init(p0)) return false;
        static geode::Mod* nodeId = geode::Loader::get()->getLoadedMod("geode.node-ids");
        static bool hasNodeIdMod = nodeId != nullptr && nodeId->isEnabled();

        if (hasNodeIdMod) 
            setupButton(this, {0, 0}, "hacks-button"_spr, "level-actions-menu");
        return true;
    }
};

#endif


#ifdef GEODE_IS_WINDOWS
#include <Geode/modify/CCKeyboardDispatcher.hpp>
#include "keyMapping.hpp"

// onGLFWKeyCallback is inlined in 2.208 - use CCKeyboardDispatcher instead
// enumKeyCodes on Windows = VK codes, convert to GLFW via KeyMappingUtils
class $modify(GDHKeyboardHook, cocos2d::CCKeyboardDispatcher) {
    bool dispatchKeyboardMSG(cocos2d::enumKeyCodes key, bool down, bool repeat, double time) {
        if (inited && down) {
            auto& gui = Gui::get();
            auto& hacks = Hacks::get();
            auto& io = ImGui::GetIO();

            // Convert VK (enumKeyCodes) to GLFW key code
            int glfwKey = KeyMappingUtils::GetGLFWFromWinAPI((int)key);

            if (gui.m_keybindMode && gui.m_waitingForBindKey && !repeat) {
                gui.m_keyToSet = glfwKey;
                gui.m_waitingForBindKey = false;
            }
            else if (!gui.m_keybindMode) {
                if (glfwKey == gui.m_toggleKey && !repeat) {
                    gui.Toggle();
                }
                if (!io.WantCaptureKeyboard) {
                    hacks.toggleKeybinds(glfwKey);
                }
            }
        }
        return CCKeyboardDispatcher::dispatchKeyboardMSG(key, down, repeat, time);
    }
};
#endif