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
