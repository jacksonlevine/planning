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

  socket.on('playerUpdate', (data) => {
    // handle incoming player update
    players[socket.id] = data;
    socket.broadcast.emit('playerUpdate', data);
  });

  socket.on('disconnect', () => {
    console.log(`Client disconnected: ${socket.id}`);
    delete players[socket.id];
    socket.broadcast.emit('playerDisconnect', socket.id);
  });
});

httpServer.listen(3001, () => {
  console.log('Server listening on port 3001');
});