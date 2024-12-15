# Tic-Tac-Toe

> 🇧🇷 Para ver este arquivo em português, clique [aqui](./README-pt.md)

This repository contains three components:

- An open-source implementation of the classic Tic-Tac-Toe game, playable via the command line (CLI).
- A "malicious" version of the game that secretly uploads a file to a server without the user's knowledge.
- A server designed to receive files from the "malicious" game.

**This software is not an actual malware.** It is designed for educational purposes, specifically to demonstrate malware reverse engineering in a controlled and safe environment. While this is not actual malware, it mimics malicious behavior without employing any obfuscation or concealment techniques. This transparency makes it ideal for learning how to identify and analyze potentially harmful software.

This was originally implemented to be used for [ThaySolis](https://github.com/ThaySolis)'s presentation for the 2024 Hacking na Web Day - São Paulo edition (HNWD 2024 - São Paulo), and was also presented in the Menina de Cybersec (MSC) Village, as part of the 2024 Hackers to Hackers Conference (H2HC 2024).

To maximize the educational value, I strongly recommend not looking at the source code. Instead, build the project (see the **Building and Deploying** section below) and attempt to reverse engineer it yourself.

This software has been tested on Windows and on Linux.

## Building an deploying

The "malicious" version of this software consists of two parts:
1. The game: A modified Tic-Tac-Toe implementation that uploads files to a server.
2. The server: A program that receives files uploaded by the malicious game.

To set up the project, you need to:
- Deploy the server to a public-facing environment.
- Update the game's source code to point to the deployed server.
- Build the game.

Once completed, the malicious game will be ready for reverse engineering exercises.

### Server

The server code is located in the server folder. It listens for connections on port 8000 by default, although you can modify this in the source code if needed.

To start the server:
1. Deploy it to a public-facing machine or cloud environment.
2. Run the following command:

```bash
python3 server.py
```

### Client

The game code is located in the `game` folder.

#### Requirements

Ensure the following tools and libraries are installed and available in your `PATH`:

- `gcc`
- `make`
- `libcurl4-openssl-dev`

#### Configurations

The game folder contains a file named `mal_settings.h`, which defines the value of `BASE_URL`. Edit this file and update `BASE_URL` to point to your server’s URL, including the IP address (and port if changed).

#### Building

To build the game, run:

```bash
make  
```

This will produce two executables:
- `tictactoe`: The standard version of the game, without any "malicious" functionality.
- `tictactoe_mal`: The "malicious" version, which reads a file and uploads it to the server without the user's knowledge.

## License
This project is licensed under the MIT License - see the [LICENSE](./LICENSE) file for details.
