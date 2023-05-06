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








class App extends Component {
  constructor() {
    super();
    this.user = null;
    this.app = null;
    this.playerId = null;
    this.socket = null;
    this.db = null;
    this.auth = null;
    this.name = "";
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



      backgroundColor: "rgb(20,20,20)",
      color: "white",
      padding: "0px 0px",
      margin: "0% 0%",
      //border: "20px solid rgb(250, 230, 150)",
      fontFamily: "Tahoma",
      overflow: "hidden"
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
    // When the page loads
    const debugRedirectResult = async () => {
      try {
        const result = await getRedirectResult(this.auth)
        if (result) {
          const details = getAdditionalUserInfo(result)
          console.log(details) // details.isNewUser to determine if a new or returning user
        } else {
          // Everything is fine
        }
      } catch (error) {
        console.log(error) // Debug errors from redirect response
      }
    }
    debugRedirectResult();
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
    this.Login().then(
      result => {
        this.playerId = result.playerId;
        this.app = result.app;
        this.socket = result.socket;
        this.user = result.user;
        this.name = result.name;
        this.db = getFirestore(this.app);
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

  signOut = () => {
    this.changeState()("gameButtonVisible")(false);
    this.changeState()("messageToClient")("signin");
    this.socket.close();
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
            <GameInfo message={this.state.messageToClient} />
            <button onClick={
              this.initializeAuth
            }>Sign in with Google</button>
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
              name={this.name}
            />
            <div style={{margin: "0px",display:"flex",flexDirection:"row",justifyContent:"space-between", alignItems:"flex-start"}}>
              <GameInfo message={this.state.messageToClient} /><button onClick = {this.signOut}>Sign Out</button></div>
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
