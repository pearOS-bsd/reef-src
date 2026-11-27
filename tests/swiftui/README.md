# SwiftUI differential tests for Reef

81 small SwiftUI apps, one area each, built on the Mac VM and run in the Reef test container with the original
SwiftUI or with a candidate file. The runner reports per app "same" or what differs (stdout, view tree, layout
values, stderr, exit state, window rects, screenshot pixels). It also runs the original twice to find output that
changes by itself.

## Layout

- `apps/NAME/main.swift` : one app per directory (some have `res/` for bundled files). Each app calls
  `Harness.run(title, w, h) { V() }` from `common/Harness.swift`.
- `common/Harness.swift` : shared helper. Starts an AppKit window with an `NSHostingView` (the `WindowGroup`
  initialisers of the current SDK are not exported by Reef's SwiftUI; `scene_app` and `window_group_content` test
  them), and gives the apps `.finalState(name)`, `.probe(name)` and `Harness.note(...)`.
- `build.sh` : builds apps on the Mac VM (`ssh -p 2229 alex@localhost`, Swift 6.3.3,
  `-target x86_64-apple-macos14.0`, ad-hoc signed) and fetches the bundles to `~/.pear/tmp/tk/swiftui/apps`
  (override with `SWIFTUI_APPS`). `build-mac.sh` is the part that runs on the Mac.
- `inside.sh` : runs one app in the container: headless sway, `grim` screenshot, sway window tree, stdout, stderr.
- `run.py` : the runner.

## How the apps behave

- No network, no clock text, no animation: state changes come from `onAppear` chains
  (`DispatchQueue.main.async`), dates are fixed.
- When the app reaches its last state, `.finalState("name")` waits 1.5 s for layout, then prints in this order:
  `NOTE ...` lines (values logged by the app), `LAYOUT name (x,y,w,h)` lines (frames of `.probe(name)` views in
  window coordinates, sorted), a `VIEWTREE-BEGIN/END` block (AppKit view tree of every window, with text
  of NSTextFields and titles of NSButtons) and `FINAL name`. The runner waits for the `FINAL` line, then takes the
  screenshot. If the app crashes or a symbol is missing there is no `FINAL` line: that is reported as final NO.
- Reef renders a SwiftUI window as one `ElementCanvas`, so the view tree is short; the LAYOUT lines carry the
  layout information, the screenshot carries the pixels. `REEF_DUMP_VIEWS` is set (8 s) but only our AppKit wrapper
  uses it.

## Run

```
sh build.sh                      # all apps (or: sh build.sh stacks frames)
python3 run.py                   # baseline: original twice per app, no comparison to a candidate
python3 run.py --table           # same, plus a markdown table
python3 run.py --swiftui /path/to/SwiftUI --apps stacks,frames   # compare a candidate against the original
```

Options: `--apps a,b` subset, `--runs DIR` (default `~/.pear/tmp/tk/swiftui/runs`), `--container NAME`
(default `reef-test-orig`, which has Reef and sway/grim), `--timeout S` (40), `--rerun-orig` (ignore the cached
original runs), `--apps-dir DIR`.

For a candidate the runner copies the file over
`/usr/lib/cutefish-reef/System/Library/Frameworks/SwiftUI.framework/Versions/A/SwiftUI` in the container
(the original is kept as `SwiftUI.keep` and restored at the end), deletes `~/Library/Saved Application State/<id>.savedState`
and starts from an empty home before every run. Per app it prints `same` or `DIFFERENT` followed by one line per
difference; differences that the original shows between its own two runs are removed and
reported as "original itself is nondeterministic". Results are kept in `runs/APP/{orig1,orig2,cand}/`
(stdout, stderr, status, final, shot.ppm, tree.json, missing.json).

Checked: `run.py --swiftui <the original file>` gives `same` (stacks).

Time: about 12 s per run, 3 runs per app with a candidate; the whole suite takes about 40 minutes.

## Baseline (original SwiftUI of upstream/cutefish-reef, md5 f6b8decf...)

64 of 81 apps reach the final state. Each of the 17 others stops at a symbol that Reef's SwiftUI/Foundation lacks
or crashes (SIGSEGV); because one failure hides everything else in an app, the apps are split so that each
problem is isolated. Only 2 apps are nondeterministic (caret blink, spinner pixels); the others give identical
stdout, layout values, windows and pixels in both runs.

Known bugs from the brief, as observed:

- custom `EnvironmentKey` in a `ButtonStyle`: reproduced (`button_style_env`).
- `_GeometryActionModifier` action never runs: reproduced (`geometry_action`).
- `alert(_:isPresented:actions:)`: the app compiled against the SDK binds to the variant Reef has; `alert_isPresented`
  works (AKAlertPanel with title, message, two buttons). `WindowGroup(content:)` missing: reproduced
  (`window_group_content`, `scene_app`).
- `View.aspectRatio` crash at address 0: not reproduced; the common forms work (`aspectratio`), but
  `aspectRatio(1, .fit).frame(maxWidth: .infinity, maxHeight: 40)` is laid out 400x0.
- NavigationSplitView 0x0 / ignored columnVisibility, and the infinite-proposal case of a max-width frame:
  not reproduced in these small apps (`nav_split`, `nav_split_hidden`, `layout_infinite_proposal`); they need the
  Calculator's structure.

Further bugs found by the suite are in the table.

