#include <allegro5/allegro.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_ttf.h>
#include <chrono>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>

#include "Allegro5Exception.hpp"
#include "GameEngine.hpp"
#include "IScene.hpp"
#include "LOG.hpp"
#include "Point.hpp"
#include "Resources.hpp"

#include <string>
#include <map>
#include <allegro5/allegro.h>


int name_to_keycode(const std::string& name) {
    static const std::map<std::string, int> keyMap = {
        // A~Z
        {"A", ALLEGRO_KEY_A}, {"B", ALLEGRO_KEY_B}, {"C", ALLEGRO_KEY_C}, {"D", ALLEGRO_KEY_D},
        {"E", ALLEGRO_KEY_E}, {"F", ALLEGRO_KEY_F}, {"G", ALLEGRO_KEY_G}, {"H", ALLEGRO_KEY_H},
        {"I", ALLEGRO_KEY_I}, {"J", ALLEGRO_KEY_J}, {"K", ALLEGRO_KEY_K}, {"L", ALLEGRO_KEY_L},
        {"M", ALLEGRO_KEY_M}, {"N", ALLEGRO_KEY_N}, {"O", ALLEGRO_KEY_O}, {"P", ALLEGRO_KEY_P},
        {"Q", ALLEGRO_KEY_Q}, {"R", ALLEGRO_KEY_R}, {"S", ALLEGRO_KEY_S}, {"T", ALLEGRO_KEY_T},
        {"U", ALLEGRO_KEY_U}, {"V", ALLEGRO_KEY_V}, {"W", ALLEGRO_KEY_W}, {"X", ALLEGRO_KEY_X},
        {"Y", ALLEGRO_KEY_Y}, {"Z", ALLEGRO_KEY_Z},

        // 0~9
        {"0", ALLEGRO_KEY_0}, {"1", ALLEGRO_KEY_1}, {"2", ALLEGRO_KEY_2}, {"3", ALLEGRO_KEY_3},
        {"4", ALLEGRO_KEY_4}, {"5", ALLEGRO_KEY_5}, {"6", ALLEGRO_KEY_6}, {"7", ALLEGRO_KEY_7},
        {"8", ALLEGRO_KEY_8}, {"9", ALLEGRO_KEY_9},

        // Arrow keys
        {"UP", ALLEGRO_KEY_UP}, {"DOWN", ALLEGRO_KEY_DOWN},
        {"LEFT", ALLEGRO_KEY_LEFT}, {"RIGHT", ALLEGRO_KEY_RIGHT},

        // Control keys
        {"SPACE", ALLEGRO_KEY_SPACE}, {"ENTER", ALLEGRO_KEY_ENTER},
        {"ESC", ALLEGRO_KEY_ESCAPE}, {"TAB", ALLEGRO_KEY_TAB},
        {"LSHIFT", ALLEGRO_KEY_LSHIFT}, {"RSHIFT", ALLEGRO_KEY_RSHIFT},
        {"LCTRL", ALLEGRO_KEY_LCTRL}, {"RCTRL", ALLEGRO_KEY_RCTRL},
        {"ALT", ALLEGRO_KEY_ALT}, {"BACKSPACE", ALLEGRO_KEY_BACKSPACE},

        // function key F1~F12
        {"F1", ALLEGRO_KEY_F1}, {"F2", ALLEGRO_KEY_F2}, {"F3", ALLEGRO_KEY_F3}, {"F4", ALLEGRO_KEY_F4},
        {"F5", ALLEGRO_KEY_F5}, {"F6", ALLEGRO_KEY_F6}, {"F7", ALLEGRO_KEY_F7}, {"F8", ALLEGRO_KEY_F8},
        {"F9", ALLEGRO_KEY_F9}, {"F10", ALLEGRO_KEY_F10}, {"F11", ALLEGRO_KEY_F11}, {"F12", ALLEGRO_KEY_F12}
    };

    auto it = keyMap.find(name);
    if (it != keyMap.end()) return it->second;
    return 0;  // 0 -> not found
}


Engine::GameEngine::GameEngine():sender(8888){
        //Check System
	    #ifdef _WIN32
        std::cout << "Running on Windows" << std::endl;
	    #else
        std::cout << "Running on Linux/Unix" << std::endl;
	    #endif
        sender.start(); // listen 

    };  
namespace Engine {

