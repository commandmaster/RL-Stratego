class Grid {
    static EMPTY = 0;
    static BLOCKED = 1;
    static UNPLACEABLE = 2;
    static FLAG = 3;
    static SPY = 4;
    static SCOUT = 5;
    static MINER = 6;
    static SERGEANT = 7;
    static LIEUTENANT = 8;
    static CAPTAIN = 9;
    static MAJOR = 10;
    static COLONEL = 11;
    static GENERAL = 12;
    static MARSHAL = 13;
    static BOMB = 14;
    static HIDDEN_PIECE = 15;

    // Two most significant bits are used to store the team
    static TEAM_RED = 0x80;
    static TEAM_BLUE = 0x40;

    static TEAM_MASK = 0xC0;

    static COLUMNS = 10;
    static ROWS = 10;

    board = new Uint8Array([
        Grid.EMPTY, Grid.EMPTY, Grid.EMPTY, Grid.EMPTY, Grid.EMPTY, Grid.EMPTY, Grid.EMPTY, Grid.EMPTY, Grid.EMPTY, Grid.EMPTY,
        Grid.EMPTY, Grid.EMPTY, Grid.EMPTY, Grid.EMPTY, Grid.EMPTY, Grid.EMPTY, Grid.EMPTY, Grid.EMPTY, Grid.EMPTY, Grid.EMPTY,
        Grid.EMPTY, Grid.EMPTY, Grid.EMPTY, Grid.EMPTY, Grid.EMPTY, Grid.EMPTY, Grid.EMPTY, Grid.EMPTY, Grid.EMPTY, Grid.EMPTY,
        Grid.EMPTY, Grid.EMPTY, Grid.EMPTY, Grid.EMPTY, Grid.EMPTY, Grid.EMPTY, Grid.EMPTY, Grid.EMPTY, Grid.EMPTY, Grid.EMPTY,
        2, 2, 1, 1, 2, 2, 1, 1, 2, 2,
        2, 2, 1, 1, 2, 2, 1, 1, 2, 2,
        Grid.EMPTY, Grid.EMPTY, Grid.EMPTY, Grid.EMPTY, Grid.EMPTY, Grid.EMPTY, Grid.EMPTY, Grid.EMPTY, Grid.EMPTY, Grid.EMPTY,
        Grid.EMPTY, Grid.EMPTY, Grid.EMPTY, Grid.EMPTY, Grid.EMPTY, Grid.EMPTY, Grid.EMPTY, Grid.EMPTY, Grid.EMPTY, Grid.EMPTY,
        Grid.EMPTY, Grid.EMPTY, Grid.EMPTY, Grid.EMPTY, Grid.EMPTY, Grid.EMPTY, Grid.EMPTY, Grid.EMPTY, Grid.EMPTY, Grid.EMPTY,
        Grid.EMPTY, Grid.EMPTY, Grid.EMPTY, Grid.EMPTY, Grid.EMPTY, Grid.EMPTY, Grid.EMPTY, Grid.EMPTY, Grid.EMPTY, Grid.EMPTY,
    ]);

    getValueAt(row, column) {
        if (row < 0 || row >= Grid.ROWS || column < 0 || column >= Grid.COLUMNS) {
            throw new Error("Invalid position");
        }
        return this.board[row * Grid.COLUMNS + column];
    }

    setValueAt(row, column, value) {
        if (row < 0 || row >= Grid.ROWS || column < 0 || column >= Grid.COLUMNS) {
            throw new Error("Invalid position");
        }
        this.board[row * Grid.COLUMNS + column] = value;
    }

    printBoard() {
        for (let row = 0; row < Grid.ROWS; row++) {
            console.log(
                this.board.slice(row * Grid.COLUMNS, (row + 1) * Grid.COLUMNS).join(" ")
            );
        }
    }

    flipVertical() {
        for (let row = 0; row < Grid.ROWS / 2; row++) {
            for (let column = 0; column < Grid.COLUMNS; column++) {
                const temp = this.getValueAt(row, column);
                this.setValueAt(row, column, this.getValueAt(Grid.ROWS - row - 1, column));
                this.setValueAt(Grid.ROWS - row - 1, column, temp);
            }
        }
    }

    flipHorizontal() {
        for (let row = 0; row < Grid.ROWS; row++) {
            for (let column = 0; column < Grid.COLUMNS / 2; column++) {
                const temp = this.getValueAt(row, column);
                this.setValueAt(row, column, this.getValueAt(row, Grid.COLUMNS - column - 1));
                this.setValueAt(row, Grid.COLUMNS - column - 1, temp);
            }
        }
    }

    static flipVertical(board) {
        const newBoard = new Uint8Array(board);
        for (let row = 0; row < Grid.ROWS / 2; row++) {
            for (let column = 0; column < Grid.COLUMNS; column++) {
                const temp = newBoard[row * Grid.COLUMNS + column];
                newBoard[row * Grid.COLUMNS + column] = newBoard[(Grid.ROWS - row - 1) * Grid.COLUMNS + column];
                newBoard[(Grid.ROWS - row - 1) * Grid.COLUMNS + column] = temp;
            }
        }
        return newBoard;
    }

    static flipHorizontal(board) {
        const newBoard = new Uint8Array(board);
        for (let row = 0; row < Grid.ROWS; row++) {
            for (let column = 0; column < Grid.COLUMNS / 2; column++) {
                const temp = newBoard[row * Grid.COLUMNS + column];
                newBoard[row * Grid.COLUMNS + column] = newBoard[row * Grid.COLUMNS + Grid.COLUMNS - column - 1];
                newBoard[row * Grid.COLUMNS + Grid.COLUMNS - column - 1] = temp;
            }
        }
        return newBoard;
    }

    static rotateClockwise(board, times = 1) {
        const newBoard = new Uint8Array(board);
        for (let i = 0; i < times; i++) {
            for (let row = 0; row < Grid.ROWS / 2; row++) {
                for (let column = row; column < Grid.COLUMNS - row - 1; column++) {
                    const temp = newBoard[row * Grid.COLUMNS + column];
                    newBoard[row * Grid.COLUMNS + column] = newBoard[(Grid.COLUMNS - column - 1) * Grid.COLUMNS + row];
                    newBoard[(Grid.COLUMNS - column - 1) * Grid.COLUMNS + row] = newBoard[(Grid.ROWS - row - 1) * Grid.COLUMNS + Grid.COLUMNS - column - 1];
                    newBoard[(Grid.ROWS - row - 1) * Grid.COLUMNS + Grid.COLUMNS - column - 1] = newBoard[column * Grid.COLUMNS + Grid.COLUMNS - row - 1];
                    newBoard[column * Grid.COLUMNS + Grid.COLUMNS - row - 1] = temp;
                }
            }
        }
        return newBoard;
    }
    
    static getBlueBoard(grid) {
        const newBoard = grid.board.slice();

        // Make the red pieces hidden, rotate the board 180 degrees, and make the red side unplaceable
        for (let i = 0; i < newBoard.length; i++) {
            if (newBoard[i] & Grid.TEAM_RED) {
                newBoard[i] = Grid.HIDDEN_PIECE | Grid.TEAM_RED;
            } else if (i >= 60 && i < 100 && newBoard[i] == Grid.EMPTY) {
                newBoard[i] = Grid.UNPLACEABLE;
            }
        }

        return Grid.rotateClockwise(newBoard, 2);
    }

    static getRedBoard(grid) {
        const newBoard = grid.board.slice();

        // Make the blue pieces hidden and the blue side unplaceable
        for (let i = 0; i < newBoard.length; i++) {
            if (newBoard[i] & Grid.TEAM_BLUE) {
                newBoard[i] = Grid.HIDDEN_PIECE | Grid.TEAM_BLUE;
            }
            else if (i >= 0 && i < 40 && newBoard[i] == Grid.EMPTY) {
                newBoard[i] = Grid.UNPLACEABLE;
            }

        }

        return newBoard;
    }
}