| app | final state | result | notes |
|---|---|---|---|
| alert_isPresented | yes | runs | works |
| alert_legacy | yes | runs | works |
| alignment | yes | runs | works |
| anchor_preference | **no** | exit 139 | crash: _OverlayPreferenceModifier (overlayPreferenceValue with an Anchor) SIGSEGV |
| animation_end | yes | runs | works |
| animation_withanimation | yes | runs | works |
| appstorage | yes | runs | works |
| aspectratio | yes | runs | works; note flex case: aspectRatio(1, .fit).frame(maxWidth: .infinity, maxHeight: 40) is laid out 400x0 (Apple: 40x40) |
| binding | yes | runs | works |
| button_default | yes | runs | works |
| button_style_env | yes | runs | REPRODUCES bug: custom EnvironmentKey values read in a ButtonStyle give the default (accent=gray, pad=10 although set on the button); the same key read in a ViewModifier works; builtin controlSize(.large) is also not seen by the style (regular) |
| button_styles | yes | runs | works |
| canvas_basic | **no** | exit 139 | crash: GraphicsContext.fill/stroke/draw in Canvas, SIGSEGV in CGContextSaveGState |
| canvas_gradient | **no** | exit 139 | crash: GraphicsContext.fill with a gradient shading |
| canvas_symbols_clip | **no** | exit 139 | crash: GraphicsContext.fill after clip/resolveSymbol |
| confirmation_dialog | yes | runs | works |
| environment | yes | runs | works |
| fills | yes | runs | works |
| focus_state | yes | runs | works |
| form_groupbox | yes | runs | works |
| format_currency | **no** | runs | missing symbol: Foundation FloatingPointFormatStyle.Currency |
| format_measurement | **no** | runs | missing symbol: Foundation Measurement format style (.measurement) |
| formats | yes | runs | works |
| frames | yes | runs | works |
| geometry_action | yes | runs | REPRODUCES bug: the action of onGeometryChange never runs ("modifier _GeometryActionModifier is not drawn yet" on stderr) |
| geometryreader | yes | runs | works |
| grid_align | yes | runs | works |
| grid_anchor | yes | runs | works |
| grid_basic | yes | runs | works |
| grid_span | **no** | exit 139 | crash: GridRow with gridCellColumns (value-witness copy of a GridRow tuple), SIGSEGV |
| image_bundled | yes | runs | works |
| image_system | yes | runs | works |
| label_link | yes | runs | works |
| layout_anylayout | **no** | exit 139 | crash: AnyLayout/HStackLayout/VStackLayout/ZStackLayout, SIGSEGV in a value-witness destroy |
| layout_flow_cache | yes | runs | works |
| layout_infinite_proposal | yes | runs | works here: frame(maxWidth: 48) answers 48 to an infinite proposal (the Calculator case did not reproduce with plain frames) |
| layout_radial | yes | runs | works |
| layout_value_key | yes | runs | works |
| lazyvgrid | yes | runs | works |
| list_children | yes | runs | works |
| list_foreach | yes | runs | works |
| menus | yes | runs | works |
| nav_link_title | **no** | runs | missing symbol: NavigationLink(_:value:) with a title string (and NavigationLink(_:destination:)) |
| nav_path | **no** | runs | missing symbol: NavigationPath.append |
| nav_split | yes | runs | works: columnVisibility .all, detail 479x400 (0x0 bug not reproduced here) |
| nav_split_hidden | yes | runs | works; initial .detailOnly is changed to .doubleColumn by the app; shows the 163 wide detail |
| nav_stack | yes | runs | works |
| nsview_representable | yes | runs | works |
| observable | yes | runs | works |
| outline_group | **no** | runs | missing symbol: OutlineGroup(_:children:content:) |
| overlay_background | yes | runs | works |
| padding_spacer | yes | runs | works |
| path | yes | runs | works |
| picker_menu | yes | runs | works |
| picker_segmented | yes | runs | works |
| pickers_date_color | yes | runs | works |
| popover_help | yes | runs | works |
| preference_keys | yes | runs | works |
| progress_gauge | yes | runs | works; nondeterministic pixels (spinner / indeterminate animation) |
| scene_app | **no** | runs | missing symbol: WindowGroup(_:content:) (non-escaping content initialiser; Reef only has the escaping one) |
| scenestorage | **no** | exit 139 | crash: @SceneStorage in a view, SIGSEGV while building the root view |
| scrollview | yes | runs | works |
| shapes | yes | runs | works |
| sheet | yes | runs | works |
| slider | yes | runs | works |
| slider_labels | **no** | runs | missing symbol: Slider(value:in:step:label:minimumValueLabel:maximumValueLabel:) |
| stacks | yes | runs | works |
| state_chain | yes | runs | works |
| table | yes | runs | works |
| tabview | yes | runs | works |
| text_styles | yes | runs | works |
| text_wrap | yes | runs | works |
| textfield | yes | runs | works; nondeterministic pixels (insertion point blink) |
| toggle | yes | runs | works |
| toolbar | yes | runs | works |
| toolbar_secondary | **no** | runs | missing symbol: ToolbarItemPlacement.secondaryAction |
| transitions | yes | runs | works |
| view_containers | yes | runs | works |
| view_that_fits | **no** | exit 139 | crash: ViewThatFits, SIGSEGV in a value-witness copy |
| window_group_content | **no** | runs | missing symbol: WindowGroup(content:) (non-escaping) |
| zindex_priority | yes | runs | works |
