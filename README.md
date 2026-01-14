# REVERT

**REVERT** is a lightweight, cross-platform image viewer and editor developed in C using SDL2. Originally designed for Harvard's CS50 2026, it provides a simple interface for viewing and making basic modifications to images.

<div align="center">
  <img src="https://github.com/Cr12dev/revert-cs50-2026/blob/main/screenshots/1.png" width="80%"/>
  <p><i>REVERT V.1.0.0 Interface</i></p>
</div>

## ✨ Features

- **Multi-format Support:** Load `.bmp`, `.png`, and `.jpg` files.
- **Interactive Editing:**
  - **Drawing Mode:** Toggle drawing to add annotations or "red points" to your images.
  - **Scaling:** Dynamic scaling (1x, 1.5x, 2x) with auto-fit capabilities.
- **Image Export:** Save your modifications directly to `.png`.
- **UI Components:** Built-in custom buttons and text rendering for a smooth user experience.
- **Responsive Layout:** Images are automatically centered and scaled to fit the window while maintaining aspect ratio.

## 🚀 Getting Started

### Prerequisites

You will need the following libraries installed on your system:

- **SDL2**
- **SDL2_image**
- **SDL2_ttf**
- **CMake** (v3.10+)

On Ubuntu/Debian:

```bash
sudo apt-get install libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev cmake
```

### Installation

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
   make
   ```

3. **Run the application:**
   ```bash
   ./sdl_image_renderer path/to/your/image.bmp
   ```

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
- [ ] Multiple brush colors and sizes
- [ ] Undo/Redo functionality
- [ ] Advanced filters (Grayscale, Sepia, etc.)

## 📜 License

This project is developed for educational purposes as part of Harvard's CS50. See the [LICENSE](LICENSE) file for details (if applicable).

---

Developed with ❤️ by [Cr12dev](https://github.com/Cr12dev)
