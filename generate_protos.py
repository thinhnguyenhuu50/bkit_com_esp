import os
from SCons.Script import Import
from SCons.Environment import Environment

env: Environment  # type: ignore
Import("env")

# 1. Define paths
# PlatformIO downloads libraries to .pio/libdeps, so we find the generator there
project_dir = env.subst("$PROJECT_DIR")
lib_deps_dir = env.subst("$PROJECT_LIBDEPS_DIR")
environment = env.subst("$PIOENV")

# Path to the nanopb_generator.py inside the downloaded library
nanopb_generator = os.path.join(
    lib_deps_dir, 
    environment, 
    "Nanopb", 
    "generator", 
    "nanopb_generator.py"
)

# Input/Output directories
proto_dir = os.path.join(project_dir, "proto")
output_dir = os.path.join(project_dir, "src")

# 2. Function to generate files
def generate_proto():
    # Find all .proto files in the proto directory
    if not os.path.exists(proto_dir):
        print(f"[Nanopb] Warning: Proto directory '{proto_dir}' not found.")
        return

    proto_files = [f for f in os.listdir(proto_dir) if f.endswith(".proto")]
    
    if not proto_files:
        print("[Nanopb] No .proto files found.")
        return

    print(f"[Nanopb] Found {len(proto_files)} proto file(s). Generating C code...")

    for proto_file in proto_files:
        source = os.path.join(proto_dir, proto_file)
        
        # Construct the command
        # Syntax: python generator.py -I include_path -D output_path source_file
        cmd = f'python "{nanopb_generator}" -I "{proto_dir}" -D "{output_dir}" "{source}"'
        
        # Execute the command
        exit_code = env.Execute(cmd)
        
        if exit_code != 0:
            print(f"[Nanopb] Error generating code for {proto_file}!")
            env.Exit(1) # Stop the build on error

# 3. Run the generator
generate_proto()