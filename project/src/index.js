import React from 'react';
import ReactDOM from 'react-dom/client';
import App from './components/App';

import { initializeApp } from "firebase/app";
import { getAnalytics } from "firebase/analytics";

const firebaseConfig = {
  apiKey: {process.env.REACT_APP_FIREBASE_API_KEY},
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
const analytics = getAnalytics(app);

const root = ReactDOM.createRoot(document.getElementById('root'));
root.render(
  <React.StrictMode>
    <App />
  </React.StrictMode>
);
