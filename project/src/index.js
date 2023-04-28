import React from 'react';
import ReactDOM from 'react-dom/client';
import App from './components/App';
import "firebase/app"; 
import firebase from "firebase/compat/app";
import "firebase/compat/database";
import { getAuth, signInAnonymously, onAuthStateChanged } from "firebase/auth";

const firebaseConfig = {
  apiKey: (process.env.NODE_ENV === 'development' ?
  process.env.REACT_APP_apiKeyDev : process.env.REACT_APP_apiKeyProd),
  authDomain: process.env.REACT_APP_authDomain,
  databaseURL: process.env.REACT_APP_databaseUrl,
  projectId: process.env.REACT_APP_projectId,
  storageBucket: process.env.REACT_APP_storageBucket,
  messagingSenderId: process.env.REACT_APP_messageSenderId,
  appId: process.env.REACT_APP_appId,
  measurementId: process.env.REACT_APP_measurementId
};

// Initialize Firebase
const app = firebase.initializeApp(firebaseConfig);
console.log(app.name);
let possibleSignInError = null;
let playerId;
let playerRef;

const auth = getAuth();

signInAnonymously(auth)
  .then(() => {
    // Signed in..
  })
  .catch((error) => {
    possibleSignInError = error;
  });

function initGame()
{
  const allPlayersRef = firebase.database().ref('players');

  allPlayersRef.on("value", (snapshot) => {
    //Fires whenever a change occurs
  });

  allPlayersRef.on("child_added", (snapshot) => {
    //Fires whenever a new node is added to players
  });

}

onAuthStateChanged(auth, (user) => {
  if (user) {
    playerId = user.uid;
    playerRef = firebase.database().ref(`players/${playerId}`);

    playerRef.set(
      {
        id: playerId,
        name: "test",
        direction: "right",
        x: 3, y: 3, z: 3
      }
    );

    playerRef.onDisconnect().remove();

    initGame();
  } else {
    // User is signed out
    // ...
  }
});
  


const root = ReactDOM.createRoot(document.getElementById('root'));
root.render(
  <React.StrictMode>
    <App error = {possibleSignInError}
        fbapp = {app}
        pid = {playerId}/>
  </React.StrictMode>
);