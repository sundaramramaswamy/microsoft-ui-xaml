# ListView neighbor states sample

Two grouped `ListView`s over the same data, styled purely in XAML from the
item's own and its neighbors' visual states. No view model tracks selection
runs, corners or separator visibility.

| | Exp1: grouped | Exp2: continuous |
|-|-|-|
| Row lines | straight, inset | squiggly, full width |
| Selection box and pill | join within a group | join across groups |
| Group headers | name; separator above all but the first | collapsed; a tag marks each group's first item |

## States used

| Group | States | Owner |
|-|-|-|
| `CommonStates` | `Normal`, `PointerOver`, `Selected`, … | `ListViewItem` |
| `PreviousItemStates` | `NoPreviousItem`, `Previous{Unselected,Selected}{SameGroup,OtherGroup}` | `ListViewItem` |
| `NextItemStates` | `NoNextItem`, `Next{Unselected,Selected}{SameGroup,OtherGroup}` | `ListViewItem` |
| `GroupPositionStates` | `FirstGroup`, `SubsequentGroup` | `ListViewHeaderItem` |

Each state group owns distinct properties, so groups never fight:

* `CommonStates` toggles the `SelectedLayer` and `UnselectedLayer`.
* `PreviousItemStates` owns the top halves of the box and pill.
* `NextItemStates` owns the bottom halves and the row line in each layer.

A row line shows only when this item and the next share selection, which
hides it at selection boundaries.

## Build and run

From the repo root, after building MUX:

```powershell
.\initrun.ps1 .\pack.component.cmd /version 3.0.0-<tag>
.\initrun.ps1 msb /q /restore Samples\ListViewNeighborStatesApp\ListViewNeighborStatesApp.csproj /p:Platform=x64 /p:WinUIVersion=3.0.0-<tag>
BuildOutput\obj\amd64chk\Samples\ListViewNeighborStatesApp\ListViewNeighborStatesApp.exe
```

Use a fresh `<tag>` per pack; NuGet caches packages by version.
