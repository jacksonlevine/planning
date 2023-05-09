import React from "react";
import { Component } from "react";
export default function UpAndDownArrow(props)
{
  let mainElement = null;
  if(!props.isHidden)
  {
    mainElement =
    <img id="uad" style={
      {
        position:"absolute",
        top: "85%",
        left: "0%",
        pointerEvents: "none"
      }
    } src="/textures/upanddown.png" alt=""/>
  }
  return (
    <React.Fragment>
      {mainElement}
    </React.Fragment>
  );
}