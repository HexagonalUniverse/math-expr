:: <run-tests.bat>

@echo off

:: Scoping the working directory
cd /d %~dp0

:: Creating the virtual environment
if not exist ".venv/" (
    echo Creating VENV
    python -m venv ".venv/"
)

:: Activating it
call ".venv/Scripts/activate.bat"

:: Installing the dependencies
pip install --quiet -r "dependencies.txt"

:: Running the script
python scripts/test.py

:: Deactivating the virtual environment
call ".venv/Scripts/deactivate.bat"
