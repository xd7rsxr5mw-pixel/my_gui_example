# my_gui_example

A four-workflow Qt 6.9+ / C++23 showcase for Windows and Linux. The application describes its interface with typed C++ calls; Qt and QML stay inside the reusable [my_gui_engine](https://github.com/xd7rsxr5mw-pixel/my_gui_engine) dependency. This example is independent of the Progressor application.

The two projects have separate Git histories. This repository owns the showcase, its backend, and its styling. `gui_engine/` is a Git submodule pinned to an exact engine commit; it contains no copy of the showcase. The initial repository setup is local only. Publishing is a separate step, with the engine published before the example that references it.

The existing local folder is still named `My_GUI`, as requested while its Windows directory lock is unresolved. The repository identity is `my_gui_example`; a later clone will use that name by default. The folder can be renamed once no terminal or process holds it open.

## Obtain the complete checkout

Once the repositories have been published and your account has access to both:

```sh
git clone --recurse-submodules https://github.com/xd7rsxr5mw-pixel/my_gui_example.git
cd my_gui_example
```

For an existing clone, run `git submodule update --init --recursive`. After pulling application changes, run the same command to restore the engine commit recorded by the application. Building never fetches or upgrades dependencies automatically.

See [Git integration and release workflow](docs/GIT_INTEGRATION.md) for initial publication, engine updates, branching, rollback, and integration into other projects such as Tindeq UI.

## Workspace map

| Folder | What belongs here |
| --- | --- |
| `gui_engine/` | Submodule of the independent engine repository: reusable rendering, interaction, persistence, public API, and engine tests. |
| `gui_src/showcase.cpp` | Application composition and callbacks. Add workflows, panels, elements, and Explorer entries here. |
| `gui_src/styling/` | Colors, dimensions, fonts, and interaction timing in four JSON files. |
| `backend/` | Ordinary C++ calculations, without Qt dependencies. |
| `gui_testing/` | Example-specific backend checks and actual showcase startup tests. |
| `gui_engine/gui_testing/` | Generic engine tests, neutral style fixtures, and a minimal consumer used only for integration testing. |
| `.generated/` | CMake intermediates and development executables; ignored by Git. |
| `build/windows/`, `build/linux/` | Release application and required runtime assets only; ignored by Git. |
| `gui_testing/.artifacts/` | Test builds, reports, and temporary test data; ignored by Git. |

Read [the engine guide](gui_engine/README.md), [list and Explorer recipes](gui_engine/LISTS.md), and the [public header](gui_engine/include/mygui/gui.hpp) for the complete vocabulary.

## Build and run on Windows

The supplied presets use the existing MSYS2 UCRT64 toolchain at `C:/msys64/ucrt64`: GCC, CMake, Ninja, Qt Quick, and Qt Quick Controls. No Bluetooth libraries are required.

```powershell
C:\msys64\ucrt64\bin\cmake.exe --preset windows
C:\msys64\ucrt64\bin\cmake.exe --build --preset windows
.\build\windows\my_gui_showcase.exe
```

Release deployment copies the needed Qt libraries/plugins and JSON styling beside the executable. It includes third-party DLL dependencies, keeps only the Basic Controls implementation used by this engine, and removes duplicate Qt resource sources and development metadata. Keep that directory together. Development builds use `windows-dev`; their executable is `.generated/cmake/windows-debug/app/my_gui_showcase.exe`. Put `C:/msys64/ucrt64/bin` on `PATH` when running a development executable.

## Build and run on Linux

Use Qt 6.9 or newer, a C++23 compiler, CMake 3.24+, and Ninja. On Debian/Ubuntu, install `g++ cmake ninja-build qt6-base-dev qt6-declarative-dev qt6-declarative-dev-tools qml6-module-qtquick qml6-module-qtquick-controls qml6-module-qtquick-layouts qml6-module-qtquick-window qml6-module-qtqml-workerscript`. Check your distribution's Qt version; older LTS repositories may require a newer Qt installation via `CMAKE_PREFIX_PATH`.

```sh
cmake --preset linux
cmake --build --preset linux
./build/linux/my_gui_showcase
```

The Linux executable uses the installed Qt runtime. This project does not produce an AppImage or bundle system libraries. Development builds use `linux-dev` and run from `.generated/cmake/linux-debug/app/`.

## What the showcase demonstrates

1. **Controls & Calculations:** typed values, numeric validation, text and multiline inputs, dropdowns, checkbox, conditional content, highlight groups, and a callback into the plain C++ calculator.
2. **Lists & Composition:** compact records with right descriptors, rich editable cards, repeated/collapsible groups, multiselection, search, context menus, copy/paste, drag reorder, and drag from catalogue to selected list.
3. **Charts & Data:** two static series, a generated streaming signal, rolling time window, adaptive axes, and a cancellable background calculation. The generated signal is explicitly demo data.
4. **Dialogs & Workspace:** modal composition, accept/cancel rollback, edit spotlight, persisted layout, and a three-row workflow.

The shell includes a resizable/collapsible navigator and Explorer, workflow scrolling, collapsible/resizable subwindows, nested scrolling, status messages, Settings, and Exit. Workflow shortcuts use their number; activating the current workflow again restores its initial panel layout. Delayed hover marks resize gaps without permanent divider buttons.

## Compose another project

Add the engine repository as a submodule in the consuming project. Keep that project's `gui_src/`, styling, backend, and application tests in its own repository. The independent fixture in `gui_engine/gui_testing/consumer/` demonstrates the minimal CMake integration; it is a small engine test harness, not the four-workflow showcase.

```sh
git submodule add https://github.com/xd7rsxr5mw-pixel/my_gui_engine.git gui_engine
git -C gui_engine fetch origin --tags
git -C gui_engine switch --detach v1.0.0
git add .gitmodules gui_engine
git commit -m "Add MyGui engine v1.0.0"
```

These commands require the engine commit and tag to have been published first. The parent records the resolved commit, rather than following a moving branch or tag during builds.

```cpp
auto a = app.state("a", 2.0);
auto b = app.state("b", 3.0);
auto result = app.state("result", 0.0);
auto panel = app.workflow("calculator", "Calculator").panel("inputs", "INPUTS");
panel.numberInput("a", "First number", a);
panel.numberInput("b", "Second number", b);
panel.button("add", "Add", [=] { result.set(backend.add(a.get(), b.get())); });
panel.readout("sum", "Sum", result);
```

Application code needs `mygui/gui.hpp`, not QObject subclasses or QML. A GUI change shared by projects belongs in the engine; titles, content, callbacks, and visual tokens belong in the consuming project.

## Styling and saved data

The four JSON files reload while the application runs. Debug watches `gui_src/styling`; Release watches `styling` beside its executable. Invalid JSON keeps the last valid style and reports the error in the status bar. Two spacing tokens and three stroke roles drive the shared layout; the larger spacing is derived from the smaller spacing and scrollbar width.

Each application ID owns its settings. The showcase normally saves under `%APPDATA%/MyGui/my_gui.showcase` on Windows, or `$XDG_DATA_HOME/MyGui/my_gui.showcase` (usually `~/.local/share/MyGui/my_gui.showcase`) on Linux. Settings can relocate the complete tree to an empty directory. A small `settings-location.ini` pointer remains in the default location.

Window/sidebar/panel layout saves automatically. Values and record models save only when `.persist()` is requested. Settings use an atomic `app.ini`; user records, layout, and values never go in the build directory. Cancel restores values/models bound into the dialog; application-defined external side effects remain the callback's responsibility.

For isolated runs, use `--data-root <folder>`. `--no-persist` disables writing; `--styles <folder>` chooses another theme folder; `--quit-after <milliseconds>` supports automated startup checks.

## Tests

Install Qt Test and Qt Quick Test (`qml6-module-qttest` on Debian/Ubuntu). Add `xvfb xauth` if your environment needs a virtual display.

```powershell
C:\msys64\ucrt64\bin\cmake.exe --preset windows-tests
C:\msys64\ucrt64\bin\cmake.exe --build --preset windows-tests
$env:PATH = 'C:\msys64\ucrt64\bin;' + $env:PATH
C:\msys64\ucrt64\bin\ctest.exe --preset windows
```

```sh
cmake --preset linux-tests
cmake --build --preset linux-tests
ctest --preset linux
```

The test presets enable both the example checks and the engine's reusable suite. Engine tests check model identity/clipboard, persistence and relocation, modal rollback, asynchronous callbacks, numeric validation, selection, wheel handoff, drag/drop, resize/collapse, viewport sizing, and a minimal independent consumer. Example tests check its calculator and actual showcase startup. Tests use isolated data roots; generated test output stays under the owning repository's `gui_testing/.artifacts/`.

## Working on the shared engine

Ordinary application work stays in this repository. For shared fixes, create a branch inside `gui_engine`, commit the engine change there, and update the parent project's engine pointer after testing. A detached engine HEAD after cloning is normal: it means the application is using its pinned dependency. Do not commit shared fixes on a detached HEAD; create a named branch first.

Use `main` for integrated, tested changes and short-lived `feature/*` or `fix/*` branches for work. A dependency bump is an ordinary application commit that can be reviewed or reverted. Do not ignore `gui_engine/`; Git must track its commit pointer. The engine's own `.gitignore` handles its generated files.
