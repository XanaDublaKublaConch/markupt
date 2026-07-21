# markupt

A lightweight, native KDE Plasma 6 Markdown viewer. View-only, fast cold
start, syntax-highlighted code fences, native Plasma theming, single-instance
with tabs.

See `markdown-viewer-spec.md` for the full technical spec.

## Installing

### Arch / CachyOS (or other Arch-based distros)

```bash
sudo pacman -S --needed git cmake extra-cmake-modules qt6-base kdbusaddons syntax-highlighting kwindowsystem breeze-icons

git clone git@github.com:XanaDublaKublaConch/markupt.git
cd markupt/packaging
makepkg -si
```

`makepkg -si` builds the package and installs it after resolving
dependencies. After install, `.md` files should show "Markdown Viewer" in
Dolphin's Open With list; if the association doesn't appear immediately,
run `kbuildsycoca6` or log out/in.

### Other KDE Plasma distros

Build directly with CMake (package names vary by distro):

```bash
sudo <pkg-manager> install cmake extra-cmake-modules qt6-base-dev kdbusaddons-dev \
    kf6-syntax-highlighting-dev kwindowsystem-dev breeze-icons

git clone git@github.com:XanaDublaKublaConch/markupt.git
cd markupt
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr
cmake --build build
sudo cmake --install build
```

### Dependencies

Runtime: `qt6-base`, `kdbusaddons`, `syntax-highlighting` (KSyntaxHighlighting),
`kwindowsystem`, `breeze-icons`.

Build-time: `cmake`, `extra-cmake-modules`, a C++20 compiler.

## Usage

```bash
markdownviewer somefile.md
```

Opening additional files reuses the running window as tabs. Refresh the
active tab with `F5` or `Ctrl+R`.
