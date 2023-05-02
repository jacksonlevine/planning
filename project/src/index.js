import React from 'react';
import ReactDOM from 'react-dom/client';
import App from './components/App.js';
import Header from './components/Header.js';
import "./index.css"


const root = ReactDOM.createRoot(document.getElementById('root'));
root.render(
  //<React.StrictMode>
  <React.Fragment>
    <Header />
    <App />
  </React.Fragment>
  //</React.StrictMode>
);
