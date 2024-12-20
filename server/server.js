const express = require('express');
const http = require('http');
const { Server } = require('socket.io');
const path = require('path');

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

// Handle WebSocket connections
io.on('connection', (socket) => {
    console.log('A user connected');

    // Send a message to the client
    socket.emit('message', 'Welcome to the Socket.IO server!');

    // Listen for messages from the client
    socket.on('message', (data) => {
        console.log('Message from client:', data);
    });

    socket.on('test', (data) => {
        console.log('test message: ' + data);
    });

    // Handle disconnection
    socket.on('disconnect', () => {
        console.log('A user disconnected');
    });
});

// Start the server
server.listen(port, () => {
    console.log(`Server running at http://localhost:${port}/`);
});
