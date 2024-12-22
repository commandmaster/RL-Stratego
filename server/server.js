const express = require('express');
const http = require('http');
const { Server } = require('socket.io');
const path = require('path');
const Game = require('./gameBackend.js');


const app = express();
const port = 3000;

// Create an HTTP server and bind it to the Express app
const server = http.createServer(app);

// Attach Socket.IO to the server
const io = new Server(server);

// Serve static files from the 'public' directory
app.use(express.static(path.join(__dirname, 'public')));

// Serve stratego.html as the default file
app.get('/', (req, res) => {
    res.sendFile(path.join(__dirname, 'public', 'stratego.html'));
});

let roomIdIndex = 0;
let games = new Map();

io.on('connection', (socket) => {
    console.log('A user connected');

    socket.on("createGame", () => {
        console.log("Creating game");
        const roomID = "room-" + roomIdIndex++;
        const game = new Game(roomID);
        socket.emit("roomCode", roomID);
        console.log("Room ID: " + roomID);  
        games.set(roomID, game);
        game.join(socket);
    });

    socket.on("joinGame", (roomID) => {
        // Append 'room-' if not already present
        if (!roomID.startsWith("room-")) {
            roomID = "room-" + roomID;
        }

        const game = games.get(roomID);
        
        if (!game) {
            socket.emit("invalidRoomID", "Invalid room ID");
            return;
        }
        game.join(socket);
    });



    socket.on('message', (data) => {
        console.log('Message from client:', data);
    });

    socket.on('test', (data) => {
        console.log('test message: ' + data);
    });

    socket.on('disconnect', () => {
        console.log('A user disconnected');

        for (const [roomID, game] of games) {
            if (game.players[socket.id]) {
                game.playerCount--;
                delete game.players[socket.id];
                socket.leave(roomID);

                if (game.playerCount === 0) {
                    games.delete(roomID);
                }
            }
        }
    });
});

// Start the server
server.listen(port, () => {
    console.log(`Server running at http://localhost:${port}/`);
});
