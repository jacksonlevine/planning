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
        top: "78%",
        left: "-90px",
        pointerEvents: "none",
        padding: "20px",
        paddingTop: "40px",
        paddingLeft: "100px",
        paddingRight: "50%"
      }
    } src="/textures/upanddown.png" alt=""/>
  }
  return (
    <React.Fragment>
      {mainElement}
    </React.Fragment>
  );
}