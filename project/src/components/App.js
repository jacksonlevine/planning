import React, { Component } from "react";
import Game from "./Game.js";
import "firebase/app";
import firebase from "firebase/compat/app";
import "firebase/compat/database";
import { getAuth, signInAnonymously, onAuthStateChanged } from "firebase/auth";
import GameInfo from "./GameInfo.js";
import { getFirestore } from "firebase/firestore";
import io from 'socket.io-client';

const Login = () => {
  return new Promise((resolve, reject) => {
    const firebaseConfig = {
      apiKey:
        process.env.NODE_ENV === "development"
          ? process.env.REACT_APP_apiKeyDev
          : process.env.REACT_APP_apiKeyProd,
      authDomain: process.env.REACT_APP_authDomain,
      databaseURL: process.env.REACT_APP_databaseUrl,
      projectId: process.env.REACT_APP_projectId,
      storageBucket: process.env.REACT_APP_storageBucket,
      messagingSenderId: process.env.REACT_APP_messageSenderId,
      appId: process.env.REACT_APP_appId,
      measurementId: process.env.REACT_APP_measurementId,
    };
    // Initialize Firebase
    const app = firebase.initializeApp(firebaseConfig);
    const auth = getAuth();
    signInAnonymously(auth)
      .then(() => {
        // Signed in..
      })
      .catch((error) => {
        console.log(error);
      });
    onAuthStateChanged(auth, (user) => {
      if (user) {
        const playerId = user.uid;
        let playerRef;
        const socket = io("67.58.229.227:3001");
        
        //playerRef.onDisconnect().remove();
        resolve({ app, playerRef, playerId, auth, socket});
      } else {
        reject("You need to sign in.");
      }
    });
  });
};



class App extends Component {
  constructor() {
    super();
    this.app = null;
    this.playerId = null;
    this.playerRef = null;
    this.socket = null;
    this.db = null;
    this.state = {
      gameButtonVisible: false,
      pageVisible: "default",
      messageToClient: "signin",
      chat: [],
    };

    this.styles = {
      display: "flex",
      flexDirection: "column",
      justifyContent: "center",
      backgroundColor: "rgb(80, 170, 80)",
      alignItems: "center",
      padding: "40px 0px",
      margin: "0% 20%",
      borderRadius: "50px",
      border: "40px solid lightgreen",
      fontFamily: "Tahoma",
    };
  }

  changeState = () => (property) => (newValue) => {
    this.setState({
      [property]: newValue,
    });
  };

  initializeAuth = () => {
    Login().then(
      result => {
        this.playerId = result.playerId;
        this.playerRef = result.playerRef;
        this.app = result.app;
        this.socket = result.socket;
        this.db = getFirestore(this.app);
        this.changeState()("gameButtonVisible")(true);
        this.changeState()("messageToClient")("none");
        this.switchPage("default");
      }
    ).catch(
      error=> {
        this.changeState()("messageToClient")("signin");
      }
    );
  }

  signOut = () => {
    this.playerRef.remove();
    this.changeState()("gameButtonVisible")(false);
    this.changeState()("messageToClient")("signin");
  }

  switchPage = (newPage) => {
    this.changeState()("pageVisible")(newPage);
  };

  render() {
    let mainElement = null;
    switch (this.state.pageVisible) {
      default:
      case "default":
        if(this.state.gameButtonVisible === true) {
        mainElement = (
          <React.Fragment>
            <h1>Hello</h1>
            <button
              onClick={() => {
                this.switchPage("game");
              }}
            >
              Go to game
            </button>
            <button onClick = {
              this.signOut
            }>Sign Out</button>
          </React.Fragment>
          
        );}
        else {
          mainElement = 
          <React.Fragment>
            <GameInfo message={this.state.messageToClient} />
            <button onClick={
              this.initializeAuth
            }>Sign in</button>
          </React.Fragment>;
        }

        break;
      case "game":

        mainElement = (
          <React.Fragment>
            <Game
              switcher={this.switchPage}
              pid={this.playerId}
              pref={this.playerRef}
              handle={this.changeState}
              db={this.db}
              socket={this.socket}
            />
            <GameInfo message={this.state.messageToClient} />
          </React.Fragment>
        );

        //console.log(this.props.pid);
        break;
    }

    return (
      <div style={this.styles} className="App">
        {mainElement}
      </div>
    );
  }
}

export default App;
