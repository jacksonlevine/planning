import React, { Component } from "react";
import Game from "./Game.js";
import "firebase/app";
import firebase from "firebase/compat/app";
import "firebase/compat/database";
import { getAuth, getRedirectResult, onAuthStateChanged, GoogleAuthProvider, signInWithRedirect, getAdditionalUserInfo, signInWithPopup } from "firebase/auth";
import GameInfo from "./GameInfo.js";
import { getFirestore } from "firebase/firestore";
import io from 'socket.io-client';
import PayPalDonate from "./PayPalDonate.js";
import { generateUUID } from "three/src/math/MathUtils.js";

class App extends Component {
  constructor() {
    super();
    this.user = null;
    this.app = null;
    this.playerId = null;
    this.socket = null;
    this.db = null;
    this.auth = null;
    this.isSinglePlayer = false;
    this.name = "";
    this.canvRef = React.createRef();
    this.state = {
      gameButtonVisible: false,
      pageVisible: "default",
      messageToClient: "signin",
      chat: [],
      width: 0,
      height: 0,
      isSignedOut: true,
    };
    this.styles = {
      color: "white",
      padding: "0px 0px",
      margin: "0% 0%",
      //border: "20px solid rgb(250, 230, 150)",
      overflow: "hidden",
      display: "flex",
      flexDirection: "column",
      alignItems: "center"
    };
  }
  Login = async () => {
    return new Promise(async (resolve, reject) => {
      const provider = new GoogleAuthProvider();
      const userCred = await signInWithPopup(this.auth, provider);
      onAuthStateChanged(this.auth, (user) => {
        if (user) {
          const playerId = user.uid;
          const socket = io("67.58.229.227:3001");
          const details = getAdditionalUserInfo(userCred);
          //playerRef.onDisconnect().remove();
          resolve({ user, app:this.app, name: details.profile.name, playerId, auth: this.auth, socket});
        } else {
          reject("You need to sign in.");
        }
      });
    });
  };

  changeState = () => (property) => (newValue) => {
    this.setState({
      [property]: newValue,
    });
  };

  componentDidMount()
  {


    this.setState(
      {
        width: document.getElementById("App").offsetWidth*.80,
        height: window.innerHeight/1.2
      }
    )
    window.addEventListener( 'resize', this.callForResize, false );
  }


  callForResize = () => {
    this.changeState()("width")(document.getElementById("App").offsetWidth*.80)
    this.changeState()("height")((window.innerHeight/1.2))
  }

  initializeAuth = () => {
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
    this.app = firebase.initializeApp(firebaseConfig);
    this.auth = getAuth();
    this.Login().then(
      result => {
        this.playerId = result.playerId;
        this.app = result.app;
        this.socket = result.socket;
        this.user = result.user;
        this.name = result.name;
        this.db = getFirestore(this.app);
        this.isSinglePlayer = false;
        this.changeState()("gameButtonVisible")(true);
        this.changeState()("messageToClient")("none");
        this.switchPage("game");
      }
    ).catch(
      error=> {
        this.changeState()("messageToClient")("signin");
      }
    );
  }
  
  initializeSinglePlayer = () => {
    this.isSinglePlayer = true;
    this.changeState()("gameButtonVisible")(true);
    this.changeState()("isSignedOut")(false);
        this.changeState()("messageToClient")("none");
        this.switchPage("game");

  }

  signOut = () => {
    
    this.changeState()("isSignedOut")(true);
    this.changeState()("gameButtonVisible")(false);
    this.changeState()("messageToClient")("signin");
    this.switchPage("default")

    if(this.socket)
    {
      this.socket.off('chat');
    
      this.socket.off('brightness');
      
      this.socket.off('playerUpdate');
      
      this.socket.off('playerDisconnect');
    this.socket.emit("playerDisconnect", this.playerId);
      this.socket.close();
    }
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
            
          </React.Fragment>
          
        );}
        else {
          mainElement = 
          <React.Fragment>
            <h3>MimosDono 2 Coming soon</h3>
            <GameInfo message={this.state.messageToClient} />
            <div className="mainButtons">
              <button className="btn btn-outline-info slide1" onClick={
                this.initializeAuth
              }>Sign in with Google</button>
              <button className="btn btn-outline-warning slide2" onClick={
                this.initializeSinglePlayer
              }>Singleplayer mode</button>
            </div>
          </React.Fragment>;
        }

        break;
      case "game":
        document.body.style.backgroundImage = 'url("/textures/backgroundgame.png")'
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
              name={this.name}
              isSinglePlayer={this.isSinglePlayer}
              isSignedOut={this.state.isSignedOut}
            />
            <div style={{margin: "0px",display:"flex",flexDirection:"row",justifyContent:"space-between", alignItems:"flex-start"}}>
              <GameInfo message={this.state.messageToClient} /><button className="btn btn-outline-danger" onClick = {this.signOut}>Sign Out</button></div>
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
