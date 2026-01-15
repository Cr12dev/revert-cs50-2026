# REVERT

**REVERT** is a lightweight, cross-platform image viewer and editor developed in C using SDL2. Originally designed for Harvard's CS50 2026, it provides a simple interface for viewing and making basic modifications to images.

<div align="center">
  <img src="https://github.com/Cr12dev/revert-cs50-2026/blob/main/screenshots/1.png" width="80%"/>
  <p><i>REVERT V.1.0.0 Interface</i></p>
</div>

## ✨ Features

- **Advanced Processors:**
  - **Normal Maps:** Generate surface normal maps in real-time from any image using Sobel-based edge detection.
  - **Plugin System:** Robust, cross-platform architecture (`.so`/`.dll`) to add new filters without recompiling the main app.
- **Enhanced UI Components:**
  - **Checkboxes:** Interactive "toggle" components for feature activation (e.g., Normal Map mode).
  - **Backup & Reset:** Instant restoration to the original image state when disabling filters.
- **Interactive Editing:**
  - **Drawing Mode:** Toggle drawing to add annotations or "red points" to your images.
  - **Scaling:** Dynamic scaling (1x, 1.5x, 2x) with auto-fit capabilities.
  - **Built-in Filters:** Apply Grayscale, Sepia, and Invert filters in real-time.
- **Developer Tools:**
  - **Plugin API:** Simple C-based API for creating external processing modules.

## 🚀 Getting Started

### Prerequisites

You will need the following libraries installed on your system:

- **SDL2**
- **SDL2_image**
- **SDL2_ttf**
- **libm** (usually included with gcc)
- **CMake** (v3.10+)

#### Ubuntu/Debian:

```bash
sudo apt-get install libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev cmake
```

#### Windows (x64):

1. Install [Visual Studio](https://visualstudio.microsoft.com/) with C++ support.
2. It is recommended to use [vcpkg](https://github.com/microsoft/vcpkg) for dependencies:
   ```powershell
   vcpkg install sdl2 sdl2-image sdl2-ttf
   ```
3. Run CMake specifying the vcpkg toolchain.

### Installation & Build

1. **Clone the repository:**

   ```bash
   git clone https://github.com/Cr12dev/revert-cs50-2026.git
   cd revert-cs50-2026
   ```

2. **Build the project:**

   ```bash
   mkdir build
   cd build
   cmake ..
   make  # This builds the main app and the included plugins.
   ```

3. **Run the application:**
   ```bash
   ./sdl_image_renderer path/to/your/image.bmp
   ```

## 🔌 Plugin System

REVERT supports dynamic plugins. To use a plugin:

1. Compile your plugin as a shared library (`.so` or `.dll`).
2. Place it in the same directory as the executable.
3. Open the **Plugins** panel in-app to see and apply your new filters.

## 🛠️ Utils `/utils`

The project includes a Python utility to generate test `.bmp` images.
**Requires:** Python 3 & Pillow (PIL).

```bash
python3 ./utils/create_test_image.py
```

## 🧪 Testing

The project includes a suite of automated tests written in Python to verify coordinate mapping, image loading, and basic CLI behavior.

**Requirements:** Python 3 & Pillow.

To run the tests:

```bash
python3 tests/test_graphics.py
```

## 🗺️ Roadmap

- [x] Initial `.bmp` support
- [x] Support for `.png` and `.jpg` (via SDL_image)
- [x] Basic drawing tools
- [x] Image scaling system
- [x] Advanced filters (Grayscale, Sepia, Invert)
- [x] Sobel-based Normal Mapping
- [x] Dynamic Plugin Architecture
- [x] Backup & Reset system
- [ ] Multiple brush colors and sizes
- [ ] Undo/Redo functionality

## 📜 License

This project is licensed under the **GNU General Public License v3.0**. See the [LICENSE](LICENSE) file for details.

---

Developed with ❤️ by [Cr12dev](https://github.com/Cr12dev)
