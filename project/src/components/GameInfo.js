

import React, { useState } from "react";
import { generateUUID } from "three/src/math/MathUtils.js";
export default function GameInfo(props) {
    const [titleKey, setTitleKey] = useState();
    const [titleKey2, setTitleKey2] = useState();

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
        <div style={{display:"flex",flexDirection:"column",alignItems:"center"}}>
            <h1 key={titleKey2} className="fadey" style={{fontSize:"72px",
            position:"absolute",
            top:"25%"
        
        }}>MimosDono</h1>

        <h1 key={titleKey} className="secondTitle" style={{fontSize:"72px",
            position:"absolute",
            top:"25%"
        
        }}><span className="blue">M</span>
        <span className="red">i</span>
        <span className="green">m</span>
        <span className="blue">o</span>
        <span className="purple">s</span>Dono</h1>
        </div>
        break;
    }
    return (
        <div id="gameInfo">
            {messageElement}
        </div>
    );
}