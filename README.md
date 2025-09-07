
# kvick-app

A simple application built with CMake and Docker.

## Requirements

- Docker installed  
- WSL 2 (if using Windows)

## Build

```bash
docker build -t kvick-app .
```

## Run

```bash
docker run -it --rm -p 8080:8080 kvick-app
```

Access the app at [http://localhost:8080](http://localhost:8080).

## Test

Test the app using the test-client.py
