import { createServer } from 'http';
import { Server } from 'socket.io';
import SunCalc from 'suncalc';

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

function sigmoid(x, k, x0) {
  return 1 / (1 + Math.exp(-k * (x - x0)));
}


setInterval(() => {

  const now = new Date();
const times = SunCalc.getTimes(now, 38.7521, 121.2880);
const sunriseTime = times.sunrise.getTime();
const sunsetTime = times.sunset.getTime();
const dayLength = sunsetTime - sunriseTime;
  const centerTime = sunriseTime + (dayLength / 2);
  const secondsFromCenter = now.getTime() - centerTime;
  const brightness = sigmoid(secondsFromCenter/1000000, 0.002, centerTime/100000000000)*2;



  io.emit('brightness', {brightness});
  console.log(`Setting the brightness to ${brightness} at ${now.getHours() >= 12 ? now.getHours() - 12 : now.getHours()} ${now.getHours() >= 12 ? "PM" : "AM"}`);
}, 60 * 1000);