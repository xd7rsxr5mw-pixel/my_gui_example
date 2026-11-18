#include "calculator.hpp"
#include <chrono>
#include <mygui/gui.hpp>
#include <vector>

using namespace std::chrono_literals;

namespace {
std::vector<gui::Point> points(const std::vector<showcase::Sample> &samples) {
  std::vector<gui::Point> result;
  result.reserve(samples.size());
  for (const auto &sample : samples)
    result.push_back({sample.x, sample.y});
  return result;
}
} // namespace

int main(int argc, char **argv) {
  gui::Application app("my_gui.showcase", "My GUI · Engine Showcase");
  app.styling(MYGUI_STYLE_DIRECTORY);
  const showcase::Calculator calculator;
  auto first = app.state("first", 2.0).persist();
  auto second = app.state("second", 3.0).persist();
  auto sum = app.state("sum", 0.0);
  auto message = app.state("message", "Ready to explore");
  auto showDetails = app.state("details", true).persist();
  auto name = app.state("name", "My project").persist();
  auto notes =
      app.state("notes", "Edit these notes. Only opted-in values are saved.")
          .persist();
  auto choice = app.state("choice", "Standard").persist();
  auto average = app.state("meanSquare", 0.0);

  // 01 — ordinary controls, reactive state, and a backend callback.
  auto controls = app.workflow("controls", "Controls & Calculations");
  auto inputs = controls.panel("inputs", "INPUTS & ACTIONS");
  auto columns = inputs.columns("inputsColumns");
  auto form = columns.column("form");
  form.input("projectName", "Project name", name);
  form.combo("mode", "Mode", choice, {"Standard", "Detailed", "Compact"});
  form.checkbox("showDetails", "Show additional information", showDetails);
  auto calculation =
      columns.column("calculation").highlight("calculator", "Calculator");
  calculation.numberInput("first", "First number", first);
  calculation.numberInput("second", "Second number", second);
  calculation
      .button("add", "Add numbers",
              [=] { sum.set(calculator.add(first.get(), second.get())); })
      .option("primary", true);
  calculation.readout("result", "Result", sum);
  auto feedback = controls.panel("feedback", "VALUES & CONDITIONAL CONTENT");
  feedback.readout("message", "Application message", message);
  feedback
      .text("help", "All controls here are constructed with C++ engine calls. "
                    "Invalid numeric input turns red, shakes, and returns to "
                    "its last valid value.")
      .visibleWhen(showDetails);
  feedback.textArea("notes", "Project notes", notes);
  feedback.button("update", "Update message", [=] {
    message.set("Hello, " + name.get() + " · " + choice.get());
  });

  // 02 — a catalogue, rich cards, and independent list selection.
  auto catalogue =
      app.listModel("catalogue",
                    {{.id = "alpha",
                      .title = "Alpha item",
                      .description = "A short, editable work item",
                      .descriptorLabel = "Priority:",
                      .descriptorValue = "High",
                      .fields = {{"duration", "Duration", "number", 7.0}}},
                     {.id = "beta",
                      .title = "Beta item",
                      .description = "A reusable item with a right descriptor",
                      .descriptorLabel = "Priority:",
                      .descriptorValue = "Normal",
                      .fields = {{"duration", "Duration", "number", 12.0}}},
                     {.id = "gamma",
                      .title = "Gamma item",
                      .description = "Select with Ctrl or Shift, then drag",
                      .descriptorLabel = "Priority:",
                      .descriptorValue = "Low",
                      .fields = {{"duration", "Duration", "number", 3.0}}}},
                    "items")
          .persist();
  auto chosen =
      app.listModel(
             "chosen",
             {{.id = "intro",
               .title = "Pinned introduction",
               .description = "Fixed rows cannot be removed or dragged",
               .fixed = true},
              {.id = "example",
               .title = "Grouped example",
               .description = "Expand the copied cards or edit their fields",
               .fields = {{"duration", "Duration", "number", 7.0},
                          {"label", "Label", "text", std::string("Editable")}},
               .copies = 3}},
             "items")
          .persist();
  auto lists = app.workflow("lists", "Lists & Composition");
  auto compose = lists.panel("compose", "CATALOGUE & SELECTED ITEMS");
  auto listColumns = compose.columns("listColumns");
  auto left = listColumns.column("catalogueColumn");
  left.list("catalogue", "AVAILABLE ITEMS", catalogue)
      .option("multiSelectable", true)
      .option("draggable", true)
      .on("add", [=] {
        catalogue.append(
            {.title = "New item",
             .description = "Created through a one-line callback"});
      });
  left.button("transfer", "Add selected →",
              [=] { catalogue.transferSelectedTo(chosen); });
  auto right = listColumns.column("chosenColumn");
  right.superList("chosen", "SELECTED ITEMS", chosen)
      .option("deletable", true)
      .option("confirmRemove", true);
  right.button("remove", "Remove selected", [=] { chosen.removeSelected(); });
  auto behavior = lists.panel("listHelp", "LIST INTERACTIONS");
  behavior.text(
      "instructions",
      "Ctrl/Shift: multi-select · Ctrl+A: select all · Ctrl+C/V: copy/paste. "
      "Drag cards to reorder or copy from the catalogue. Drag a removable card "
      "outside to remove it. Right-click for more actions.");
  behavior.text(
      "wheel",
      "Lists scroll under the pointer. A fresh outward wheel gesture waits 250 "
      "ms at a scrollable edge; lists that already fit hand off immediately.");
  behavior.button("duplicate", "Duplicate selected cards",
                  [=] { chosen.duplicateSelected(); });

  // 03 — static and streaming plots use the same generic X/Y series.
  auto reference = app.series("reference", "textBright");
  auto comparison = app.series("comparison", "neonViolet");
  auto live = app.series("live", "textBright");
  reference.setPoints(points(calculator.curve(1.0, 0.5)));
  comparison.setPoints(points(calculator.curve(0.65, 0.75)));
  auto streaming = app.state("streaming", false);
  auto stream = app.timer("signal", 16ms, [=](double time) {
    const auto sample = calculator.sample(time);
    live.append({sample.x, sample.y});
  });
  auto charts = app.workflow("charts", "Charts & Data");
  charts.panel("static", "STATIC DATA · TWO SERIES")
      .plot("curves", "Reference curves", {reference, comparison}, "X",
            "Amplitude");
  charts.panel("stream", "STREAMING DATA")
      .plot("signal", "Live generated signal", {live}, "Time (s)", "Amplitude",
            true);
  auto chartActions = charts.panel("actions", "DATA ACTIONS");
  auto actionRow = chartActions.row("buttons");
  actionRow
      .button("start", "Start stream",
              [=] {
                live.clear();
                stream.start();
                streaming.set(true);
              })
      .enabledWhen([=] { return !streaming.get(); });
  actionRow
      .button("stop", "Stop stream",
              [=] {
                stream.stop();
                streaming.set(false);
              })
      .enabledWhen(streaming);
  actionRow.button("clear", "Clear plot", [=] { live.clear(); });
  chartActions.readout("average", "Backend mean square", average);
  chartActions.button(
      "worker", "Calculate in background", [&app, calculator, average] {
        app.background(
            [=](std::stop_token stop) { return calculator.meanSquare(stop); },
            average);
      });

  // 04 — reusable dialogs, spotlight, and the three-row layout budget.
  auto dialog = app.dialog("editItem", "Edit project");
  dialog.input("name", "Project name", name);
  dialog.textArea("notes", "Notes", notes);
  dialog.combo("mode", "Mode", choice, {"Standard", "Detailed", "Compact"});
  dialog.list("items", "ITEMS", catalogue).option("importantSelection", true);
  auto dialogActions = dialog.row("actions");
  dialogActions.button("save", "Save", [&app, dialog] {
    app.saveSettings();
    dialog.close();
  });
  dialogActions.button("cancel", "Cancel", [dialog] { dialog.cancel(); });
  auto workspace = app.workflow("workspace", "Dialogs & Workspace");
  auto modalPanel = workspace.panel("dialogs", "OVERLAPPING WINDOWS");
  modalPanel.text(
      "intro", "Dialogs can contain the same controls, lists, cards, and plots "
               "as ordinary subwindows. Clicking outside does not close them.");
  modalPanel.button("open", "Open editor", [dialog] { dialog.open(); });
  auto editPanel = workspace.panel("edit", "EDIT SPOTLIGHT");
  auto highlight = editPanel.highlight("editBox", "Editable project settings");
  highlight.input("name", "Name", name);
  highlight.combo("mode", "Mode", choice, {"Standard", "Detailed", "Compact"});
  auto save =
      editPanel.button("save", "Save settings", [&app] { app.saveSettings(); });
  editPanel.button(
      "spotlight", "Highlight editable area",
      [&app, highlight, save] { app.spotlight({highlight, save}); });
  auto layoutRow = workspace.panelRow("layoutRow");
  auto layout = layoutRow.panel("layout", "LAYOUT & PERSISTENCE");
  layout.text("help",
              "Resize either sidebar, drag the gaps between subwindows, or "
              "close a subwindow with its chevron. These three panels share "
              "one viewport including all gaps. Settings can change the data "
              "folder or reset saved preferences.");
  layout.button("save", "Save now", [&app] { app.saveSettings(); });
  layoutRow.panel("split", "HORIZONTAL PANELS")
      .text(
          "help",
          "This row has two panels. Drag the gap to resize them horizontally; "
          "narrow layouts keep their contents usable with internal scrolling.");

  auto projects = app.explorerCategory("projects", "Projects");
  auto project = projects.item("current", "Current project");
  project.action("select", "Open controls",
                 [&app] { app.navigate("controls"); });
  project.action("edit", "Edit project", [dialog] { dialog.open(); });
  projects.item("example", "Example project")
      .action("view", "View workspace", [&app] { app.navigate("workspace"); });
  auto datasets = app.explorerCategory("datasets", "Datasets");
  datasets.item("curves", "Reference curves")
      .action("view", "View charts", [&app] { app.navigate("charts"); });
  datasets.item("stream", "Generated signal")
      .action("view", "Open streaming view",
              [&app] { app.navigate("charts"); });
  auto items = app.explorerCategory("items", "Items");
  items.item("catalogue", "Item catalogue")
      .action("select", "Open lists", [&app] { app.navigate("lists"); });
  app.status("Welcome · four workflows, one reusable engine");
  return app.run(argc, argv);
}
