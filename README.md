*This project has been created as part of the 42 curriculum by yaboukir, ilallali, obouftou.*

# ft_irc

## Description
**ft_irc** is a small, single-server Internet Relay Chat (IRC) daemon implemented in C++.  
The goal of the project is to learn and demonstrate low-level network programming (BSD sockets), non-blocking I/O with `poll()`, protocol parsing/validation (RFC-like behavior), and server state management (clients, channels, modes). This implementation supports core registration flow, channel management, messaging, moderation commands, and several bonus features (bot, simple DCC-like file transfer initiation).

Key focuses:
- building a stable event-driven server,
- correct IRC registration and command handling,
- safe resource management (non-blocking sockets, clean shutdown),
- modular design (Server / Client / Channel separation).

---

## Instructions

### Requirements
- POSIX-compatible system (Linux/macOS).  
- C++ compiler (g++/clang++) — see project constraints for required standard.  
- `make` to build.

### Compilation
From repository root:
```bash
make
```
- This builds the server binary (./ircserv) and compiles mandatory and if you want to make bonus do:
```bash
make bonus
```
- to run the server :
```bash
./ircserv <port> <password>
```
- Example:
```bash
./ircserv 6969 w
```
- port must be between 1024 and 65535.
- password is required by clients with the PASS command given in the server param!

### Connecting with a client
- You can Connect with netcat, or with limechat (limechat is available in macos)
```bash 
nc -C localhost 6969
```
- Registration example via netcat (raw commands after connecting):
```bash
PASS w
NICK tester
USER usertester 0 * :tester Realname
```
- 0 and * are ignored by RFC (hostname and servername)

### Test Commands
- try to join channel:
```bash
join #room
```
- send private message to a person:
```bash
privmsg user : hello!
```
- set topic to a channel:
```bash
topic #room : this is topic test
```
_**and all other commands like : mode invite part...**_

## Project Structure
```bash
.
├── bonus/
│ └── bot.cpp
│
├── headers/
│ ├── channel.hpp
│ ├── client.hpp
│ ├── commands.hpp
│ ├── replies.hpp
│ └── server.hpp
│
├── mandatory/
│ ├── channel.cpp
│ ├── client.cpp
│ ├── commands.cpp
│ ├── handleauth.cpp
│ ├── invite_command.cpp
│ ├── join_command.cpp
│ ├── kick_command.cpp
│ ├── mode_command.cpp
│ ├── privmsg_command.cpp
│ ├── server.cpp
│ └── topic_command.cpp
│
├── main.cpp
├── Makefile
└── README.md
```

## Resources

### we take rfc 1459 as a reference to our irc chat
#### here is all resources:

- <https://www.rfc-editor.org/rfc/rfc1459.html>
- <https://ircdocs.horse/tools/wstester>
- <https://www.geeksforgeeks.org/computer-networks/socket-in-computer-network/>
- <https://www.ibm.com/docs/en/i/7.4.0?topic=ssw_ibm_i_74/apis/socket.html>
- <https://modern.ircdocs.horse/>
- <https://www.youtube.com/watch?v=gntyAFoZp-E>
- <https://youtu.be/-utm73RxNo4?si=_Bm7xqNM9V2JZR5_>

#### AI Usage:
ChatGPT were used to assist for:

- clarifying RFC behavior and edge cases (registration order, authentication, ...)

- drafting and polishing README content and usage examples

- suggesting testing approaches (valgrind, leaks, memory-monitoring snippets).

- All design decisions, and final validation were implemented and reviewed manually by the authors. AI was used as an assistant for research, examples, and  explanations only.


## Feature List

### Implemented Features

#### Mandatory Features

- TCP server using BSD sockets.
- Non-blocking I/O with `poll()` for multiplexing multiple clients.
- Client connection handling (`accept`, socket configuration, IP detection).
- Complete registration system: `PASS`, `NICK`, `USER`.
- Strict registration validation and correct numeric replies.
- Channel creation and membership management (`JOIN`).
- Private messaging (`PRIVMSG`) — user-to-user and user-to-channel.
- Topic management (`TOPIC`).
- Correct numeric replies and error handling (e.g. `001`, `451`, `433`, `461`).
- Proper message buffering and handling of partial `recv()` (`\r\n` parsing).
- Resource cleanup: closing file descriptors and deleting allocated client objects.
- Clean event-driven architecture (Server / Client separation).

#### Bonus Features

- Full `MODE` implementation (channel modes and subcommands).
- `KICK` and `INVITE` command handling.
- Simple IRC bot implementation.
- DCC-like file transfer initiation handled via `PRIVMSG` control messages (simplified FTP-like behavior).

---

## Authors & Contributions

### yaboukir
- Core server architecture.
- Client management.
- `PRIVMSG` and `TOPIC` implementation.
- Registration flow.
- Socket configuration and `poll()` event loop.
- Host/IP handling and main logic structure.

### obouftou
- Complete `MODE` implementation (all required subcommands).
-  `PART` `JOIN` `KICK` and `INVITE` commands.
- Channel moderation logic.

### ilallali
- Bonus `BOT` implementation.
- Automation features and `BOT` behavior.