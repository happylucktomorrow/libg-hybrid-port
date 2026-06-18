# Customization Guide

This document covers how to customize Boom Beach's `libg.so` using the hybrid-port approach.

---

## 1. Which Game Logic Functions Can I Touch?

Run the call graph analysis:

```bash
python3 scripts/analyze_call_graph.py original/libg.so symbols/game_logic_symbols.txt
```

This produces two files:

| File | What it means |
|------|---------------|
| `call_graph_SAFE.txt` | Replace directly with C++ — no internal ARM calls these |
| `call_graph_NEEDS_TRAMPOLINE.txt` | Called by internal ARM — need a branch veneer |

### Quick heuristics

A function is **SAFE** if:
- No internal ARM code calls it (only JNI/Java or game logic calls it)
- It's a **leaf** — doesn't call other functions that would break

A function **NEEDS_TRAMPOLINE** if:
- Internal ARM code does `BL` to reach it
- At the original address, we put a branch instruction to our new C++

A function is **DONT_TOUCH** if:
- It's part of a hot internal call path you'd need to fully reimplement

### Example safe-to-touch functions (based on typical Boom Beach structure)

```
LogicBuyBuildingCommand
LogicTrainUnitCommand
LogicUpgradeBuildingCommand
LogicSellBuildingCommand
LogicMoveBuildingCommand
LogicSpeedUpConstructionCommand
LogicSpeedUpTrainingCommand
HomeState::tick
GameData::serialize/deserialize
```

---

## 2. Adding Custom Buttons to UI

### How Boom Beach UI works

The display hierarchy is:
```
Stage
  └── MovieClip / Sprite
        └── Button (extends MovieClip)
              ├── background (MovieClip frame)
              ├── icon (Sprite)
              └── text (TextField)
```

### To add a custom button:

**Step 1: Identify where to inject**

Find a screen/menu class that handles the area you want. For example, `MainMenuScreen` or `HUD`. These are exported symbols you can locate.

**Step 2: Create a Button subclass**

In the display framework (C++), a button is essentially a `MovieClip` with touch handling:

```cpp
// Pseudocode for custom button
class CustomButton : public MovieClip {
public:
    CustomButton(const char* button_id, float x, float y) {
        // Load button art from SC bundle/atlas
        loadMovieClip("ui_buttons/custom_button");

        // Add touch listener
        addTouchHandler([this](TouchEvent e) {
            if (e.type == TouchEvent::CLICK) {
                onClick();
            }
        });
    }

    void onClick() {
        // Handle button press
        showPopup();
    }

    void showPopup();
};
```

**Step 3: Instantiate the button**

In the screen's `show()` method (which you override), add:

```cpp
extern "C" LIBG_EXPORT
void _ZN14CustomScreen6showEv(void* self) {
    auto* screen = static_cast<CustomScreen*>(self);

    // Add our custom button
    auto* btn = new CustomButton("my_button", 100.0f, 200.0f);
    screen->addChild(btn);
}
```

**Step 4: Register touch handling**

The internal ARM code handles the actual touch dispatch. Your C++ button subclass methods get called through virtual dispatch from the original `MovieClip::handleTouch` that routes to your overridden methods.

### Important: UI classes are mostly INTERNAL

Most `*Screen`, `*HUD`, `*Menu` classes are **internal** (hidden visibility in original binary). You cannot simply add a new button by replacing a screen — the screen's `show()` method is internal ARM code.

**Options:**
1. **Find an existing hook** — some screens have `onShow()` virtual methods you can override
2. **Replace the entire screen class** — more work, but lets you add any button
3. **Use the debug menu hook** — the game has a debug menu system you can extend

---

## 3. Adding Custom Popup Dialogs

### How popups work

Popups in Boom Beach are layers that appear on top of the game:

```
Stage
  └── PopupLayer (MovieClip)
        ├── ConfirmPopup
        ├── InfoPopup
        └── CustomPopup  ← you
```

### Creating a custom popup

```cpp
class CustomPopup : public MovieClip {
public:
    CustomPopup(const std::string& title, const std::string& message) {
        // Create popup background
        loadMovieClip("popups/confirm_popup");

        // Add title text
        auto* titleField = new TextField();
        titleField->setText(title);
        titleField->setFontSize(24);
        addChild(titleField);

        // Add message text
        auto* msgField = new TextField();
        msgField->setText(message);
        msgField->setFontSize(18);
        addChild(msgField);

        // Add OK button
        auto* okBtn = new Button();
        okBtn->setLabel("OK");
        okBtn->addClickHandler([this]() { close(); });
        addChild(okBtn);

        // Add Cancel button
        auto* cancelBtn = new Button();
        cancelBtn->setLabel("Cancel");
        cancelBtn->addClickHandler([this]() { close(); });
        cancelBtn->setX(100);
        addChild(cancelBtn);
    }

    void close() {
        removeFromParent();
    }
};
```

### Showing the popup

```cpp
extern "C" LIBG_EXPORT
void showCustomPopup(const char* title, const char* msg) {
    auto* popup = new CustomPopup(title, msg);
    getStage()->addChild(popup);
}
```

### Integration with game logic

Call your popup from a game logic command:

```cpp
extern "C" LIBG_EXPORT
void _ZN21LogicBuyBuildingCommand7executeEv(void* self) {
    auto* cmd = static_cast<LogicBuyBuildingCommand*>(self);

    if (canAfford(cmd)) {
        executePurchase(cmd);
    } else {
        showCustomPopup("Not Enough Resources",
                        "You don't have enough gold!");
    }
}
```

---

## 4. Adding Custom API Endpoints

