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

    const provider = new firebase.auth.GoogleAuthProvider();
    provider.setCustomParameters({ prompt: 'select_account' });
    auth.signInWithPopup(provider);
    // signInAnonymously(auth)
    //   .then(() => {
    //     // Signed in..
    //   })
    //   .catch((error) => {
    //     console.log(error);
    //   });
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
    this.canvRef = React.createRef();
    this.state = {
      gameButtonVisible: false,
      pageVisible: "default",
      messageToClient: "signin",
      chat: [],
      width: 0,
      height: 0,
    };

    this.styles = {



      backgroundColor: "rgb(190, 170, 80)",

      padding: "0px 0px",
      margin: "0% 0%",
      border: "20px solid rgb(250, 230, 150)",
      fontFamily: "Tahoma",
      overflow: "hidden"
    };
  }

  changeState = () => (property) => (newValue) => {
    this.setState({
      [property]: newValue,
    });
  };

  componentDidMount()
  {
    this.setState(
      {
        width: document.getElementById("App").offsetWidth*.70,
        height: window.innerHeight/1.5
      }
    )
    window.addEventListener( 'resize', this.callForResize, false );
  }


  callForResize = () => {
    this.changeState()("width")(document.getElementById("App").offsetWidth*.70)
    this.changeState()("height")((window.innerHeight/1.5))
  }

  

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
              width={this.state.width}
              height={this.state.height}
              resize={this.callForResize}
            />
            <GameInfo message={this.state.messageToClient} />
          </React.Fragment>
        );

        //console.log(this.props.pid);
        break;
    }

    return (
      <div ref={this.canvRef} style={this.styles} id="App">
        {mainElement}
      </div>
    );
  }
}

export default App;
