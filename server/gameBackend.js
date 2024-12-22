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
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        2, 2, 1, 1, 2, 2, 1, 1, 2, 2,
        2, 2, 1, 1, 2, 2, 1, 1, 2, 2,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0
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
}

class Game{
    static Grid = Grid;

    constructor(roomID){
        this.roomID = roomID;

        this.playerCount = 0;
        this.players = {};

        this.turn = Math.random() > 0.5 ? "red" : "blue";
        this.gameBoard = new Grid();
    }

    join(socket){
        if(this.playerCount >= 2){
            throw new Error("Room is full");
        }
        else if (this.playerCount === 1){
            const player = this.players[Object.keys(this.players)[0]];
            player.socket.emit("gameFull", "Player 2 has joined the game");

            player.socket.on("startGame", () => {
                this.#startGame();
            });
        }

        socket.join(this.roomID);

        this.players[socket.id] = {
            socket: socket,
            color: this.playerCount === 0 ? "red" : "blue"
        };

        this.playerCount++;

        return true;
    }

    #startGame(){
        const LOBBY_MODE = 0;
        const PLACE_MODE = 1;
        const PLAY_MODE = 2;
        
        for(const playerName in this.players){
            const player = this.players[playerName];
            player.socket.emit("changeGameState", PLACE_MODE);

            if (player.color === "red"){
                let board = this.gameBoard.board.forEach((value, index) => {
                    if(value & Grid.TEAM_BLUE){
                        board[index] = Grid.HIDDEN_PIECE;
                    }
                });
                
                player.socket.emit("setBoard", board);

            } else if (player.color === "blue"){
                let board = this.gameBoard.board.forEach((value, index) => {
                    if(value & Grid.TEAM_RED){
                        board[index] = Grid.HIDDEN_PIECE;
                    }
                });

                //Flip board 
                board = board.flipVertical();

                player.socket.emit("setBoard", board);
            }
             
        }
    }
}

module.exports = Game;