import { createServer } from 'http';
import { Server } from 'socket.io';

const httpServer = createServer();
const io = new Server(httpServer, {
  cors: {
    origin: '*',
  },
});

const players = {};

io.on('connection', (socket) => {
  console.log(`Client connected: ${socket.id}`);
  players[socket.id] = {id: socket.id};
  console.log(`Players connected now: ${Object.keys(players).length}`)
  socket.on('playerUpdate', (data) => {
    // handle incoming player update
    players[socket.id] = data;
    socket.broadcast.emit('playerUpdate', data);
    
  });

  socket.on('disconnect', () => {
    console.log(`Client disconnected: ${socket.id}`);

    delete players[socket.id];
    console.log(`Players connected now: ${Object.keys(players).length}`)
    socket.broadcast.emit('playerDisconnect', socket.id);
  });

  socket.on('chat', (data) => {
    console.log(`${data.id}: ${data.message}`);
    socket.broadcast.emit('chat', data);
  })
});

httpServer.listen(3001, () => {
  console.log('Server listening on port 3001');
});