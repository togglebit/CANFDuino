import subprocess
import os
from mcp.server.fastmcp import FastMCP


mcp = FastMCP("Arduino_Controller")

#SKETCH_PATH = os.path.abspath("mcp_sketch/mcp_sketch.ino")
#SKETCH_PATH = os.path.join(os.path.dirname(__file__), "mcp_sketch", "mcp_sketch.ino")
BASE = r"C:\Users\Admin\AppData\Local\Programs\Arduino IDE"
SKETCH_PATH = os.path.join(BASE, "mcp_sketch", "mcp_sketch.ino")
CLI = os.path.join(BASE, "arduino-cli.exe")




@mcp.tool()
def write_sketch(code: str) -> str:
    """Writes C++ code to the local Arduino sketch file."""
    os.makedirs(os.path.dirname(SKETCH_PATH), exist_ok=True)
    with open(SKETCH_PATH, "w") as f:
        f.write(code)
    return f"Sketch written to {SKETCH_PATH}"

@mcp.tool()
def upload_to_arduino(fqbn: str, port: str) -> str:
    """Compiles and uploads the current sketch. 
    Example FQBN: 'arduino:avr:uno', Port: 'COM3' or '/dev/ttyUSB0'
    """
    SKETCH_DIR = os.path.dirname(SKETCH_PATH)
    try:
        # Compile
        compile_cmd = f'"{CLI}" compile --fqbn {fqbn} --output-dir "{SKETCH_DIR}" "{SKETCH_DIR}"'
        subprocess.run(compile_cmd, shell=True, capture_output=True)
        
        # Upload
        upload_cmd = f'"{CLI}" upload -p {port} --fqbn {fqbn} --input-dir "{SKETCH_DIR}"'
        result = subprocess.run(upload_cmd, shell=True, capture_output=True, text=True)
        return "Upload successful!"
    except subprocess.CalledProcessError as e:
        return f"Error: stdout={e.stdout} stderr={e.stderr} returncode={e.returncode}"

@mcp.tool()
def list_ports() -> str:
    """Lists all available serial ports and connected Arduino boards."""
    cmd = f'"{CLI}" board list'
    result = subprocess.run(cmd, shell=True, capture_output=True, text=True)
    return result.stdout if result.stdout else "No ports found"

if __name__ == "__main__":
    mcp.run()

#debug, these two CLI's work
#arduino-cli compile --fqbn arduino:avr:uno --output-dir "C:\Users\Admin\AppData\Local\Programs\Arduino IDE\mcp_sketch" "C:\Users\Admin\AppData\Local\Programs\Arduino IDE\mcp_sketch"
#arduino-cli upload -p COM5 --fqbn arduino:avr:uno --input-dir "C:\Users\Admin\AppData\Local\Programs\Arduino IDE\mcp_sketch"
