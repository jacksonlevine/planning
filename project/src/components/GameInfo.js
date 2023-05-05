

import React from "react";
export default function GameInfo(props) {
    let messageElement = null;
    switch(props.message)
    {
        default: case "none":
        break;
        case "loadingworld":
        messageElement = <h4>Loading World...</h4>
        break;
        case "signin":
        messageElement = 
        <div style={{display:"flex",flexDirection:"column",alignItems:"flex-start"}}>
            <h3>Welcome!</h3>
            <h4>You need to sign in.</h4>
        </div>
    }
    return (
        <div id="gameInfo">
            {messageElement}
        </div>
    );
}