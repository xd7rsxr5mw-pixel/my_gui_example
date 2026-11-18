# my_gui_example development notes

- This is the application showcase for the independent Qt 6.9+ / C++23 MyGui engine on Windows and Linux.
- The repository identity is `my_gui_example`; the local folder currently remains `My_GUI` at the user's request. Do not rename it until authorized again.
- Work only inside this project. The sibling Tindeq_UI project is a read-only reference.
- `gui_engine` is a Git submodule with its own history, instructions, and remote. It owns reusable rendering, input behavior, models, persistence, Qt adapters, the public C++ facade, and generic engine tests.
- `gui_src` composes the showcase using the public API. Its styling files own all visual values.
- `backend` contains plain C++ calculations without Qt dependencies.
- Root `gui_testing` owns example-specific backend/startup tests and their artifacts. Generic engine tests and neutral fixtures belong in `gui_engine/gui_testing`.
- Keep normal runnable application output under `build`; CMake work/debug artifacts belong under `.generated`.
- Do not put test executables, logs, screenshots, or debug symbols in `build`.
- Keep all application data namespaced to its application ID; tests use isolated data roots.
- Use apply_patch for manual edits. Never edit generated/copied QML.
- The parent repository records an exact engine commit. Do not upgrade or fetch the engine as part of a normal configure/build.
- Create a named engine branch before editing a detached submodule checkout. Commit shared changes in the engine repository, then commit the tested engine pointer in this repository.
- Use `main` with short-lived `feature/*` and `fix/*` branches. Preserve source API and existing style-file compatibility for engine patch/minor releases.
- Keep remotes configured but do not push or publish unless the user authorizes it. When publication is authorized, publish the engine commit/tag before a parent commit that references it.
- See `docs/GIT_INTEGRATION.md` for the repository, release, update, and future Tindeq migration workflow.