### How the protocol works

Client ↔ Server communication uses a binary protocol:

```
[4 bytes: message type ID]
[4 bytes: length]
[N bytes: payload]
```

Message types are defined in the protocol layer. To add a new API endpoint:

**Step 1: Define a new message type ID**

```cpp
// In your custom code or patched libg
enum CustomMessageType {
    MSG_CUSTOM_HELLO = 30000,
    MSG_CUSTOM_GAME_STATE = 30001,
    MSG_CUSTOM_MODIFY_TROOP = 30002,
};
```

**Step 2: Implement the message handler**

```cpp
class CustomHelloMessage : public Message {
public:
    CustomHelloMessage() {
        m_type = MSG_CUSTOM_HELLO;
    }

    bool unserialize(ByteStream& stream) override {
        if (!Message::unserialize(stream)) return false;

        // Read custom fields
        m_clientVersion = stream.read_int32();
        m_playerName = stream.read_string();
        return true;
    }

    void process() override {
        // Handle on server side
        printf("Custom hello from: %s (v%d)\n",
               m_playerName.c_str(), m_clientVersion);

        // Send response
        sendMessage(MSG_CUSTOM_HELLO, createResponse());
    }

private:
    int32_t m_clientVersion;
    std::string m_playerName;
};
```

**Step 3: Register with the factory**

The original code has a `Factory` class that creates messages by type ID:

```cpp
extern "C" LIBG_EXPORT
void* _ZN7Factory17createMessageFromEPKhi(void* factory, const uint8_t* data, int32_t len) {
    int32_t type = decodeMessageId(data);

    switch (type) {
        case MSG_CUSTOM_HELLO:
            return new CustomHelloMessage();
        // ... other cases
        default:
            // Fall through to original factory
            return nullptr;  // signal original code to handle
    }
}
```

**Step 4: Server side (HBS server)**

Add handling in your Boom Beach server. See `libg-message-bridge` project which handles the message bridge between client and server.

### Important: Server must understand your messages

The **client and server must agree** on message types and payloads. If you're only modifying the client, you can only add client-side logic. For server communication, you need the corresponding `HBS_src` changes.

---

## 5. How to Make Models of Units

### How units are defined in Boom Beach

Units have **two layers**:
1. **Game logic data** (CSV/data tables) — defines stats: damage, health, speed, cost, etc.
2. **Visual assets** (SC bundles/atlas) — defines how the unit looks

### Game logic: Static Data

Unit stats are in the game's static data tables (CSV files extracted from APK):

```csv
# units.csv (simplified example)
id,name,hp,damage,attack_speed,move_speed,cost_type,cost_amount,training_time
1,Warrior,100,20,1.0,2.0,gold,50,60
2,Archer,60,15,0.8,2.5,gold,30,45
3,Tank,500,30,1.5,1.0,gold,100,120
```

To create a new unit:

**Step 1: Add to the data table**

In `StaticDataCatalog` (game logic):

```cpp
extern "C" LIBG_EXPORT
void _ZN11StaticData16addUnitTypeDataERK11UnitDataRecord(void* self, void* record) {
    // Original function — adds unit to catalog
    // Your override can add custom units
}

extern "C" LIBG_EXPORT
UnitData* _ZN11StaticData13getUnitByIdEi(void* self, int32_t id) {
    // Return unit data by ID
    // For custom unit IDs (e.g., > 1000), return your custom data
}
```

**Step 2: Add visual assets (SC bundle)**

Units are rendered using SC bundle atlas textures. The game loads sprites from:
- `assets/`: extracted from APK
- `scbundle/`: decoded SC bundle format

For a new unit, you need:
1. **Sprite sheet** — PNG with all animation frames
2. **Animation data** — XML/JSON defining animation sequences (idle, walk, attack)
3. **SC bundle** — pack the sprite + animation into game's bundle format

### Adding a custom unit step by step

1. **Create sprite assets**
   ```
   custom_unit/
   ├── idle_1.png .. idle_4.png
   ├── walk_1.png .. walk_8.png
   ├── attack_1.png .. attack_6.png
   └── death_1.png .. death_4.png
   ```

2. **Define animation data**
   ```json
   {
     "unit_id": 1001,
     "animations": {
       "idle":  { "frames": [1,2,3,4], "fps": 8, "loop": true },
       "walk":  { "frames": [1,2,3,4,5,6,7,8], "fps": 12, "loop": true },
       "attack": { "frames": [1,2,3,4,5,6], "fps": 10, "loop": false }
     }
   }
   ```

3. **Pack into SC bundle** (use existing tools from the project)

4. **Add to data tables**

5. **Register unit in game logic**

### Key insight: You don't need to remake models

The unit models are already in the game. You can:
- **Modify existing units** by changing their data table entries
- **Use unused unit slots** (the game may have placeholder IDs)
- **Add cosmetic variants** by creating new SC bundle entries with custom art but same IDs

The actual rendering is handled by `MovieClip` → `Sprite` → `Texture` pipeline, all internal ARM code you keep as-is.

---

## Summary: What You Can Easily Change

| Area | Difficulty | Approach |
|------|------------|----------|
| Game logic commands | Easy | Replace LogicCommand::execute C++ |
| Unit stats | Easy | Override StaticData getters |
| Popup dialogs | Medium | Create new MovieClip subclasses |
| Custom buttons | Medium | Hook into screen show() methods |
| New API messages | Medium | Add message types + factory |
| Unit visual models | Hard | SC bundle creation + data table |
| UI screens | Hard | Full screen replacement |
| Display framework | Very Hard | Trampoline everything |
