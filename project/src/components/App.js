import { Component } from "react";

class App extends Component {
  constructor()
  {
    super();
    this.state = {
      pageVisible: "default"
    };
  }
  render() {
    let mainElement = null;

    if(this.props.error != null)
    {
      mainElement = <h1>There was an error signing in. {error.message}</h1>;
    } else {
      switch(this.state.pageVisible)
      {
        default:
        case "default":
          mainElement = <h1>Hello</h1>;
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
