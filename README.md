
# kvick-db

A simple Key value store built with CMake and Docker.

## Requirements

- Docker installed  
- WSL 2 (if using Windows)

## Build

```bash
docker build -t kvick-db .
```

## Run

```bash
docker run -it --rm -p 8080:8080 kvick-db
```

Access the DB at [http://localhost:8080](http://localhost:8080).

## Test

Test the DB using the test-client.py