    void GameEngine::initAllegro5() {
        if (!al_init()) throw Allegro5Exception("failed to initialize allegro");

        // Initialize add-ons.
        if (!al_init_primitives_addon()) throw Allegro5Exception("failed to initialize primitives add-on");
        al_init_font_addon();
        // Use the code below if you're using a newer version or Allegro5, the code above is for compatibility.
        // if (!al_init_font_addon()) throw Allegro5Exception("failed to initialize font add-on");
        if (!al_init_ttf_addon()) throw Allegro5Exception("failed to initialize ttf add-on");
        if (!al_init_image_addon()) throw Allegro5Exception("failed to initialize image add-on");
        // Enable antialias by linear interpolation.
        al_set_new_bitmap_flags(ALLEGRO_MIN_LINEAR | ALLEGRO_MAG_LINEAR);
        if (!al_install_audio()) throw Allegro5Exception("failed to initialize audio add-on");
        if (!al_init_acodec_addon()) throw Allegro5Exception("failed to initialize audio codec add-on");
        if (!al_reserve_samples(reserveSamples)) throw Allegro5Exception("failed to reserve samples");
        // Can initialize other add-ons here, such as video, ...
        // Install peripherals.
        if (!al_install_keyboard()) throw Allegro5Exception("failed to install keyboard");
        if (!al_install_mouse()) throw Allegro5Exception("failed to install mouse");

        // Setup game display.

#define FULL_SCREEN

#ifdef FULL_SCREEN
        al_set_new_display_flags(ALLEGRO_FULLSCREEN_WINDOW);
#endif

        display = al_create_display(screenW, screenH);
        if (!display) throw Allegro5Exception("failed to create display");

#ifdef FULL_SCREEN
        const float scale_factor_x = ((float)al_get_display_width(display)) / screenW;
        const float scale_factor_y = ((float)al_get_display_height(display)) / screenH;

        ALLEGRO_TRANSFORM t;
        al_identity_transform(&t);
        al_scale_transform(&t, scale_factor_x, scale_factor_y);
        al_use_transform(&t);
#endif

        al_set_window_title(display, title);
        // Set alpha blending mode.
        al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA);

        // Load and set window icon.
        if (icon) {
            static std::shared_ptr<ALLEGRO_BITMAP> iconReference = Resources::GetInstance().GetBitmap(icon);
            al_set_display_icon(display, iconReference.get());
            LOG(INFO) << "Loaded window icon from: " << icon;
        }

        // Setup update timer.
        update_timer = al_create_timer(1.0f / fps);
        if (!update_timer) throw Allegro5Exception("failed to create timer");

        // Setup event queue.
        event_queue = al_create_event_queue();
        if (!event_queue) throw Allegro5Exception("failed to create event queue");

        // Check how many mouse buttons are supported.
        const unsigned m_buttons = al_get_mouse_num_buttons();
        LOG(INFO) << "There are total " << m_buttons << " supported mouse buttons";

        // Register display, timer, keyboard, mouse events to the event queue.
        al_register_event_source(event_queue, al_get_display_event_source(display));
        al_register_event_source(event_queue, al_get_timer_event_source(update_timer));
        al_register_event_source(event_queue, al_get_keyboard_event_source());
        al_register_event_source(event_queue, al_get_mouse_event_source());
        // Can register other event sources, such as timer, video, ...

