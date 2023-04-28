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
    return (
      <div className="App">
        <h1>{props.pid}</h1>
      </div>
    );
  }
}

export default App;
