import React from 'react';
import ReactDOM from 'react-dom/client';
import App from './components/App';
require('dotenv').config()
import { initializeApp } from "firebase/app";
import { getAuth, signInAnonymously } from "firebase/auth";

const firebaseConfig = {
  apiKey: process.env.NODE_ENV === 'development' ?
  process.env.REACT_APP_FIREBASE_API_KEY_DEVELOPMENT : process.env.REACT_APP_FIREBASE_API_KEY_PRODUCTION,
  authDomain: "jacksongame-eb775.firebaseapp.com",
  databaseURL: "https://jacksongame-eb775-default-rtdb.firebaseio.com",
  projectId: "jacksongame-eb775",
  storageBucket: "jacksongame-eb775.appspot.com",
  messagingSenderId: "852977180992",
  appId: "1:852977180992:web:ac8e4f04a47d35e96ddaa5",
  measurementId: "G-5Y06CGYN2F"
};

// Initialize Firebase
const app = initializeApp(firebaseConfig);

const auth = getAuth();
signInAnonymously(auth)
  .then(() => {
    // Signed in..
  })
  .catch((error) => {
    const errorCode = error.code;
    const errorMessage = error.message;
    // ...
  });
const root = ReactDOM.createRoot(document.getElementById('root'));
root.render(
  <React.StrictMode>
    <App />
  </React.StrictMode>
);
