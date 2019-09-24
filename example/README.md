# Sabre Example

Example of the sabre library

## Building and Running

### Libraries

SFML is required.

These can be installed from your project manager. For example, on Debian/ Ubuntu:

```sh
sudo apt install libsfml-dev
```

If this is not possible (eg windows), you can install these manually from their respective websites:

[SFML](https://www.sfml-dev.org/download.php)

### Linux

To build, at the root of the project:

```sh
sh scripts/build2.sh
```

To run, at the root of the project:

```sh
#run server
sh scripts/server.sh

#run clients
sh scripts/client.sh <player name>
```
