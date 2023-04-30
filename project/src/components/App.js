import React, { Component } from "react";
import Game from "./Game";

class App extends Component {
  constructor()
  {
    super();
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
    if(this.props.isError) {
    if(false)
    {
     
    } } else {
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
            <Game switcher = {this.switchPage}/>
          </React.Fragment>;
          break;
      }
    }

    return (
      <div className="App">
        {mainElement}
      </div>
    );
  }
}

export default App;