class PieceInfo
{
    pieces = [];

    constructor()
    {
        for (let i = 0; i < 15; i++)
        {
            this.pieces.push({name: "", rank: 0, count: 0});
        }

        this.pieces[Grid.FLAG] = {name: "Flag", rank: 0, count: 1};
        this.pieces[Grid.SPY] = {name: "Spy", rank: 1, count: 1};
        this.pieces[Grid.SCOUT] = {name: "Scout", rank: 2, count: 8};
        this.pieces[Grid.MINER] = {name: "Miner", rank: 3, count: 5};
        this.pieces[Grid.SERGEANT] = {name: "Sergeant", rank: 4, count: 4};
        this.pieces[Grid.LIEUTENANT] = {name: "Lieutenant", rank: 5, count: 4};
        this.pieces[Grid.CAPTAIN] = {name: "Captain", rank: 6, count: 4};
        this.pieces[Grid.MAJOR] = {name: "Major", rank: 7, count: 3};
        this.pieces[Grid.COLONEL] = {name: "Colonel", rank: 8, count: 2};
        this.pieces[Grid.GENERAL] = {name: "General", rank: 9, count: 1};
        this.pieces[Grid.MARSHAL] = {name: "Marshal", rank: 10, count: 1};
        this.pieces[Grid.BOMB] = {name: "Bomb", rank: 11, count: 6};
    }

