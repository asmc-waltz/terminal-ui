# Terminal UI
**Terminal UI** is a lightweight graphical interface designed for embedded
Linux systems. It is part of the **go001** project and serves as the primary
user interface running directly on DRM without any window manager.

---

## 🚀 Overview

Terminal UI is implemented using **LVGL (v9.2.2)** and optimized for direct
framebuffer rendering. It is designed with a **phone-like interface**, 
supporting smooth transitions, automatic rotation, and a dynamic settings
menu.  

The application communicates with the **System Manager (`sys-mgr`)** service
via **D-Bus**, allowing real-time synchronization of system configurations and
events.

---

## 🧩 Key Features

- **🎨 Modern Embedded UI**  
  Built on **LVGL** for rich, responsive UI with optimized flex/grid layouts.  
  Supports scalable size interpolation and orientation rotation.

- **⚙️ Settings Menu**  
  Structured and dynamic menu with auto show/hide bar and theme awareness.

- **📱 Rotation Support**  
  Logical and physical rotation (90°, 180°, 270°).  
  Adaptive alignment, scaling, and layout refresh for rotated objects.

- **🔌 D-Bus Communication**  
  Exchanges commands and signals with `sys-mgr` via **libdbus**.  
  Supports asynchronous method calls and event handling.

- **🧠 Performance**  
  No window manager or compositor required.  
  Direct DRM rendering ensures low latency and small memory footprint.

---

## 🧱 Architecture

```text

+------------------------------------------------------------------------------+
| OS (Yocto rootfs + Linux kernel)                                             |
|                                                                              |
|  +-------------------------------------------------+   +------------------+  |
|  |                  Terminal UI                    |   | OS services, VFS |  |
|  | +---------------+     +-----------------------+ |   +------------------+  |
|  | | LVGL Core     | --> | Rendering via DRM/KMS | |            ↑            |
|  | +---------------+     +-----------------------+ |            |            |
|  |         ↑                                       |            ↓            |
|  | +---------------+                               |   +------------------+  |
|  | | UI Components | <-------------------------------> |  System Manager  |  |
|  | +---------------+  DBus (libdbus)               |   +------------------+  |
|  |                                                 |                         |
|  +-------------------------------------------------+                         |
|                                                                              |
+------------------------------------------------------------------------------+

```

---

## 🧰 Build Instructions

### Prerequisites
- Yocto-built Linux (tested on **STM32MP157D-DK1**)
- **LVGL v9.2.2**
- **libdbus-1**
- **DRM/KMS** enabled
- **CMake** or **Makefile** build system

### Build Steps

```bash
git clone https://github.com/asmc-waltz/terminal-ui.git
cd terminal-ui
mkdir build && cd build
cmake .. -G "Unix Makefiles"
# Resolve all required dependencies
make -j$(nproc)
```

---

## ⚙️ Logging & Error Handling

- Uses **Linux-standard log levels** (`LOG_ERROR`, `LOG_INFO`, `LOG_DEBUG`, …)
- Follows **`errno.h`** for all return codes (`-EIO`, `-EINVAL`, `-ENOMEM`, …)

---

## ✅ Supported

- **D-Bus communication**
  Unified IPC layer for all UI ↔ sys-mgr
  interactions (commands, queries, events).
- **Layout (flex / grid)** with rotation support
  Layout engine respects logical rotation (90°/180°/270°) and
  reflows children accordingly.
- Widget rotation modes
  - Base objects / controls (buttons, sliders, containers): Logical Rotation
  - Text & symbols: Transform rotation — render content rotated as a visual
    transform without changing logical layout.
- **Nested menu widget**
  Hierarchical menu component with support for submenus,
- **On-screen keyboard with rotation**
  Virtual keyboard that adapts key layout and hit areas when device orientation
  changes (supports logical rotation).


---

## 🔮 Future Extensions
- Setting: Wi-Fi, Hotspot, Airplane, Bluetooth, Network, Cellular, GNSS...
- Menu breadcrumb/back navigation and auto show/hide behaviour.
- Add a Systemd service file for the application
  to start as a daemon within systemd
- Migrate from **Standard C library** to **glib** for
  improved maintainability and type safety
- ...
