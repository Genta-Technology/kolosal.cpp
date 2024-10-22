# ImGui Chatbot Application

## Overview
Kolosal AI Desktop application let you train and run any LLM easily on device with easy to use UI. Enabling you to easily download models or loras from huggingface hub or kolosal hub.

## Prerequisites
- C++17 or later
- CMake 3.10 or later
- OpenGL (3.3+)
- GLFW
- GLAD
- ImGui

## Installation

### Step 1: Clone the Repository
```bash
git clone https://your-repo-url/kolosal-desktop.git
cd kolosal-desktop
```

### Step 2: Set Up External Libraries
The `external` folder includes the required external dependencies like GLFW, GLAD, and ImGui. Make sure these are placed in the correct folder structure as shown.

### Step 3: Build the Project
1. **Create a Build Directory**:
   ```bash
   mkdir build
   cd build
   ```
2. **Run CMake**:
   ```bash
   cmake ..
   ```
3. **Build the Application**:
   ```bash
   cmake --build .
   ```

### Step 4: Run the Application
After a successful build, you can run the application from the build directory:

```bash
./KolosalDesktop
```

## Project Structure

```plaintext
kolosal-desktop/
├── CMakeLists.txt        # CMake build configuration
├── main.cpp              # Main application code
└── external/             # External libraries
    ├── fonts/            # Fonts folder (Inter-Regular.ttf included)
    ├── glfw/             # GLFW library for window and input handling
    ├── glad/             # GLAD for OpenGL function loading
    └── imgui/            # ImGui library for UI rendering
```

## Customization
### Font
The font used in this project is `Inter-Regular.ttf`. You can replace this font by placing a new `.ttf` file in the `external/fonts/` folder and updating the font path in the `CMakeLists.txt`.

### Input Field Styling
The input field is customizable through ImGui's style variables, such as padding and rounding. Modify the `setInputFieldStyle()` function in `main.cpp` to change the appearance.

### Message Rendering
Message bubbles are styled based on whether they are user messages or bot responses. You can customize the colors and layout in the `renderMessage()` function in `main.cpp`.

## License
This project is licensed under the Apache 2.0 License.

## Contributions
Feel free to open issues or submit pull requests if you'd like to contribute to improving the application.