    canPlacePiece(piece, grid, row, column)
    {
        if (grid.getValueAt(row, column) != Grid.EMPTY || grid.getValueAt(row, column) == Grid.UNPLACEABLE)
        {
            console.log("Cannot place piece here");
            return false;
        }

        if (this.pieces[piece].count == 0)
        {
            console.log("No more pieces of this type");
            return false;
        }

        console.log("Piece can be placed");
        return true;
    }
}

class Game{
    static Grid = Grid;

    static PLACE_MODE = 0;
    static MOVE_MODE = 1;

    constructor(io, roomID){
        this.io = io;
        this.roomID = roomID;

        this.playerCount = 0;
        this.players = {};

        this.mode = Game.PLACE_MODE;

        this.turn = Math.random() > 0.5 ? "red" : "blue";
        this.gameBoard = new Grid();
    }

    join(socket){
        if(this.playerCount >= 2){
            throw new Error("Room is full");
        }
        else if (this.playerCount === 1){
            const host = this.players[Object.keys(this.players)[0]];
            const hostSocket = this.io.sockets.sockets.get(host.socketID);  

            hostSocket.on("startGame", () => {
                this.#startGame();
            });

            this.players[socket.id] = {
                socketID: socket.id,
                color: this.playerCount === 0 ? "red" : "blue",
                host: false,
                name: String(socket.id),
                pieceInfo: new PieceInfo(),
            };

            socket.join(this.roomID);
            this.io.to(this.roomID).emit("gameFull", JSON.stringify(Object.values(this.players)));
        }
        else{
            socket.join(this.roomID);
            this.players[socket.id] = {
                socketID: socket.id,
                color: this.playerCount === 0 ? "red" : "blue",
                host: true,
                name: String(socket.id),
                pieceInfo: new PieceInfo(),
            };
        }


        const player = this.players[socket.id];

        socket.emit("requestName", "");
        socket.on("setName", (name) => {
            if(name === "") 
            {
                socket.emit("nameTaken", String(player.socketID));
            }

            for(const playerName in this.players){
                if(this.players[playerName].name === name && playerName !== socket.id){
                    socket.emit("nameTaken", String(player.socketID));
                    this.players[socket.id].name = String(socket.id);
                    socket.to(this.roomID).emit("renameOtherPlayer", String(socket.id));
                    console.log("Name is already taken");
                    return;
                }
            }

            player.name = name;
            socket.to(this.roomID).emit("renameOtherPlayer", name);
        });

        

        this.playerCount++;

        return true;
    }

    #startGame(){
        const LOBBY_MODE = 0;
        const GAME_MODE = 1;
        
