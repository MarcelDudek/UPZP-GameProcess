# UPZP game sub-process

## Build

Build tested on Ubuntu 20.04 64-bit.

### Build dependencies

```bash
sudo apt install libasio-dev
sudo snap install flatbuffers
sudo apt install libmysqlcppconn-dev
sudo apt install libmysqlclient-dev
```

By default CMake is expecting ASIO and Flatbuffers libraries to be available in following folders:
```
/usr/include/asio
/snap/flatbuffers/current/include
```

### Building

In order to build the sub-process:
```bash
cmake -DCMAKE_BUILD_TYPE=Release -G "CodeBlocks - Unix Makefiles" CMakeLists.txt
cmake --build . --target UPZP_GameProcess -- -j 3
```

## Run arguments

| Argument | Meaning | Default |
| -------- |:--------|:-------:|
| `--udp port` | port for communication with clients via UDP | 8850 |
| `--tcp port` | port for communication with main process via TCP | 8860 |
| `--lat_start latutude` | latitude of map center in degrees| 17.034299 |
| `--long_start longitude` | longitude of map center in degrees | 51.109385 |
| `--radius radius`  | map radius in meters | 1000 |
| `--map name` | map name that will be sent to database | WROCLAW |
| `--id value` | game ID which will be sent to database (must be unique) | 0x0 |
| `--win_points points` | Points required for a team to win | 1000 |
| `--spawn_period period` | Period in seconds between point boxes spawns | 10 |

## MySQL Statistics database

For a MySQL database communication via SSL `./crt` folder is required with files `client-cert.pem`, `client-key.pem`, `server-ca.pem`.