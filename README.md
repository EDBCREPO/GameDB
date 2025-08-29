# GameDB

GameDB is a lightweight and efficient, real-time game server backend designed for multiplayer applications. Built on the nodepp framework, it provides a solid foundation for managing player sessions, game lobbies, and server-side interactions via WebSockets.

## Key Features

- **Real-time Player Management:** Handle player connections, disconnections, and state in real-time.
- **Secure Authentication:** Utilizes JWT (JSON Web Tokens) for player authentication and session validation.
- **Dynamic Lobbies:** Players can connect to a specific game, view a list of available servers (lobbies), and join or create new ones.
- **Messaging System:** Includes a built-in chat system for players within the same server.
- **Robust API:** A well-defined API router handles various client events and routes them to the appropriate game logic.

## Dependencies
This project relies on the following libraries and frameworks:

- **Nodepp:** An asynchronous, event-driven framework for building high-performance network applications.
- **Redis:** A Redis database instance (though its current implementation is minimal in the provided code).
- **Apify:** A proof of concept protocol made to handle thousand reaquest simultaneously over a single connection.

## API Endpoints
The API router handles the following events and routes. All communication is done over WebSockets with APIFY payloads.

| Event | Path | Description |
| CONNECT | /:tken/:pid | Establishes the initial player connection and authenticates the user with a JWT token. |
| PING | /:tken/:pid | A simple health check to verify a player's connection and token validity. |
| DISCONNECT | /:tken/:pid | Disconnects a player from their current game and server, cleaning up resources. |
| SERVER | /:tken/:pid/all/:offset/:limit | Retrieves a paginated and filtered list of all available game servers. |
| SERVER | /:tken/:pid/get/:sid | Retrieves detailed information about a specific game server. |
| SERVER | /:tken/:pid/join/:sid | Connects a player to a specified server. |
| SERVER | /:tken/:pid/unjoin | Disconnects a player from their current server. |
| SERVER | /:tken/:pid/new | Creates a new game server. |
| CHAT | /:tken/:pid | Broadcasts a chat message to all other players in the same server. |
| GAME | /:tken/:pid | Broadcasts a Player State to all other players in the same server. |

## Getting Started
To get the server up and running, ensure you have the nodepp framework and its dependencies installed. A Redis instance is also required.

## Future Improvements

- **Persistent Storage:** Fully implement Redis to store game state, player data, and server information, allowing for persistence across server restarts.
- **Client Library:** Develop a client-side library to simplify interaction with the API, abstracting away the WebSocket communication layer.