        for(const playerId in this.players){
            const player = this.players[playerId];
            const playerSocket = this.io.sockets.sockets.get(player.socketID);


           playerSocket.on("deletePiece", (message) => {
                const data = JSON.parse(message);
                const row = data.row;
                const column = data.column;

                if(this.mode !== Game.PLACE_MODE){
                    return;
                }

                if (player.color === "red")
                {
                    if (this.gameBoard.getValueAt(row, column) & Grid.TEAM_RED)
                    {
                        let piece = this.gameBoard.getValueAt(row, column) & ~Grid.TEAM_RED;
                        player.pieceInfo.pieces[piece].count++;
                        this.gameBoard.setValueAt(row, column, Grid.EMPTY);
                        playerSocket.emit("setBoard", JSON.stringify(Array.from(Grid.getRedBoard(this.gameBoard))));
                        playerSocket.to(this.roomID).emit("setBoard", JSON.stringify(Array.from(Grid.getBlueBoard(this.gameBoard))));
                    }
                }
                else
                {
                    if (this.gameBoard.getValueAt(9 - row, 9 - column) & Grid.TEAM_BLUE)
                    {
                        let piece = this.gameBoard.getValueAt(9 - row, 9 - column) & ~Grid.TEAM_BLUE;
                        player.pieceInfo.pieces[piece].count++;
                        this.gameBoard.setValueAt(9 - row, 9 - column, Grid.EMPTY);
                        playerSocket.emit("setBoard", JSON.stringify(Array.from(Grid.getBlueBoard(this.gameBoard))));
                        playerSocket.to(this.roomID).emit("setBoard", JSON.stringify(Array.from(Grid.getRedBoard(this.gameBoard))));
                    }
                }

           }); 
            
            playerSocket.on("placePiece", (message) => {
                const data = JSON.parse(message);
                const piece = data.piece;

                if(this.mode !== Game.PLACE_MODE){
                    return;
                }

                if (player.color === "red")
                {
                    const row = data.row;
                    const column = data.column;

                    let gameBoard = new Grid();
                    gameBoard.board = Grid.getRedBoard(this.gameBoard);
                    let canPlace = player.pieceInfo.canPlacePiece(piece, gameBoard, row, column);
                    if (canPlace)
                    {
                        this.gameBoard.setValueAt(row, column, piece | Grid.TEAM_RED);
                        player.pieceInfo.pieces[piece].count--;

                        //playerSocket.to(this.roomID).emit("setBoard", JSON.stringify(Array.from(Grid.getRedBoard(this.gameBoard))));
                        playerSocket.emit("setBoard", JSON.stringify(Array.from(Grid.getRedBoard(this.gameBoard))));
                        playerSocket.to(this.roomID).emit("setBoard", JSON.stringify(Array.from(Grid.getBlueBoard(this.gameBoard))));
                    }
                }
                else{
                    const row = data.row;
                    const column = data.column;

                    let gameBoard = new Grid();
                    gameBoard.board = Grid.getBlueBoard(this.gameBoard);
                    let canPlace = player.pieceInfo.canPlacePiece(piece, gameBoard, row, column);
                    if (canPlace)
                    {
                        this.gameBoard.setValueAt(9 - row, 9 - column, piece | Grid.TEAM_BLUE);
                        player.pieceInfo.pieces[piece].count--;

                        playerSocket.emit("setBoard", JSON.stringify(Array.from(Grid.getBlueBoard(this.gameBoard))));
                        playerSocket.to(this.roomID).emit("setBoard", JSON.stringify(Array.from(Grid.getRedBoard(this.gameBoard))));
                    }
                }
            });

            playerSocket.on("ready", () => {
                player.ready = true;
                playerSocket.to(this.roomID).emit("playerReady", player.color);

                if(this.players[Object.keys(this.players)[0]].ready && this.players[Object.keys(this.players)[1]].ready){
                    console.log("Both players are ready");

                    this.io.to(this.roomID).emit("playGame", String(GAME_MODE));
                    this.#sendBoard(this.players[Object.keys(this.players)[0]]);
                    this.#sendBoard(this.players[Object.keys(this.players)[1]]);

                    this.mode = Game.MOVE_MODE;
                }
            });

            playerSocket.on("unReady", () => {
                player.ready = false;
                playerSocket.to(this.roomID).emit("playerUnReady", player.color);
            });

            playerSocket.emit("changeGameState", String(GAME_MODE));

            playerSocket.on("boardLoaded", (message) => {
                this.#sendBoard(player);
            });
        }
    }

    #sendBoard(player){
        const playerSocket = this.io.sockets.sockets.get(player.socketID);

        if(player.color === "red"){
            playerSocket.emit("setBoard", JSON.stringify(Array.from(Grid.getRedBoard(this.gameBoard))));
        }
        else if(player.color === "blue"){
            playerSocket.emit("setBoard", JSON.stringify(Array.from(Grid.getBlueBoard(this.gameBoard))));
        }
    }


}

module.exports = Game;