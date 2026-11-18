# Git integration and shared-engine workflow

## Repository boundaries

Use two repositories, rather than different branches of one repository:

| Repository | Owns |
| --- | --- |
| `my_gui_engine` | Public C++ API, Qt/QML implementation, reusable behavior, generic tests, neutral test styling, and a minimal consumer fixture. |
| `my_gui_example` | Four-workflow showcase, `gui_src/`, production styling, calculator backend, application checks, and an exact engine dependency. |

```text
my_gui_example/                    # application repository
├── .gitmodules
├── gui_engine/                    # independent my_gui_engine submodule
│   ├── include/, src/, qml/, cmake/
│   ├── gui_testing/               # generic tests, not the full showcase
│   └── CMakeLists.txt
├── gui_src/
│   ├── showcase.cpp
│   └── styling/
├── backend/
├── gui_testing/                   # example-specific checks
└── CMakeLists.txt
```

The intended remotes are:

- Engine: `git@github.com:xd7rsxr5mw-pixel/my_gui_engine.git`
- Example: `git@github.com:xd7rsxr5mw-pixel/my_gui_example.git`

Use `ssh -T git@github.com` to check that GitHub recognizes your SSH key.

The local checkout is named `my_gui_example`. The repositories remain local until their `main` branches are pushed.

The engine remote must exist and be accessible before another machine can clone the example recursively. A private engine requires read access for every consumer and its CI runner. A public example does not grant access to a private dependency.

The engine can be developed in the nested working directory; no sibling checkout is required. Its Git metadata belongs under the parent `.git/modules/` after `git submodule absorbgitdirs`. Do not move or copy `.git` directories manually.

## Clone, reproduce, upgrade, and roll back

