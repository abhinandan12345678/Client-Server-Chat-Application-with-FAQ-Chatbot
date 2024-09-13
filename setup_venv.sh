#!/bin/bash

# Name of the virtual environment directory
VENV_NAME="venv"

# Requirements file
REQUIREMENTS_FILE="requirements.txt"

# Check if the virtual environment directory exists
if [ ! -d "$VENV_NAME" ]; then
    echo "Virtual environment not found. Creating..."
    # Create the virtual environment
    python3 -m venv --system-site-packages $VENV_NAME
else
			echo "Virtual environment found."
fi

# Activate the virtual environment
source $VENV_NAME/bin/activate

# Check if the requirements file exists
if [ -f "$REQUIREMENTS_FILE" ]; then
    echo "Installing dependencies from $REQUIREMENTS_FILE..."
    # Install dependencies
    pip install -r $REQUIREMENTS_FILE
else
    echo "Requirements file not found. Skipping dependency installation."
fi

# Deactivate the virtual environment
deactivate
