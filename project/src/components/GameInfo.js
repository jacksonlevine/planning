

import React, { useState } from "react";
import { generateUUID } from "three/src/math/MathUtils.js";
import img1 from './../images/underconstruction.png';
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
            <h1 key={titleKey2} className="fadey" style={{
            position:"absolute",
            top:"25%"
        
        }}>MimosDono</h1>

        <h1 key={titleKey} className="secondTitle" style={{
            position:"absolute",
            top:"25%"
        
        }}><span className="red">M</span>
        <span className="red">i</span>
        <span className="red">m</span>
        <span className="red">o</span>
        <span className="red">s</span>Dono</h1>
        <img className="construct" src={img1}alt="under construction"/>
        </div>
        break;
    }
    return (
        <div id="gameInfo">
            {messageElement}
        </div>
    );
}