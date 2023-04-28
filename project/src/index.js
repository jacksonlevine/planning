import React from 'react';
import ReactDOM from 'react-dom/client';
import App from './components/App';
require('dotenv').config()
import { initializeApp } from "firebase/app";
import { getAuth, signInAnonymously } from "firebase/auth";

const firebaseConfig = {
  apiKey: (process.env.NODE_ENV === 'development' ?
  process.env.REACT_APP_apiKeyDev : process.env.REACT_APP_apiKeyProd),
  authDomain: process.env.authDomain,
  databaseURL: process.env.databaseUrl,
  projectId: process.env.projectId,
  storageBucket: process.env.storageBucket,
  messagingSenderId: process.env.messageSenderId,
  appId: process.env.appId,
  measurementId: process.env.measurementId
};

// Initialize Firebase
const app = initializeApp(firebaseConfig);

let possibleError;
let playerId;
let playerRef;

const auth = getAuth();
signInAnonymously(auth)
  .then(() => {
    // Signed in..
  })
  .catch((error) => {
    possibleError = error;
  });

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
    } else {
      // User is signed out
      // ...
    }
  });
  


const root = ReactDOM.createRoot(document.getElementById('root'));
root.render(
  <React.StrictMode>
    <App error = {possibleError}
        fbapp = {app}
        auth = {auth}/>
  </React.StrictMode>
);
