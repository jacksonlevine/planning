import React from "react";
import { Component } from "react";
export default function MoveArrow(props)
{
  let mainElement = null;
  if(!props.isHidden)
  {
    mainElement =
    <img id="ma" style={
      {
        position:"absolute",
        top: "80%",
        left: "57%",
        pointerEvents: "none"
      }
    } src="/textures/move.png" alt=""/>
  }
  return (
    <React.Fragment>
      {mainElement}
    </React.Fragment>
  );
}