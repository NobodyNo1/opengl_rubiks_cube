# Get the directory path of the script
script_dir="$(cd "$(dirname "$0")" && pwd)"

# Execute the program with the directory path as an argument
./build/mac_rubiks_cube "$script_dir"