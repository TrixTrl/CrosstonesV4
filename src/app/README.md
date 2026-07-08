# UI Architecture Reference

How to write a new screen or widget in `src/app/`.

## App lifecycle

Every screen extends `App` ([App.h](App.h)) and implements a subset of:

```cpp
virtual void onStart() {}                                  // one-time setup
virtual void onTick(float dt, const InputState& input) {}  // ALL input handling + state mutation
virtual void onDraw(Rectangle rect) = 0;                    // pure draw, reads state only
virtual void onDrawOverlay(Rectangle rect) {}               // pure draw, drawn after onDraw (dropdowns, status bars, etc.)
virtual void onStop() {}                                    // teardown
```

Input polling only happens in `onTick`.
`onDraw`/`onDrawOverlay`, are used to make draw calls for already computed state. 
`InputState` carries `mouse`/`clicked`/`wheel`, 
keyboard functions can be called directly from `onTick`.

`layout` is already resolved for the current frames `rect` by
the time `onTick` runs. To read it in `onTick` and `onDraw`/`onDrawOverlay`,
use `layout.find("id")->rect`.

If a screen needs a service beyond the base `App` interface, override
`injectServices`:

```cpp
void injectServices(ui::UiScale* scale) override { scaleCtl = scale; }
```

`AppHost` calls this on every screen before `onStart()`.

## State management

- Use a `Phase` enum when a screen has multiple modes. I decided to group each phases
  data into its own struct rather than flattening everything onto the class.

## Widgets (`ui/components/`)

Every widget follows the same shape:

```cpp
void draw(Rectangle rect, ..., const Theme& t);
int/bool/void handleInput(Rectangle rect, Vector2 mouse, bool click, ...);
```

Rules:
1. Hover/selection state lives on the widget, is written only inside
   `handleInput`, and read only inside `draw`.
2. All colors come from `const Theme&`. For new colors, add a field 
   to `ui::Theme` ([theme.h](ui/theme.h)) rather than hardcoding.
3. Similar to Screens, `draw` never mutates state and never polls input. 
   `handleInput` never draws.
4. Screens call `handleInput` only from `onTick`, `draw` only from
   `onDraw`/`onDrawOverlay`.

See `Button`, `Stepper`, `Dropdown`, or `Board` as examples.

### raygui stays inside two files

raygui is only used in [ui/theme.cpp](ui/theme.cpp) and [ui/components/](ui/components/). 
It is best to wrap raygui components it in a new `ui/components/*` widget instead
of calling raygui from screen code, see [ui/components/scroll_area.h](ui/components/scroll_area.h).

## Layout (`ui/layout.h`)

`ui::Slot` is a flexbox-like tree (`Row`/`Col`, `fixed`/`flex`/`minPx`
sizing, `padding`, `gap`):

```cpp
layout = { .dir = ui::Dir::Col, .padding = {0, 50, 0, 50}, .children = {
    { .id = "title", .fixed = 50 },
    { .id = "body",  .flex = 1, .dir = ui::Dir::Row, .gap = 30, .children = {
        { .id = "left",  .flex = 1 },
        { .id = "right", .flex = 1 },
    }},
}};
```

Set this up once in `onStart()`, then read positions via
`layout.find("id")->rect`. A widget splitting its own rect internally (like
`Stepper` for arrow/label/arrow) is fine, i have found no better solution yet.

## Z-ordering (`ui/overlay.h`)

For anything that must draw on top of everything else this frame (e.g. an
open dropdown list), push it instead of drawing inline:

```cpp
overlayPush(id, [snapshot](const Theme& t) { /* draw using only captured values */ }, zOrder);
```

The pushed lambda must capture a value snapshot of everything it needs,
never `this` or a live reference into a widget, since the widget's state can
change between when you push and when it's drawn. `overlayBegin()`/
`overlayFlush()` are called once per frame by `AppHost`.

## Theme & scale

- `ui::Theme` ([theme.h](ui/theme.h)) holds all colors and font sizes.
- `ui::UiScale` ([ui/scale.h](ui/scale.h)) holds the UI scale factor setting, owned by `AppHost`. 
  It's modified by [SettingsApp.h](SettingsApp.h).

