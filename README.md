
# C Web Server for Windows



A lightweight, multi-threaded HTTP server built in C for the Windows operating system. This server uses the Windows Sockets API (Winsock) to handle incoming network connections and is capable of serving static web content.

## Features

*   **HTTP/1.1 Compliant**: Handles basic `GET` requests from clients.
*   **Multi-threaded**: Utilizes Windows threads to manage multiple client connections concurrently without blocking.
*   **Static File Serving**: Serves local files such as HTML, CSS, JavaScript, and images.
*   **MIME Type Detection**: Automatically determines the correct `Content-Type` for common web file formats.
*   **Error Handling**: Responds with appropriate HTTP status codes, including `404 Not Found` for missing files.
*   **Lightweight**: Built with standard C libraries and the native Windows API for minimal overhead.

## Prerequisites

To build and run this project, you will need:
*   A Windows operating system.
*   A C compiler installed and configured for your system, such as **MinGW** (GCC) or **Microsoft Visual C++**.

## How to Build and Run

1.  **Clone the Repository**:
    Make sure all project files (`server1.c`, `index.html`, `about.html`, `img.jpg`) are in the same directory.

2.  **Compile the Code**:
    Open a command prompt or terminal in the project directory and compile the server source code.

    If you are using **GCC (MinGW)**, run the following command. The `-lws2_32` flag is necessary to link the Winsock library.
    ```sh
    gcc server1.c -o webserver.exe -lws2_32
    ```

    If you are using the **Microsoft Visual C++ compiler (cl.exe)**, you might use a command like this from a Developer Command Prompt:
    ```sh
    cl server1.c /link /out:webserver.exe Ws2_32.lib
    ```

3.  **Run the Server**:
    Once compilation is successful, an executable file (`webserver.exe`) will be created. Run it from the terminal:
    ```sh
    .\webserver.exe
    ```
    You should see a confirmation message that the server is running:
    ```
    Server is running on http://localhost:8080
    ```

4.  **Access the Content**:
    Open your web browser and navigate to the following URLs to see your static pages:
    *   **Home Page**: `http://localhost:8080/index.html`
    *   **About Page**: `http://localhost:8080/about.html`
    *   **Image**: `http://localhost:8080/img.jpg`

## Project File Structure

*   `server1.c`: The main source file containing all the logic for the web server, including socket setup, listening for connections, and handling client requests in separate threads.
*   `index.html`: The default landing page for the website.
*   `about.html`: A secondary page providing more information.
*   `img.jpg`: An image asset that can be requested from the server.
*   `webserver.exe` (after compilation): The final executable program.