Git stores an engine commit ID as a **gitlink** in the application tree. `.gitmodules` stores its path and URL. Even if `.gitmodules` names `main` as an update branch, ordinary checkout restores the commit recorded by the application. See [Git's submodule documentation](https://git-scm.com/docs/gitsubmodules).

After publication, clone with:

```sh
git clone --recurse-submodules git@github.com:xd7rsxr5mw-pixel/my_gui_example.git
cd my_gui_example
```

For an existing checkout:

```sh
git pull --ff-only
git submodule sync --recursive
git submodule update --init --recursive
git submodule status --recursive
```

This updates the application and restores its recorded engine version. It does not select the newest engine release. A detached HEAD inside the engine is normal for a pinned dependency. Configure and build never fetch or upgrade Git dependencies.

To adopt the latest published engine commit deliberately:

```sh
git switch -c update/engine
git -C gui_engine fetch origin
git -C gui_engine switch --detach origin/main
# Configure, build, and test this application.
git diff --submodule=log
git add gui_engine
git commit -m "Update MyGui engine dependency"
```

The application records the commit at `origin/main` when you make the update. Review and merge that dependency bump as an ordinary application change. Use `git submodule update --init --recursive` to restore the recorded dependency for a build.

To roll back a bad upgrade, revert its application commit and restore the dependency:

```sh
git revert <dependency-bump-commit>
git submodule update --init --recursive
```

Do not reset or force-push shared history to roll back a dependency.

## Branches, releases, and shared fixes

Both repositories use a stable `main` branch with short-lived `feature/*`, `fix/*`, or `update/*` branches. The engine and application have independent branch histories.

Applications pin engine commits. When describing release compatibility, use these versioning conventions:

- **Patch:** compatible bug fixes.
- **Minor:** compatible features and optional settings.
- **Major:** intentional C++/CMake, persistence, or style-contract breaks, with migration instructions.

Patch/minor releases must keep existing `gui_src` and style JSON working. New widgets should reuse visual roles or provide compatible defaults for optional additions. Engine upgrades do not replace the application's styling or backend. Review compatibility when changing `mygui/gui.hpp`, `MyGui::Engine`, `MyGui.Engine`, or required style fields.

For a shared fix discovered in an application:

```sh
git switch -c fix/example-integration
git -C gui_engine switch -c fix/grouped-card-layout
# Edit shared engine files and add an engine regression test.
git -C gui_engine add <changed-engine-files>
git -C gui_engine commit -m "Fix grouped card layout"
# Build and test the application against this engine commit.
```

Commit engine changes in the inner repository and application changes in the outer one. The application tracks the engine's commit pointer, not copied engine source files. Create a named branch before changing a detached checkout. Commit or deliberately preserve engine work before switching dependency revisions.

When publication is authorized, publish the engine branch first and merge it into engine `main`. Then update and publish the application's gitlink to a commit others can fetch.

For the initial publication, push the engine from PowerShell first.

```powershell
cd 'C:\Users\Malte\Desktop\Coding\my_gui_example\gui_engine'
git push -u origin main
```

After the engine push succeeds, push the example and its recorded submodule commit.

```powershell
cd 'C:\Users\Malte\Desktop\Coding\my_gui_example'
git push --recurse-submodules=check -u origin main
```

Do not run these commands until authorized and the engine remote is accessible. Empty remotes need no force push. If either remote gains history before publication, fetch and inspect it before integrating.

After CI has run on GitHub, protect `main` with pull requests and the appropriate passing checks, keeping force pushes disabled. Visibility, permissions, branch protection, and GitHub releases are separate administrative actions; local Git configuration does not change them.

## Build, tests, and ignored files

Consumers default to `MYGUI_ENGINE_BUILD_TESTS=OFF`. The example's test presets explicitly enable both the generic engine suite and example-specific checks.

- Engine tests run independently from `gui_engine/`, using their own neutral style fixtures. Engine source does not depend on `../gui_src/` or the calculator backend.
- Example tests verify its calculator and actual showcase startup against the pinned dependency and application styling.
- Shared behavior changes need engine regressions and consuming-application checks where applicable.
- CI checks out submodules recursively and tests the recorded commit, rather than silently following the latest engine `main`.

The supplied GitHub Actions workflows cover Ubuntu 24.04 with Qt 6.9.3 and Windows with MSYS2 UCRT64's current Qt. Both run Debug tests under `gui_testing/.artifacts/cmake/ci/`. Action references are pinned to upstream release commit IDs. The example recursively checks out its pinned engine; a private engine needs read-access configuration before that hosted checkout can succeed. Hosted CI has not been run by the local-only repository preparation.

Each repository ignores its own generated output:

| Rule | Purpose |
| --- | --- |
| `/.generated/` | CMake work and development executables. |
| `/build/` | Release executables and deployed runtime assets. |
| `/gui_testing/.artifacts/` | Test builds, reports, captures, and temporary data. |
| `/CMakeUserPresets.json`, `*.user` | Machine-specific configuration. |

Track `.gitmodules`, `CMakePresets.json`, production styling, test fixtures, and the engine gitlink. Do not ignore `gui_engine/` or blanket-ignore `*.ini`: configuration templates and INI test fixtures may be intentional. Saved application data belongs under its configured data root, outside these repositories.

After a future directory rename, reconfigure in fresh generated folders. CMake caches contain absolute paths and must not be reused under the new name. Git's relative submodule metadata remains valid when the parent checkout moves as a unit.

## Another project, including Tindeq UI

After engine publication, add it directly to another application:

```sh
git submodule add git@github.com:xd7rsxr5mw-pixel/my_gui_engine.git gui_engine
git -C gui_engine switch --detach origin/main
git add .gitmodules gui_engine
git commit -m "Add MyGui engine dependency"
```

The consuming CMake project finds Qt, calls `add_subdirectory(gui_engine)`, and uses `my_gui_add_application` with its own sources, styling folder, and backend target. See [the engine guide](../gui_engine/README.md) for the complete CMake example. Add the engine repository, not the showcase repository, as the dependency.

For **Tindeq UI**, migrate separately in testable stages:

1. Add the engine submodule and a separate preview executable while preserving the existing application for comparison.
2. Keep Bluetooth, protocol parsing, force acquisition, domain storage, and saved data in Tindeq's layers. Put its workflow definitions, styling, and adapters in its own `gui_src`/application layer.
3. Connect Tindeq data through public engine state/models/callbacks. Keep device, athlete, exercise, hangboard, workout, and result concepts out of the engine.
4. Put genuinely reusable behavior fixes in the engine, add regressions, and update Tindeq's engine pointer after its application and hardware checks pass.
5. Replace the old frontend only after feature parity is demonstrated. Review saved-data changes as a separate migration.

This repository split does not migrate or modify the current Tindeq UI project.
