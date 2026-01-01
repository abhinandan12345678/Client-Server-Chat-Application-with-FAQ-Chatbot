# FAQ_D1 Application

FAQ_D1 is a client-server chat application with an integrated FAQ chatbot feature, designed to facilitate peer-to-peer communication and provide automated responses to frequently asked questions.

## Prerequisites

Before you begin, ensure you have met the following requirements:
- Linux Operating System
- CMake version 3.27 or higher
- Python 3.x
- GCC/G++ for C compilation

## Setting Up and Running the Application

### Server and Client Compilation

1. Clone the repository to your local machine.
2. Navigate to the project directory where the `CMakeLists.txt` file is located.
3. Run the following commands to compile the server and client applications:
   ```sh
   cmake ..
   make
   ```
   This will generate two executables: `FAQ_D1` for the server and `FAQ_D1_C` for the client.

### Configuring the Python Environment
Note: This part is optional because the server file will execute the setup script automatically. However, if you want to set up the Python environment manually, follow the steps below.

The server requires a Python virtual environment to run the FAQ chatbot feature. A setup script, `setup_venv.sh`, is provided to automate this process.

1. Ensure the `setup_venv.sh` script is in the root directory of your project.
2. Make the script executable by running:
   ```sh
   chmod +x setup_venv.sh
   ```
3. Execute the script to create and configure the virtual environment:
   ```sh
   ./setup_venv.sh
   ```
   This script checks for the existence of a virtual environment directory (`venv`), creates it if it doesn't exist, activates it, installs the necessary Python dependencies listed in `requirements.txt`, and then deactivates the environment.

### Testing the python script (Optional)
To test the python script, open the terminal make sure you have run the `setup_venv.sh` script. Then run the following command:
```sh
venv/bin/python3 gpt-inference.py "Morning, how are you doing?"
```
On the first try it may take a while to load the model, but after that it should be quick.
That's why it is recommended to run the python script first, so that the model is loaded before starting the server and client.

### Running the Server

After setting up the Python environment, start the server application by running:
```sh
./FAQ_D1
```
This command initializes the server, sets up the Python environment, and waits for client connections.

### Connecting a Client

Open a new terminal window on the root directory. Start a client application by running:
```sh
./FAQ_D1_C
```
Follow the task instructions to interact with the server and other connected clients.

## Features

- Peer-to-peer chat functionality.
- FAQ chatbot for automated responses.
- Multiple client support with unique identifiers.
- Chat history management.

For more information on how to use the chat and chatbot commands, refer to the instruction file you provided.

To set up and run your application in CLion, a popular IDE for C/C++ development, you'll need to follow specific steps that account for building the C application, setting up the Python virtual environment, and running the Python model for the FAQ chatbot. Below is a guide to help you with this process after unzipping the folder containing your project files.

### Setting Up the Project in CLion

1. **Open CLion and Import the Project**:
   - Launch CLion.
   - Select "Open" and navigate to the unzipped project folder.
   - Choose the project directory and click "Open as Project".

2. **Configure the CMake Project**:
   - CLion should automatically detect the `CMakeLists.txt` file and prompt you to load the CMake project. If it doesn't, you can manually trigger the CMake project load by going to "File" > "Reload CMake Project".
   - Once CMake configuration is complete, you'll have both your server (`FAQ_D1`) and client (`FAQ_D1_C`) applications set up in the project.

### Configuring the Python Virtual Environment

Before running the server application, you must set up the Python virtual environment and install the required dependencies.

1. **Open the Terminal in CLion**:
   - Navigate to the terminal window within CLion, usually located at the bottom of the IDE.

2. **Run the `setup_venv.sh` Script**:
   - In the terminal, make the `setup_venv.sh` script executable by running:
     ```bash
     chmod +x setup_venv.sh
     ```
   - Execute the script to create and configure the virtual environment:
     ```bash
     ./setup_venv.sh
     ```
   This script will create a virtual environment, activate it, install the necessary dependencies from `requirements.txt`, and then deactivate the environment.

### Running the Server and Python Model

After setting up the virtual environment, you can run the server application and the Python model.

1. **Build the Project**:
   - Use the "Build" menu or the toolbar buttons to build the project in CLion. This compiles both the server and client applications.

2. **Run the Server**:
   - To run the server, select `FAQ_D1` from the run configurations dropdown menu in the toolbar and click the run button. This starts the server application.
   - If the server application requires the Python model to be loaded and run separately, ensure that your server is properly invoking the Python script or model as designed.

3. **Run the Python Model**:
   - If the server application does not automatically load the Python model, you may need to run the Python model manually.
   - Activate the virtual environment through the terminal in CLion:
     ```bash
     source venv/bin/activate
     ```
   - Run the Python model script using:
     ```bash
     python gpt-inference.py
     ```
    - This will load the model and make it ready for inference.
    - You can test the model by providing input text and observing the responses.
    - Once the model is running, you can proceed to connect clients to the server.
   
### Connecting a Client

- Open a new terminal in CLion or use a separate terminal window.
- Build and run the client application by selecting `FAQ_D1_C` from the run configurations and clicking the run button.
- Interact with the server through the client application as intended.

Remember to check and adjust paths or specific commands based on your project's structure and requirements. This guide assumes a standard setup and might need modifications to fit your project's specific needs.
