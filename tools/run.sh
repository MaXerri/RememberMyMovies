#!/usr/bin/env bash
set -euo pipefail

echo "--- Starting MovieReviewApp ---"

# Activate Python environment
source backend/.venv/bin/activate

# Start backend server in the background
echo "--- Starting backend server ---"
uvicorn backend.app:app --host 0.0.0.0 --port 8000 --workers 1 &
SERVER_PID=$!
echo "Backend server started with PID: $SERVER_PID"

# Function to clean up (kill the server)
cleanup() {
    echo "--- Shutting down backend server (PID: $SERVER_PID) ---"
    kill $SERVER_PID
    wait $SERVER_PID || true
}

# Set a trap to run the cleanup function on script exit
trap cleanup EXIT

# Wait for the backend to be ready
echo "--- Waiting for backend server to be ready ---"
ATTEMPTS=0
MAX_ATTEMPTS=30
until curl -s -f http://0.0.0.0:8000/docs > /dev/null; do
    if [ $ATTEMPTS -ge $MAX_ATTEMPTS ]; then
        echo "Error: Backend server did not become ready in time."
        exit 1
    fi
    ATTEMPTS=$((ATTEMPTS + 1))
    echo "Waiting for backend... (attempt $ATTEMPTS/$MAX_ATTEMPTS)"
    sleep 1
done

# Run the frontend application
echo "--- Launching frontend application ---"
OS="$(uname -s)"
case "$OS" in
    Darwin)
        # On macOS, 'open' with -W waits until the app is closed.
        open -W build/MovieReviewApp.app
        ;;
    Linux)
        build/MovieReviewApp
        ;;
    *)
        echo "Unsupported OS: $OS"
        exit 1
        ;;
esac

echo "--- Application closed ---"
