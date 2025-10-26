
#!/bin/bash

# Defaults


show_help() {
    cat << EOF
Usage: ${0##*/} [OPTIONS]

Options:
  -h, --help       Show this help message
  -r, --run        Run the executable after building 
EOF
}

run_after_build=false

# Parse arguments manually
while [[ $# -gt 0 ]]; do
    case "$1" in
        -h|--help)
            show_help
            exit 0
            ;;
        -r|--run)
            run_after_build=true
            shift
            ;;
        *)
            echo "Unknown option: $1"
            show_help
            exit 1
            ;;
    esac
done

# Build steps
mkdir -p build
cd build
cmake ..
cmake --build .

# Run the executable if requested
if $run_after_build; then
    echo "Attempting to Run UI..."
    # Replace this with your actual executable name
    ./DebugBoardUI
fi
