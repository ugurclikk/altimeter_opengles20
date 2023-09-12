# OpenGL ES 2.0 Altimeter


An altimeter application built using OpenGL ES 2.0, demonstrating altitude visualization and interactive controls.

## Description

The OpenGL ES 2.0 Altimeter project showcases an altimeter application that utilizes OpenGL ES 2.0 for rendering altitude data in a visually appealing and interactive manner. The application features a circular background with altitude indicators and interactive arrow indicators for different altitude levels.

## Screenshots 
![WhatsApp Image 2023-08-18 at 16 53 26](https://github.com/ugurclikk/altimeter_opengles20/assets/75498822/a1686cad-3100-4c34-865a-84773089f18c)

## VIDEO

[Screencast from 27-08-2023 09_39_37.webm](https://github.com/ugurclikk/altimeter_opengles20/assets/75498822/715949a6-4004-4804-96e5-33000792143e)



## Technologies Used

- OpenGL ES 2.0
- GLFW
- GLEW
- GLM

## Features

- **Altitude Visualization**: A circular background with altitude indicators provides a visual representation of altitude changes.
- **Interactive Controls**: Arrow indicators can be controlled using keyboard inputs, allowing users to simulate altitude changes.
- **Dynamic Movement**: The circular background can be rotated interactively, enhancing the user experience.
- **Modular Design**: The codebase is organized into functions for drawing the circular backgrounds, altitude indicators, and managing user input.



### Shaders

The application uses vertex and fragment shaders to process and render graphical elements. The shaders are responsible for texturing, color blending, and determining which parts of the textures to display.

### Functions

- `drawinnercircle`: Renders the inner circular background, applying a texture for a detailed visual representation.
- `drawoutercircle`: Renders the outer circular background, applying a texture for a stylized appearance.
- `drawStick`: Draws altitude indicators representing different altitudes.
- `updateAltitude`: Monitors keyboard inputs for altitude changes and updates the display accordingly.
- `init`: Initializes GLFW and sets up the application window.
- `main`: The main loop that manages rendering, user input, and interaction.


Usage and Contributions
Feel free to use this project as a starting point for your own OpenGL ES 2.0 applications or contribute improvements and features. Pull requests are welcome!
## How to Run

1. **Install Dependencies**:
   Make sure you have the required libraries installed: GLFW, GLEW, GLM.

2. **Compile the Code**:
   Run the following command in the terminal to compile the code:
   ```bash
   g++ -o main altimeter.cpp -lglfw -lGLESv2 -lEGL
   
3- **Run the Application**:
    Execute the compiled binary to launch the altimeter application:
   ```bash
   ./main

