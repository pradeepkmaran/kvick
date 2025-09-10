# kvick-db

A simple Key-Value store built with CMake and Docker.

## Requirements

- Docker installed  
- WSL 2 (if using Windows)

## Build

```bash
docker build -t kvick-db .
```

## Run

You can run multiple instances of the service on different ports.

For example, to run two instances on ports 8080 and 8081:

```bash
 docker run -it --rm -p 8080:8080 -e PORT=8080 -v "$(pwd)/data":/app/data kvick-db 
```

```bash
 docker run -it --rm -p 8081:8081 -e PORT=8080 -v "$(pwd)/data":/app/data kvick-db 
```

Each instance will be accessible at:

- [http://localhost:8080](http://localhost:8080)
- [http://localhost:8081](http://localhost:8081)

## Test

You can test the service using the provided `test-client.py` script.

For concurrency testing, send simultaneous requests to the same port (e.g., 8080) and verify that shared data is handled correctly.

Make sure to update the script or run multiple instances to simulate concurrent access.

---
