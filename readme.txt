PREREQUISITES:

GCC (for compiling C programs)
Python 3.x (for Python scripts and managing virtual environments)

SETUP INSTRUCTIONS:
SETTING UP THE PYTHON ENVIRONMENT:

This project may require a Python virtual environment for both the server and client components, especially if they interact with Python scripts or libraries.

CREATE A VIRTUAL ENVIRONMENT:

Navigate to the project's root directory and run:
python3 -m venv venv

This command creates a new directory venv containing the virtual environment.

ACTIVATE THE VIRTUAL ENVIRONMENT:

On Unix or MacOS:
source venv/bin/activate

On Windows (Command Prompt):
venv\Scripts\activate.bat

You'll need to activate the virtual environment every time you work on the project in a new terminal session.

INSTALL REQUIRED PYTHON PACKAGES:
With the virtual environment activated, install the project's dependencies:
pip install -r requirements.txt

Compiling the C Programs

Ensure you have GCC installed and accessible from your terminal or command prompt.

COMPILE THE SERVER:

In the terminal, navigate to the directory containing the server's source code, then compile:
gcc -std=c11 -D_GNU_SOURCE -o FAQ_D1 server.c uuid4.c utils.c -I.

COMPILE THE CLIENT:

Compile the client program using a similar command:
gcc -std=c11 -D_GNU_SOURCE -o FAQ_D1_C client.c uuid4.c utils.c -I.

RUNNING THE APPLICATION:
SERVER:
Before running the server, ensure any required Python environment is activated if the server utilizes Python components.

Activate the Virtual Environment (if not already active):
source venv/bin/activate

Start the Server:
./FAQ_D1 <ip_address> <port number>

CLIENT:
If the client also interacts with Python, activate the virtual environment in a new terminal window.

Activate the Virtual Environment (in a new terminal, if needed):
source venv/bin/activate

Run the Client:
./FAQ_D1_C <ip_address> <port number>

TROUBLESHOOTING:

If encountering issues with Python scripts or dependencies, ensure the virtual environment is activated and all necessary packages are installed.
For compilation issues, verify GCC is installed correctly and paths to source files in compilation commands are accurate.