        // Start the timer to update and draw the game.
        al_start_timer(update_timer);
    }
    void GameEngine::startEventLoop() {
        bool done = false;
        ALLEGRO_EVENT event;
        int redraws = 0;
        auto timestamp = std::chrono::steady_clock::now();
        while (!done) {
            al_wait_for_event(event_queue, &event);
            switch (event.type) {
                case ALLEGRO_EVENT_DISPLAY_CLOSE:
                    // Event for clicking the window close button.
                    LOG(VERBOSE) << "Window close button clicked";
                    done = true;
                    break;
                case ALLEGRO_EVENT_TIMER:
                    // Event for redrawing the display.
                    if (event.timer.source == update_timer)
                        // The redraw timer has ticked.
                        redraws++;
                    break;
                case ALLEGRO_EVENT_KEY_DOWN:
                    // Event for keyboard key down.
                    LOG(VERBOSE) << "Key with keycode " << event.keyboard.keycode << " down";
                    activeScene->OnKeyDown(event.keyboard.keycode);

#ifdef FULL_SCREEN
                    // For full screen, leave game by pressing ESC.
                    if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE) done = true;
#endif

                    break;
                case ALLEGRO_EVENT_KEY_UP:
                    // Event for keyboard key up.
                    LOG(VERBOSE) << "Key with keycode " << event.keyboard.keycode << " up";
                    activeScene->OnKeyUp(event.keyboard.keycode);
                    break;
                case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
                    // Event for mouse key down.
#ifdef FULL_SCREEN
                    event.mouse.x /= ((float)al_get_display_width(display) / screenW);
                    event.mouse.y /= ((float)al_get_display_height(display) / screenH);
#endif

                    LOG(VERBOSE) << "Mouse button " << event.mouse.button << " down at (" << event.mouse.x << ", " << event.mouse.y << ")";
                    activeScene->OnMouseDown(event.mouse.button, event.mouse.x, event.mouse.y);
                    break;
                case ALLEGRO_EVENT_MOUSE_BUTTON_UP:
                    // Event for mouse key up.
#ifdef FULL_SCREEN
                    event.mouse.x /= ((float)al_get_display_width(display) / screenW);
                    event.mouse.y /= ((float)al_get_display_height(display) / screenH);
#endif

                    LOG(VERBOSE) << "Mouse button " << event.mouse.button << " down at (" << event.mouse.x << ", " << event.mouse.y << ")";
                    activeScene->OnMouseUp(event.mouse.button, event.mouse.x, event.mouse.y);
                    break;
                case ALLEGRO_EVENT_MOUSE_AXES:
                    if (event.mouse.dx != 0 || event.mouse.dy != 0) {
#ifdef FULL_SCREEN
                        event.mouse.x /= ((float)al_get_display_width(display) / screenW);
                        event.mouse.y /= ((float)al_get_display_height(display) / screenH);
#endif
                        // Event for mouse move.
                        LOG(VERBOSE) << "Mouse move to (" << event.mouse.x << ", " << event.mouse.y << ")";
                        activeScene->OnMouseMove(event.mouse.x, event.mouse.y);
                    } else if (event.mouse.dz != 0) {
                        // Event for mouse scroll.
                        LOG(VERBOSE) << "Mouse scroll at (" << event.mouse.x << ", " << event.mouse.y << ") with delta " << event.mouse.dz;
                        activeScene->OnMouseScroll(event.mouse.x, event.mouse.y, event.mouse.dz);
                    }
                    break;
                case ALLEGRO_EVENT_MOUSE_LEAVE_DISPLAY:
                    LOG(VERBOSE) << "Mouse leave display.";
                    ALLEGRO_MOUSE_STATE state;
                    al_get_mouse_state(&state);
                    // Fake mouse out.
                    activeScene->OnMouseMove(-1, -1);
                    break;
                case ALLEGRO_EVENT_MOUSE_ENTER_DISPLAY:
                    LOG(VERBOSE) << "Mouse enter display.";
                    break;
                default:
                    // Ignore events that we're not interested in.
                    break;
            }
            // Can process more events and call callbacks by adding more cases.

            // Redraw the scene.
            if (redraws > 0 && al_is_event_queue_empty(event_queue)) {
                if (redraws > 1)
                    LOG(VERBOSE) << redraws - 1 << " frame(s) dropped";
                // Calculate the timeElapsed and update the timestamp.
                auto nextTimestamp = std::chrono::steady_clock::now();
                std::chrono::duration<float> timeElapsed = nextTimestamp - timestamp;
                timestamp = nextTimestamp;
                // Update and draw the next frame.
                update(timeElapsed.count());
                draw();
                redraws = 0;
            }
        }
    }
    void GameEngine::update(float deltaTime) {
        for(auto it:sender.clients){
            sender.recvOnce(it);
        }
        //deal the data from client
        for (auto& client : sender.clients) {
            if (!client->active || client->lastInput.is_null()) continue;

            nlohmann::json& input = client->lastInput;

            if (input["type"] == "input") {
                // === KeyDown ===
                if (input.contains("keys_down")) {
                    for (auto& keyName : input["keys_down"]) {
                        int keycode = name_to_keycode(keyName.get<std::string>());
                        if (keycode != 0) {
                            activeScene->OnKeyDown(keycode);
                        }
                    }
                }

                // === KeyUp ===
                if (input.contains("keys_up")) {
                    for (auto& keyName : input["keys_up"]) {
                        int keycode = name_to_keycode(keyName.get<std::string>());
                        if (keycode != 0) {
                            activeScene->OnKeyUp(keycode);
                        }
                    }
                }

                // === Mouse ===
                if (input.contains("mouse")) {
                    int mx = input["mouse"]["x"];
                    int my = input["mouse"]["y"];
                    if(input["mouse"].contains("Move")){
                        activeScene->OnMouseMove(mx, my);
                    }
                    

                    if (input["mouse"].contains("mouse_down")) {
                        for (auto& btn : input["mouse"]["mouse_down"]) {
                            activeScene->OnMouseDown(btn, mx, my);
                        }
                    }
                    if (input["mouse"].contains("mouse_up")) {
                        for (auto& btn : input["mouse"]["mouse_up"]) {
                            activeScene->OnMouseUp(btn, mx, my);
                        }
                    }
                }


            }
        }
        
        if (!nextScene.empty()) {
            changeScene(nextScene);
            nextScene = "";
        }
        // Force lag to avoid bullet-through-paper issue.
        if (deltaTime >= deltaTimeThreshold)
            deltaTime = deltaTimeThreshold;
        activeScene->Update(deltaTime);

        for(auto it:sender.clients){
            sender.sendOnce(it);
        }
        sender.frame.clear(); 
        sender.cleanupInactiveClients();
    }
    void GameEngine::draw() const {
        activeScene->Draw();
        al_flip_display();
    }
    void GameEngine::destroy() {
        // Destroy allegro5 window resources.
        al_destroy_timer(update_timer);
        al_destroy_event_queue(event_queue);
        al_destroy_display(display);
        // Free all scenes.
        for (const auto &pair : scenes)
            delete pair.second;
    }
    void GameEngine::changeScene(const std::string &name) {
        if (scenes.count(name) == 0)
            throw std::invalid_argument("Cannot change to a unknown scene.");
        // Terminate the old scene.
        activeScene->Terminate();
        activeScene = scenes[name];
        // Release unused resources.
        if (freeMemoryOnSceneChanged)
            Resources::GetInstance().ReleaseUnused();
        // Initialize the new scene.
        activeScene->Initialize();
        LOG(INFO) << "Changed to " << name << " scene";
    }
    void GameEngine::Start(const std::string &firstSceneName, int fps, int screenW, int screenH,
                           int reserveSamples, const char *title, const char *icon, bool freeMemoryOnSceneChanged, float deltaTimeThreshold) {
        LOG(INFO) << "Game Initializing...";
        // Update Allegro5 configs.
        this->fps = fps;
        this->screenW = screenW;
        this->screenH = screenH;
        this->reserveSamples = reserveSamples;
        this->title = title;
        this->icon = icon;
        this->freeMemoryOnSceneChanged = freeMemoryOnSceneChanged;
        this->deltaTimeThreshold = deltaTimeThreshold;
        if (scenes.count(firstSceneName) == 0)
            throw std::invalid_argument("The scene is not added yet.");
        activeScene = scenes[firstSceneName];

        initAllegro5();
        LOG(INFO) << "Allegro5 initialized";
        LOG(INFO) << "Game begin";

        activeScene->Initialize();
        LOG(INFO) << "Game initialized";
        // Draw the first frame.
        draw();
        LOG(INFO) << "Game start event loop";
        // This call blocks until the game is finished.
        startEventLoop();
        LOG(INFO) << "Game Terminating...";
        activeScene->Terminate();
        LOG(INFO) << "Game terminated";
        LOG(INFO) << "Game end";
        destroy();
    }
    void GameEngine::AddNewScene(const std::string &name, IScene *scene) {
        if (scenes.count(name) != 0)
            throw std::invalid_argument("Cannot add scenes with the same name.");
        scenes[name] = scene;
    }
    void GameEngine::ChangeScene(const std::string &name) {
        nextScene = name;
    }
    IScene *GameEngine::GetActiveScene() const {
        return activeScene;
    }
    IScene *GameEngine::GetScene(const std::string &name) {
        if (scenes.count(name) == 0)
            throw std::invalid_argument("Cannot get scenes that aren't added.");
        return scenes[name];
    }
    Point GameEngine::GetScreenSize() const {
        return Point(screenW, screenH);
    }
    int GameEngine::GetScreenWidth() const {
        return screenW;
    }
    int GameEngine::GetScreenHeight() const {
        return screenH;
    }
    Point GameEngine::GetMousePosition() const {
        ALLEGRO_MOUSE_STATE state;
        al_get_mouse_state(&state);
        
#ifdef FULL_SCREEN
        state.x /= ((float)al_get_display_width(display) / screenW);
        state.y /= ((float)al_get_display_height(display) / screenH);
#endif

        return Point(state.x, state.y);
    }
    bool GameEngine::IsKeyDown(int keyCode) const {
        ALLEGRO_KEYBOARD_STATE state;
        al_get_keyboard_state(&state);
        return al_key_down(&state, keyCode);
    }
    GameEngine &GameEngine::GetInstance() {
        // The classic way to lazy initialize a Singleton.
        static GameEngine instance;
        return instance;
    }
    RenderSender& GameEngine::GetSender(){
        return sender;
    }
}
