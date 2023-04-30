import React, { Component } from "react";
import Game from "./Game";
import "firebase/app"; 
import firebase from "firebase/compat/app";
import "firebase/compat/database";
import { getAuth, signInAnonymously, onAuthStateChanged } from "firebase/auth";

let playerId = null; 
let playerRef;
function Login()
{

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
let possibleSignInError = null;



const auth = getAuth();

signInAnonymously(auth)
  .then(() => {
    // Signed in..
  })
  .catch((error) => {
    possibleSignInError = error;
  });


onAuthStateChanged(auth, (user) => {
  if (user) {
    playerId = user.uid;
    playerRef = firebase.database().ref(`players/${playerId}`);

    playerRef.set(
      {
        id: playerId,
        name: "test",
        zrotation: 0,
        x: 3, y: 3, z: 3
      }
    );

    playerRef.onDisconnect().remove();

  } else {
    // User is signed out
    // ...
  }
});
  
}

class App extends Component {
  constructor()
  {
    super();
    Login();
    this.state = {
      pageVisible: "default"
    };
  }

  changeState = () => (property) => (newValue) => {
    this.setState(
      {
        [property]: newValue
      }
    );
  };

  switchPage = (newPage) =>
  {
    this.changeState()("pageVisible")(newPage);
  }

  render() {
    let mainElement = null;
      switch(this.state.pageVisible)
      {
        default:
        case "default":
          mainElement = 

          <React.Fragment>
            <h1>Hello</h1>
          <button onClick={() => {
            this.switchPage("game");

          }}>Go to game</button>
          </React.Fragment>;

          break;
        case "game":
          mainElement = 

          <React.Fragment>
            <Game switcher = {this.switchPage}
                  pid = {playerId}
                  pref = {playerRef}/>
          </React.Fragment>;
          console.log(this.props.pid);
          break;
      }


    return (
      <div className="App">
        {mainElement}
      </div>
    );

  }
}

export default App;
