import React, { Component } from "react";
import "firebase/app";
import firebase from "firebase/compat/app";
import "firebase/compat/database";
import * as THREE from "three";
import { GLTFLoader } from "three/examples/jsm/loaders/GLTFLoader.js";
import ImprovedNoise from "./../perlin.js";
//import LZString from "lz-string/libs/lz-string.js";
import { generateUUID } from "three/src/math/MathUtils.js";
//import { collection, addDoc, getDoc, doc, setDoc } from "firebase/firestore";
import LinkedHashMap from "../linkHashMap.js";
import ChatView from "./ChatVIew.js";
import UpAndDownArrow from "./UpAndDownArrow.js";
import MoveArrow from "./MoveArrow.js";
import Inventory from "./Inventory.js";
import { PointerLockControls2 } from "../customPointerLockControls.js";

let saturn;

let minBrightness = 0.1;
const maxBrightness = 1;

let backFogColor = new THREE.Color((134.0/255.0)*minBrightness, (196.0/255.0)*minBrightness, (194.0/255.0)*minBrightness);


class InputState {
  constructor() {
    this.left = false;
    this.right = false;
    this.back = false;
    this.forward = false;
    this.jump = false;
    this.crouch = false;
    this.jumpSpeed = 2;
    this.jumpTimer = 0;
    this.isGrounded = true;
  }
}

const b = [1, .8, .6, .4, .2] //torch brightnesses

class InputHandler {
  constructor() {
    this.ActiveState = new InputState();
  }
}
const texture = new THREE.TextureLoader().load("textures/textures.png");
texture.magFilter = THREE.NearestFilter;
texture.minFilter = THREE.LinearFilter;
texture.generateMipmaps = false;
texture.antialias = false;

const meshMaterial = new THREE.MeshBasicMaterial({
  color: 0xffffff,
});

meshMaterial.map = texture;
meshMaterial.vertexColors = true;
meshMaterial.emissiveIntensity = 20;

class PrevAndNewPosition {
  constructor(vector1, vector2) {
    this.prevPosition = vector1;
    this.newPosition = vector2; 
  }
}
const texPad = 0.0015;

const blockTypes = {
  count: 5,
  /*stone*/1: {
    texture: {
      uniform: true,
      all: [
        0.0 + 0.0625 - texPad,
        1.0 - 0.0625 + texPad,
        0.0 + texPad,
        1.0 - 0.0625 + texPad,
        0.0 + texPad,
        1.0 - texPad,
        0.0 + 0.0625 - texPad,
        1.0 - texPad,
      ],
    },
  },
  /*brick*/2: {
    texture: {
      uniform: true,
      all: [
        0.0625 + 0.0 + 0.0625 - texPad,
        1.0 - 0.0625 + texPad,
        0.0625 + 0.0 + texPad,
        1.0 - 0.0625 + texPad,
        0.0625 + 0.0 + texPad,
        1.0 - texPad,
        0.0625 + 0.0 + 0.0625 - texPad,
        1.0 - texPad,
      ],
    },
  },
  /*sand*/3: {
    texture: {
      uniform: true,
      all: [
        0.0625 * 2 + 0.0 + 0.0625 - texPad,
        1.0 - 0.0625 + texPad,
        0.0625 * 2 + 0.0 + texPad,
        1.0 - 0.0625 + texPad,
        0.0625 * 2 + 0.0 + texPad,
        1.0 - texPad,
        0.0625 * 2 + 0.0 + 0.0625 - texPad,
        1.0 - texPad,
      ],
    },
  },
  /*grass*/4: {
    texture: {
      uniform: false,
      top: [
        0.0625 * 3 + 0.0 + 0.0625 - texPad,
        1.0 - 0.0625 + texPad,
        0.0625 * 3 + 0.0 + texPad,
        1.0 - 0.0625 + texPad,
        0.0625 * 3 + 0.0 + texPad,
        1.0 - texPad,
        0.0625 * 3 + 0.0 + 0.0625 - texPad,
        1.0 - texPad,
      ],
      sides: [
        0.0625 * 4 + 0.0 + 0.0625 - texPad,
        1.0 - 0.0625 + texPad,
        0.0625 * 4 + 0.0 + texPad,
        1.0 - 0.0625 + texPad,
        0.0625 * 4 + 0.0 + texPad,
        1.0 - texPad,
        0.0625 * 4 + 0.0 + 0.0625 - texPad,
        1.0 - texPad,
      ],
      bottom: [
        0.0625 * 5 + 0.0 + 0.0625 - texPad,
        1.0 - 0.0625 + texPad,
        0.0625 * 5 + 0.0 + texPad,
        1.0 - 0.0625 + texPad,
        0.0625 * 5 + 0.0 + texPad,
        1.0 - texPad,
        0.0625 * 5 + 0.0 + 0.0625 - texPad,
        1.0 - texPad,
      ],
    },
  },
  /*light*/5: {
    texture: {
      uniform: true,
      all: [
        0.0625 * 6 + 0.0 + 0.0625 - texPad,
        1.0 - 0.0625 + texPad,
        0.0625 * 6 + 0.0 + texPad,
        1.0 - 0.0625 + texPad,
        0.0625 * 6 + 0.0 + texPad,
        1.0 - texPad,
        0.0625 * 6 + 0.0 + 0.0625 - texPad,
        1.0 - texPad,
      ],
    },
    isLight: true
  },
};

export default class Game extends Component {
  constructor(props) {
    super(props);
    this.allPlayerModels = new Map();
    this.allPlayersRef = (props.isSinglePlayer ? null : firebase.database().ref("players"));
    this.allBlockActionsRef = (props.isSinglePlayer ? null : firebase.database().ref("blockActions"));


    this.width = 1024;
    this.height = 640;
    this.fov = 100;

    this.camera = new THREE.PerspectiveCamera(
      this.fov,
      this.width / this.height,
      0.001,
      1000
    );
    this.players = {};
    this.isOpen = true;
    this.styles = {
      display: "flex",
      flexDirection: "column",
      alignItems: "center",
    };
    this.canvas = null;
    this.controls = null;
    this.chunkWidth = 11;
    this.scene = new THREE.Scene();
    this.world = null;
    this.surveyNeededChunksTimer = 0;
    this.surveyNeededChunksInterval = 5;
    this.chunkQueueTimer = 0;
    this.chunkQueueInterval = 0;
    this.mappedChunks = new Map();
    this.neededChunks = new Map();
    this.lightedChunks = new Map();
    this.chunkpool = [];
    this.input = new InputHandler();
    this.playerOldAndNewPositions = new Map(); // uid , PrevAndNewPosiotn
    this.updatePlayersTimer = parseFloat("0.0");
    this.updatePlayersInterval = 0.15;
    this.clock = new THREE.Clock();
    this.delt = 0;
    this.isReady = false;
    this.currentTouchX = {};
    this.currentTouchY = {};
    this.touchIndex = 0;
    this.state = {
      chat: "",
      chats: [],
      maxChatLines: 12,
      mobileMode: false,
      currentlyPlacingId: 2,
    };
    this.chatBoxRef = React.createRef();
    this.canvasRef = React.createRef();
    this.mobileVerticalOrientatorDown = false;
    this.mobileMoverDown = false;
    this.blockChangeRequested = false;
  }

  isCameraFacingThis = (direction, x, y, z) => {
    const spot = new THREE.Vector3(x, y, z);
    return (
      spot.distanceTo(this.camera.position) <
      spot.distanceTo(this.camera.position.add(direction.addScalar(2.0)))
    );
  };

  render() {
    return (
      <div style={this.styles}>
        <div
          style={{
            textAlign: "center",
          }}
        >
          <div
            style={{
              display: "flex",
              flexDirection: "column",
              alignItems: "center",
            }}
          >
            <div
              style={{
                position: "relative",
              }}
            >
              <img
                style={{
                  position: "absolute",
                  zIndex: "10",
                  margin: "0px",
                  top: "50%",
                  left: "50%",
                  transform: "translate(-50%, -50%)",
                }}
                src="/textures/hairsmall.png"
                alt=""
              />
              <UpAndDownArrow isHidden={!this.state.mobileMode} />
              <MoveArrow isHidden={!this.state.mobileMode} />
              <p>
                <strong>WARNING:</strong> This is a development server, the game world will likely be reset.
              </p>
              <canvas
                ref={this.canvasRef}
                style={{
                  zIndex: "-1",
                }}
                id="canvas"
              ></canvas>
            </div>
            <p>Chat: Press 't' to type and 'Enter' to send.</p>
            <ChatView chats={this.state.chats} />
            <Inventory id={this.state.currentlyPlacingId} />
          </div>
        </div>
        <form onSubmit={this.sendChat}>
          <input
            ref={this.chatBoxRef}
            value={this.state.chat}
            tabIndex={1}
            type="text"
            id="inp"
            name="inp"
            onChange={(e) => {
              this.setState({
                chat: e.target.value,
              });
            }}
          ></input>
          <button type="submit">Send chat</button>
        </form>
      </div>
    );
  }

  castRay = (x, y, z, dx, dy, dz, maxDistance, stepback) => {
    return new Promise((resolve, reject) => {
      let distanceTraveled = 0;
      let currentX = x;
      let currentY = y;
      let currentZ = z;

      while (distanceTraveled < maxDistance) {
        currentX += dx * 0.05;
        currentY += dy * 0.05;
        currentZ += dz * 0.05;
        distanceTraveled += 0.05;

        const key = `${Math.floor(currentX)},${Math.floor(
          currentY
        )},${Math.floor(currentZ)}`;
        if (this.world.data.has(key)) {
          resolve({
            x: Math.floor(currentX - dx * stepback),
            y: Math.floor(currentY - dy * stepback),
            z: Math.floor(currentZ - dz * stepback),
          });
          return;
        }
      }

      reject("Raycast did not hit anything");
    });
  };

  getCameraDirection = () => {
    var vector = new THREE.Vector3(0, 0, -1);
    vector.applyQuaternion(this.camera.quaternion);
    return vector;
  };

  getCameraDirectionRight = () => {
    const cameraDirection = this.getCameraDirection();
    cameraDirection.y = 0;
    const upVector = new THREE.Vector3(0, 1, 0);
    const rightVector = cameraDirection.clone().cross(upVector).normalize();
    return rightVector;
  };

  onClick = (event) => {
    switch (event.which) {
      case 1: //Left click
        if (!this.controls.isLocked) {
          this.controls.lock();
        } else {
          let vector = this.getCameraDirection();

          this.castRay(
            this.camera.position.x,
            this.camera.position.y,
            this.camera.position.z,
            vector.x,
            vector.y,
            vector.z,
            5,
            0
          )
            .then((pos) => {
              this.breakBlock(pos);
              if(!this.props.isSinglePlayer){let actionRef = firebase
                .database()
                .ref(`blockActions/${generateUUID()}`);
              actionRef.set({
                action: "break",
                time: new Date().getTime(),
                ...pos,
              });}
            })
            .catch((error) => {
              console.log(error);
            });
        }
        break;
      case 3: //Right click
        event.preventDefault();
        let vector = this.getCameraDirection();

        this.castRay(
          this.camera.position.x,
          this.camera.position.y,
          this.camera.position.z,
          vector.x,
          vector.y,
          vector.z,
          5,
          0.3
        )
          .then((pos) => {
            let placed = `${this.state.currentlyPlacingId}`;
            this.placeBlock(pos, placed);
            if(!this.props.isSinglePlayer){let actionRef = firebase
              .database()
              .ref(`blockActions/${generateUUID()}`);
            actionRef.set({
              action: "place",
              type: placed,
              time: new Date().getTime(),
              ...pos,
            });}
          })
          .catch((error) => {});
        break;
      default:
        break;
    }
  };

  breakBlock(pos) {
    const blockKey = `${pos.x},${pos.y},${pos.z}`;
    if(this.world.data.has(blockKey)) {
      if(blockTypes[this.world.data.get(blockKey)].isLight)
      {
        this.removePointLight(pos.x, pos.y, pos.z);

      }
    }
    this.world.data.delete(blockKey);

    const chunkX = Math.floor(pos.x / this.chunkWidth);
    const chunkY = Math.floor(pos.y / this.chunkWidth);
    const chunkZ = Math.floor(pos.z / this.chunkWidth);
    if (this.world.fullBlockMarks.has(`${chunkX},${chunkY},${chunkZ}`)) {
      this.world.fullBlockMarks.delete(`${chunkX},${chunkY},${chunkZ}`);
    }
    if (this.mappedChunks.has(`${chunkX},${chunkY},${chunkZ}`)) {
      const chunk = this.mappedChunks.get(`${chunkX},${chunkY},${chunkZ}`);
      chunk.buildMeshInPlace();
    } else {
      this.neededChunks.set(`${chunkX},${chunkY},${chunkZ}`, {
        x: chunkX,
        y: chunkY,
        z: chunkZ,
      });
    }
    //6 surrounders
    if (this.mappedChunks.has(`${chunkX - 1},${chunkY},${chunkZ}`)) {
      this.mappedChunks
        .get(`${chunkX - 1},${chunkY},${chunkZ}`)
        .buildMeshInPlace();
    }
    if (this.mappedChunks.has(`${chunkX + 1},${chunkY},${chunkZ}`)) {
      this.mappedChunks
        .get(`${chunkX + 1},${chunkY},${chunkZ}`)
        .buildMeshInPlace();
    }
    if (this.mappedChunks.has(`${chunkX},${chunkY - 1},${chunkZ}`)) {
      this.mappedChunks
        .get(`${chunkX},${chunkY - 1},${chunkZ}`)
        .buildMeshInPlace();
    }
    if (this.mappedChunks.has(`${chunkX},${chunkY + 1},${chunkZ}`)) {
      this.mappedChunks
        .get(`${chunkX},${chunkY + 1},${chunkZ}`)
        .buildMeshInPlace();
    }
    if (this.mappedChunks.has(`${chunkX},${chunkY},${chunkZ - 1}`)) {
      this.mappedChunks
        .get(`${chunkX},${chunkY},${chunkZ - 1}`)
        .buildMeshInPlace();
    }
    if (this.mappedChunks.has(`${chunkX},${chunkY},${chunkZ + 1}`)) {
      this.mappedChunks
        .get(`${chunkX},${chunkY},${chunkZ + 1}`)
        .buildMeshInPlace();
    }
  }

  placeBlock(pos, id) {
    if(blockTypes[id]) {
      if (blockTypes[id].isLight) {
        this.addPointLight(pos.x, pos.y, pos.z);
      } 
        let [chunkx, chunky, chunkz] = [
          Math.floor(pos.x / this.chunkWidth),
          Math.floor(pos.y / this.chunkWidth),
          Math.floor(pos.z / this.chunkWidth),
        ];

        if (!this.world.ishandledmarks.has(`${chunkx},${chunky},${chunkz}`)) {
          this.world.generateOneChunk(chunkx, chunky, chunkz);
        }
        const blockKey = `${pos.x},${pos.y},${pos.z}`;
        this.world.data.set(blockKey, id);
        const chunkX = Math.floor(pos.x / this.chunkWidth);
        const chunkY = Math.floor(pos.y / this.chunkWidth);
        const chunkZ = Math.floor(pos.z / this.chunkWidth);
        if (!this.world.hasBlocksMarks.has(`${chunkX},${chunkY},${chunkZ}`)) {
          this.world.hasBlocksMarks.set(`${chunkX},${chunkY},${chunkZ}`, "1");
        }
        if (this.mappedChunks.has(`${chunkX},${chunkY},${chunkZ}`)) {
          this.mappedChunks
            .get(`${chunkX},${chunkY},${chunkZ}`)
            .buildMeshInPlace();
        } else {
          this.neededChunks.set(`${chunkX},${chunkY},${chunkZ}`, {
            x: chunkX,
            y: chunkY,
            z: chunkZ,
          });
        }
      
    }
  }

  componentWillUnmount() {
    window.removeEventListener("keydown", this.onKeyDown, false);

    window.removeEventListener("keyup", this.onKeyUp, false);

    window.removeEventListener("touchstart", this.onTouchStart, false);
    window.removeEventListener("touchmove", this.onTouchMove, false);
    window.removeEventListener("touchend", this.onTouchEnd, false);
    window.removeEventListener("mousedown", this.onClick);
  }

  mountListeners = () => {
    window.addEventListener("wheel", (event) => {
      this.setState({
        currentlyPlacingId: Math.max(
          Math.min(this.state.currentlyPlacingId + ((event.deltaY > 0) ? 1 : -1), blockTypes.count),
          1
        ),
      });
    });
    window.setInterval(() => {
      this.surveyNeededChunks();
    }, 2000);

    window.addEventListener("resize", this.onWindowResize, false);

    window.addEventListener("keydown", this.onKeyDown, false);
    window.addEventListener("mousedown", this.onClick, false);
    window.addEventListener("keyup", this.onKeyUp, false);
    window.addEventListener("touchstart", this.onTouchStart, false);
    window.addEventListener("touchmove", this.onTouchMove, false);
    window.addEventListener("touchend", this.onTouchEnd, false);
  };
  onWindowResize = () => {
    this.camera.aspect = this.props.width / this.props.height;
    this.camera.updateProjectionMatrix();
    this.width = this.props.width;
    this.height = this.props.height;
    this.renderer.setSize(this.props.width, this.props.height);

    this.setSmallMode();
  };

  setSmallMode = () => {
    if (window.innerWidth < 1200) {
      this.setState({
        mobileMode: true,
      });
    } else {
      this.setState({
        mobileMode: false,
      });
    }
  };

  onTouchStart = (event) => {
    this.touchIndex = 0;
    for (let i = 0; i < Array.from(event.touches).length; i++) {
      this.currentTouchX[i] = event.touches[i].clientX;
      this.currentTouchY[i] = event.touches[i].clientY;
      if (this.state.mobileMode) {
        const element = document.getElementById("uad");
        const element2 = document.getElementById("ma");
        const clientRect = element.getBoundingClientRect();
        const clientRectMove = element2.getBoundingClientRect();
        const mobileVerticalOrientatorX = clientRect.left;
        const mobileVerticalOrientatorY = clientRect.top;
        const mobileVerticalOrientatorWidth = clientRect.width;
        const mobileVerticalOrientatorHeight = clientRect.height;

        const mobileVerticalOrientatorX2 = clientRectMove.left;
        const mobileVerticalOrientatorY2 = clientRectMove.top;
        const mobileVerticalOrientatorWidth2 = clientRectMove.width;
        const mobileVerticalOrientatorHeight2 = clientRectMove.height;
        if (
          this.currentTouchX[i] >= mobileVerticalOrientatorX &&
          this.currentTouchX[i] <=
            mobileVerticalOrientatorX + mobileVerticalOrientatorWidth &&
          this.currentTouchY[i] >= mobileVerticalOrientatorY &&
          this.currentTouchY[i] <=
            mobileVerticalOrientatorY + mobileVerticalOrientatorHeight
        ) {
          this.mobileVerticalOrientatorDown = true;
          this.input.ActiveState.jump = true;
          this.input.ActiveState.isGrounded = false;
        }
        if (
          this.currentTouchX[i] >= mobileVerticalOrientatorX2 &&
          this.currentTouchX[i] <=
            mobileVerticalOrientatorX2 + mobileVerticalOrientatorWidth2 &&
          this.currentTouchY[i] >= mobileVerticalOrientatorY2 &&
          this.currentTouchY[i] <=
            mobileVerticalOrientatorY2 + mobileVerticalOrientatorHeight2
        ) {
          this.touchIndex += 1;
          this.mobileMoverDown = true;
        }
      }
    }
  };

  onTouchMove = (event) => {
    //event.preventDefault();

    // if(event.touches[0].clientX !== this.currentTouchX)
    // {
    //   let differenceX = (event.touches[0].clientX - this.currentTouchX);
    //   //this.neck.rotation.y -= differenceX/20;

    //   this.currentTouchX = event.touches[0].clientX;
    // }

    if (this.mobileMoverDown) {
      event.preventDefault();
      this.controls.mobileMoverDown = true;
      this.input.jump = true;
      this.input.isGrounded = false;
      if (
        event.touches[0].clientY !== this.currentTouchY[0] ||
        event.touches[0].clientX !== this.currentTouchX[0]
      ) {
        let differenceX = event.touches[0].clientX - this.currentTouchX[0];
        let differenceY = -(event.touches[0].clientY - this.currentTouchY[0]);
        if (differenceX > 0) {
          this.input.ActiveState.right = true;
        }
        if (differenceX < 0) {
          this.input.ActiveState.left = true;
        }
        if (differenceY > 0) {
          this.input.ActiveState.forward = true;
        }
        if (differenceY < 0) {
          this.input.ActiveState.back = true;
        }
      }
    }
  };

  onTouchEnd = (event) => {
    this.controls.mobileMoverDown = false;
    this.input.ActiveState.right = false;

    this.input.ActiveState.left = false;

    this.input.ActiveState.forward = false;

    this.input.ActiveState.back = false;
    const element = document.getElementById("uad");
    const element2 = document.getElementById("ma");
    if (element !== null && element2 !== null) {
      const clientRect = element.getBoundingClientRect();
      const clientRectMove = element2.getBoundingClientRect();
      const mobileVerticalOrientatorX = clientRect.left;
      const mobileVerticalOrientatorY = clientRect.top;
      const mobileVerticalOrientatorWidth = clientRect.width;
      const mobileVerticalOrientatorHeight = clientRect.height;

      const mobileVerticalOrientatorX2 = clientRectMove.left;
      const mobileVerticalOrientatorY2 = clientRectMove.top;
      const mobileVerticalOrientatorWidth2 = clientRectMove.width;
      const mobileVerticalOrientatorHeight2 = clientRectMove.height;

      delete this.currentTouchX[this.touchIndex];
      delete this.currentTouchY[this.touchIndex];
      if (
        this.currentTouchX[this.touchIndex] >= mobileVerticalOrientatorX &&
        this.currentTouchX[this.touchIndex] <=
          mobileVerticalOrientatorX + mobileVerticalOrientatorWidth &&
        this.currentTouchY[this.touchIndex] >= mobileVerticalOrientatorY &&
        this.currentTouchY[this.touchIndex] <=
          mobileVerticalOrientatorY + mobileVerticalOrientatorHeight
      ) {
        this.mobileVerticalOrientatorDown = false;
      }

        this.mobileMoverDown = false;

    }
  };
  changeBrightness = (newMinBrightness) => {
    minBrightness = newMinBrightness;
    backFogColor = new THREE.Color((134.0/255.0)*minBrightness, (196.0/255.0)*minBrightness, (194.0/255.0)*minBrightness);
    this.scene.fog.color = backFogColor;

    this.renderer.setClearColor(backFogColor, 1);

    //this.mappedChunks.clear();
    this.lightedChunks.clear();
    //this.neededChunks.clear();
    for(const i of this.mappedChunks.values())
    {
      if(!this.neededChunks.has(`${i.x},${i.y},${i.z}`))
       this.neededChunks.set(`${i.x},${i.y},${i.z}`, {x: i.x, y: i.y, z: i.z});
    }
    // this.blockChangeRequested = true;
    // window.setTimeout(()=> {
    //   this.blockChangeRequested = false;
    // }, 40000);
    this.surveyNeededChunks();
  };

  onKeyDown = (event) => {

    switch (event.code) {
      // case "KeyL":
      //   this.changeBrightness(minBrightness+0.1);
      //   break;
      //   case "KeyP":
      //     this.changeBrightness(minBrightness-0.1);
      //     break;
      case "KeyW":
        this.input.ActiveState.forward = true;
        break;
      case "KeyA":
        this.input.ActiveState.left = true;
        break;
      case "KeyS":
        this.input.ActiveState.back = true;
        break;
      case "KeyD":
        this.input.ActiveState.right = true;
        break;
      case "KeyT":
        if (document.activeElement !== document.getElementById("inp")) {
          event.preventDefault();
        }
        this.chatBoxRef.current.focus();
        break;
      case "Space":
        this.input.ActiveState.jump = true;
        this.input.ActiveState.isGrounded = false;
        break;
      case "ShiftLeft":
        this.input.ActiveState.crouch = true;
        break;
      default:
        break;
    }
  };

  onKeyUp = (event) => {
    switch (event.code) {
      default:
        break;
      case "KeyW":
        this.input.ActiveState.forward = false;
        break;
      case "KeyA":
        this.input.ActiveState.left = false;
        break;
      case "KeyS":
        this.input.ActiveState.back = false;
        break;
      case "KeyD":
        this.input.ActiveState.right = false;
        break;
      case "ShiftLeft":
        this.input.ActiveState.crouch = false;
        break;
    }
  };
  componentDidMount = () => {
    this.setSmallMode();
    this.scene.fog = new THREE.Fog(backFogColor, 20, 160);
    const gltfLoader = new GLTFLoader();
    const loadAsync = (url) => {
      return new Promise((resolve) => {
        gltfLoader.load(url, (gltf) => {
          resolve(gltf);
        });
      });
    };
    Promise.all([loadAsync("/player/scene.gltf")]).then((models) => {
      saturn = models[0].scene.children[0];
      saturn.scale.set(2, 2, 2);
      if(!this.props.isSinglePlayer) {
        this.props.socket.on("chat", (theChat) => {
          let key = generateUUID();
          this.setState((prevState) => {
            return {
              chats: [
                ...prevState.chats,
                {
                  key,
                  id: theChat.id,
                  message: theChat.message,
                },
              ],
            };
          });
          window.setTimeout(() => {
            this.setState({
              chats: this.state.chats.filter((chat) => {
                return chat.key !== key;
              }),
            });
          }, 30000);
        });

        this.props.socket.on("brightness", (theUpdate) => {
          this.changeBrightness(theUpdate.newMinBrightness);
        });

        this.props.socket.on("playerUpdate", (data) => {
          const key = data.id;
          const playerState = data;
          //console.log(data);
          if (this.allPlayerModels.has(key)) {
            let model = this.allPlayerModels.get(key);
            //console.log("Model", model);

            model.rotation.z = playerState.zrotation;
            this.playerOldAndNewPositions.set(
              key,
              new PrevAndNewPosition(
                model.position,
                new THREE.Vector3(data.x, data.y - 1.6, data.z)
              )
            );
            // model.position.x = data.x;
            //   model.position.y = data.y-1.6; //odd positioning problem on the model I'm using atm, this will be gone!
            //   model.position.z = data.z;

            this.allPlayerModels.set(key, model);
          } else {
            const newPlayer = saturn.clone();
            this.scene.add(newPlayer);
            this.allPlayerModels.set(data.id, newPlayer);
            newPlayer.position.x = data.x;
            newPlayer.position.y = data.y - 1.6; //odd positioning problem on the model I'm using atm, this will be gone!
            newPlayer.position.z = data.z;
          }
        });

        this.allBlockActionsRef.on("child_added", (snapshot) => {
          const addedAct = snapshot.val();
          switch (addedAct.action) {
            case "break":
              this.breakBlock({ x: addedAct.x, y: addedAct.y, z: addedAct.z });
              break;
            case "place":
                if(!this.world.data.has(`${addedAct.x},${addedAct.y},${addedAct.z}`)) {
                this.placeBlock(
                  { x: addedAct.x, y: addedAct.y, z: addedAct.z },
                  addedAct.type || "1"
                );
                }

              break;
            default:
              break;
          }
        });

        this.props.socket.on("playerDisconnect", (id) => {
          if (this.allPlayerModels.has(id)) {
            this.scene.remove(this.allPlayerModels.get(id));
            this.allPlayerModels.delete(id);
          }
        });
      }
    });

    this.mountListeners();
    const chunkWidth = this.chunkWidth;
    class World {
      constructor() {
        this.data = new LinkedHashMap();
        this.hasBlocksMarks = new LinkedHashMap();
        this.fullBlockMarks = new LinkedHashMap();
        this.ishandledmarks = new LinkedHashMap();
        this.lightMarks = new LinkedHashMap();
      }
      // load = (string) => {

      //       const text = LZString.decompress(string);
      //       console.log(text);
      //       const json = JSON.parse(text);

      //       //const map = new Map(Object.entries(json));
      //       //this.data = new Map([...map, ...this.data]);
      // }
      generateOneChunk(A, B, C) {
        let blockCount = 0;
        for (var o = 0 + A * chunkWidth; o < chunkWidth + A * chunkWidth; o++) {
          for (
            var o2 = 0 + B * chunkWidth;
            o2 < chunkWidth + B * chunkWidth;
            o2++
          ) {
            for (
              var o3 = 0 + C * chunkWidth;
              o3 < chunkWidth + C * chunkWidth;
              o3++
            ) {
              let realWorldX = o;
              let realWorldZ = o3;
              let realWorldY = o2;

              let n =
                ImprovedNoise.noise(
                  realWorldX / 25.34,
                  34.425,
                  realWorldZ / 25.65
                ) * 15;

              if (!this.ishandledmarks.has("" + A + "," + B + "," + C)) {
                this.ishandledmarks.set("" + A + "," + B + "," + C, "1");
              }

              if (realWorldY < n) {
                blockCount++;
                if (!this.hasBlocksMarks.has("" + A + "," + B + "," + C)) {
                  this.hasBlocksMarks.set("" + A + "," + B + "," + C, "1"); //Chunk level (zoomed out)
                }
                if (blockCount >= chunkWidth * chunkWidth * chunkWidth) {
                  this.fullBlockMarks.set("" + A + "," + B + "," + C, "1"); // Remove it if its full for now
                }

                let idToSet =
                  Math.abs(realWorldY - n) < 2
                    ? realWorldY < 1
                      ? "3"
                      : "4"
                    : "1";
                this.data.set(
                  "" + realWorldX + "," + realWorldY + "," + realWorldZ,
                  idToSet
                ); // Real this.world level (micro)
                // try {
                //   const docRef = addDoc(collection(db, "blocks"), {
                //     x: realWorldX,
                //     y: realWorldY,
                //     z: realWorldZ,
                //     id: 1
                //   });

                // } catch (e) {
                //   console.error("Error adding document: ", e);
                // }
              }
            }
          }
        }
        if (blockCount === 0) {
          return "air";
        } else {
          return "go";
        }
      }
      generate = () => {
        let realWorldX;
        let realWorldY;
        let realWorldZ;

        for (let j = -2; j < 2; j++) {
          for (let i = -5; i < 5; i++) {
            for (let k = -5; k < 5; k++) {
              let blockCount = 0;
              for (var o = 0; o < chunkWidth; o++) {
                for (var o2 = 0; o2 < chunkWidth; o2++) {
                  for (var o3 = 0; o3 < chunkWidth; o3++) {
                    realWorldX = i * chunkWidth + o;
                    realWorldZ = k * chunkWidth + o3;
                    realWorldY = j * chunkWidth + o2;

                    let n =
                      ImprovedNoise.noise(
                        realWorldX / 25.34,
                        34.425,
                        realWorldZ / 25.65
                      ) * 15;

                    if (!this.ishandledmarks.has("" + i + "," + j + "," + k)) {
                      this.ishandledmarks.set("" + i + "," + j + "," + k, "1");
                    }
                    if (realWorldY < n) {
                      blockCount++;
                      if (
                        !this.hasBlocksMarks.has("" + i + "," + j + "," + k)
                      ) {
                        this.hasBlocksMarks.set(
                          "" + i + "," + j + "," + k,
                          "1"
                        );
                      }
                      if (blockCount >= chunkWidth * chunkWidth * chunkWidth) {
                        this.fullBlockMarks.set(
                          "" + i + "," + j + "," + k,
                          "1"
                        );
                      }
                      let idToSet =
                        Math.abs(realWorldY - n) < 2
                          ? realWorldY < 1
                            ? "3"
                            : "4"
                          : "1";
                      this.data.set(
                        "" + realWorldX + "," + realWorldY + "," + realWorldZ,
                        idToSet
                      );
                    }
                  }
                }
              }
            }
          }
        }
      };
    }

    this.world = new World();

    this.camera.position.z = 0;
    this.camera.position.y = 0;
    this.camera.position.x = 0;

    this.scene.add(this.camera);

    // Lights
    const ambientLight = new THREE.AmbientLight(0xffffff, 0.1);
    ambientLight.position.set(0, 0, 0);
    this.scene.add(ambientLight);

    //const pointLight = new THREE.PointLight(0xffffff, 1);
    //pointLight.position.set(0, 50, 0);
    //this.scene.add(pointLight);

    this.canvas = document.getElementById("canvas");

    this.renderer = new THREE.WebGLRenderer({
      canvas: this.canvas,
      antialias: false,
    });

    this.renderer.setSize(this.width, this.height);
    this.renderer.setClearColor(backFogColor, 1);

    this.controls = new PointerLockControls2(this.camera, this.canvas);

    this.world.generate();

    this.populateChunkPool();

    this.runGameLoop(this.input);

    this.props.resize();
    this.onWindowResize();
  };

  addPointLight(x, y, z) {
    // const pointLight = new THREE.PointLight(0xffffff, 0.7, 10);
    // pointLight.distance = 3;
    // pointLight.position.set(x, y + 1 , z);
    // this.scene.add(pointLight);

    const lightSpots = [
      {x:x,y:y,z:z, b: b[0]},

      {x:x+1,y:y,z:z, b: b[1]},
      {x:x-1,y:y,z:z, b: b[1]},
      {x:x,y:y,z:z+1, b: b[1]},
      {x:x,y:y,z:z-1, b: b[1]},
      {x:x,y:y+1,z:z, b: b[1]},
      {x:x,y:y-1,z:z, b: b[1]},

      {x:x+2,y:y,z:z, b: b[2]},
      {x:x-2,y:y,z:z, b: b[2]},
      {x:x,y:y,z:z+2, b: b[2]},
      {x:x,y:y,z:z-2, b: b[2]},
      {x:x,y:y+2,z:z, b: b[2]},
      {x:x,y:y-2,z:z, b: b[2]},

      {x:x+3,y:y,z:z, b: b[3]},
      {x:x-3,y:y,z:z, b: b[3]},
      {x:x,y:y,z:z+3, b: b[3]},
      {x:x,y:y,z:z-3, b: b[3]},
      {x:x,y:y+3,z:z, b: b[3]},
      {x:x,y:y-3,z:z, b: b[3]},

      {x:x+4,y:y,z:z, b: b[4]},
      {x:x-4,y:y,z:z, b: b[4]},
      {x:x,y:y,z:z+4, b: b[4]},
      {x:x,y:y,z:z-4, b: b[4]},
      {x:x,y:y+4,z:z, b: b[4]},
      {x:x,y:y-4,z:z, b: b[4]},

      {x:x+1,y:y,z:z+1, b: b[2]},
      {x:x+2,y:y,z:z+1, b: b[3]},
      {x:x+3,y:y,z:z+1, b: b[4]},

      {x:x+2,y:y,z:z+2, b: b[3]},

      {x:x+1,y:y,z:z+1, b: b[2]},
      {x:x+1,y:y,z:z+2, b: b[3]},
      {x:x+1,y:y,z:z+3, b: b[4]},

      {x:x+1,y:y,z:z-1, b: b[2]},
      {x:x+2,y:y,z:z-1, b: b[3]},
      {x:x+3,y:y,z:z-1, b: b[4]},

      {x:x+2,y:y,z:z-2, b: b[3]},

      {x:x+1,y:y,z:z-1, b: b[2]},
      {x:x+1,y:y,z:z-2, b: b[3]},
      {x:x+1,y:y,z:z-3, b: b[4]},

      {x:x-1,y:y,z:z+1, b: b[2]},
      {x:x-2,y:y,z:z+1, b: b[3]},
      {x:x-3,y:y,z:z+1, b: b[4]},

      {x:x-2,y:y,z:z+2, b: b[3]},

      {x:x-1,y:y,z:z+1, b: b[2]},
      {x:x-1,y:y,z:z+2, b: b[3]},
      {x:x-1,y:y,z:z+3, b: b[4]},


      {x:x-1,y:y,z:z-1, b: b[2]},
      {x:x-2,y:y,z:z-1, b: b[3]},
      {x:x-3,y:y,z:z-1, b: b[4]},

      {x:x-2,y:y,z:z-2, b: b[3]},
      
      {x:x-1,y:y,z:z-1, b: b[2]},
      {x:x-1,y:y,z:z-2, b: b[3]},
      {x:x-1,y:y,z:z-3, b: b[4]},

      //  

      {x:x+1,y:y+1,z:z, b: b[2]},
      {x:x-1,y:y+1,z:z, b: b[2]},
      {x:x,y:y+1,z:z+1, b: b[2]},
      {x:x,y:y+1,z:z-1, b: b[2]},
      {x:x+2,y:y+1,z:z, b: b[3]},
      {x:x-2,y:y+1,z:z, b: b[3]},
      {x:x,y:y+1,z:z+2, b: b[3]},
      {x:x,y:y+1,z:z-2, b: b[3]},
      {x:x+1,y:y+1,z:z+1, b: b[2]},
      {x:x+2,y:y+1,z:z+1, b: b[3]},
      {x:x+1,y:y+1,z:z+1, b: b[2]},
      {x:x+1,y:y+1,z:z+2, b: b[3]},
      {x:x+1,y:y+1,z:z-1, b: b[2]},
      {x:x+2,y:y+1,z:z-1, b: b[3]},
      {x:x+1,y:y+1,z:z-1, b: b[2]},
      {x:x+1,y:y+1,z:z-2, b: b[3]},
      {x:x-1,y:y+1,z:z+1, b: b[2]},
      {x:x-2,y:y+1,z:z+1, b: b[3]},
      {x:x-1,y:y+1,z:z+1, b: b[2]},
      {x:x-1,y:y+1,z:z+2, b: b[3]},
      {x:x-1,y:y+1,z:z-1, b: b[2]},
      {x:x-2,y:y+1,z:z-1, b: b[3]},
      {x:x-1,y:y+1,z:z-1, b: b[2]},
      {x:x-1,y:y+1,z:z-2, b: b[3]},
       //  

       {x:x+1,y:y+2,z:z, b: b[3]},
       {x:x-1,y:y+2,z:z, b: b[3]},
       {x:x,y:y+2,z:z+1, b: b[3]},
       {x:x,y:y+2,z:z-1, b: b[3]},
       {x:x+1,y:y+2,z:z+1, b: b[3]},
       {x:x+1,y:y+2,z:z+1, b: b[3]},
       {x:x+1,y:y+2,z:z-1, b: b[3]},
       {x:x+1,y:y+2,z:z-1, b: b[3]},
       {x:x-1,y:y+2,z:z+1, b: b[3]},
       {x:x-1,y:y+2,z:z+1, b: b[3]},
       {x:x-1,y:y+2,z:z-1, b: b[3]},
       {x:x-1,y:y+2,z:z-1, b: b[3]},
             //  

      {x:x+1,y:y-1,z:z, b: b[2]},
      {x:x-1,y:y-1,z:z, b: b[2]},
      {x:x,y:y-1,z:z+1, b: b[2]},
      {x:x,y:y-1,z:z-1, b: b[2]},
      {x:x+2,y:y-1,z:z, b: b[3]},
      {x:x-2,y:y-1,z:z, b: b[3]},
      {x:x,y:y-1,z:z+2, b: b[3]},
      {x:x,y:y-1,z:z-2, b: b[3]},
      {x:x+1,y:y-1,z:z+1, b: b[3]},
      {x:x+2,y:y-1,z:z+1, b: b[4]},
      {x:x+1,y:y-1,z:z+1, b: b[3]},
      {x:x+1,y:y-1,z:z+2, b: b[4]},
      {x:x+1,y:y-1,z:z-1, b: b[3]},
      {x:x+2,y:y-1,z:z-1, b: b[4]},
      {x:x+1,y:y-1,z:z-1, b: b[3]},
      {x:x+1,y:y-1,z:z-2, b: b[4]},
      {x:x-1,y:y-1,z:z+1, b: b[3]},
      {x:x-2,y:y-1,z:z+1, b: b[4]},
      {x:x-1,y:y-1,z:z+1, b: b[3]},
      {x:x-1,y:y-1,z:z+2, b: b[4]},
      {x:x-1,y:y-1,z:z-1, b: b[3]},
      {x:x-2,y:y-1,z:z-1, b: b[4]},
      {x:x-1,y:y-1,z:z-1, b: b[3]},
      {x:x-1,y:y-1,z:z-2, b: b[4]},
       //  

       {x:x+1,y:y-2,z:z, b: b[3]},
       {x:x-1,y:y-2,z:z, b: b[3]},
       {x:x,y:y-2,z:z+1, b: b[3]},
       {x:x,y:y-2,z:z-1, b: b[3]},
       {x:x+1,y:y-2,z:z+1, b: b[4]},
       {x:x+1,y:y-2,z:z+1, b: b[4]},
       {x:x+1,y:y-2,z:z-1, b: b[4]},
       {x:x+1,y:y-2,z:z-1, b: b[4]},
       {x:x-1,y:y-2,z:z+1, b: b[4]},
       {x:x-1,y:y-2,z:z+1, b: b[4]},
       {x:x-1,y:y-2,z:z-1, b: b[4]},
       {x:x-1,y:y-2,z:z-1, b: b[4]},

    ];
    for(let i of lightSpots)
    {
      let existingLight = this.world.lightMarks.get(`${i.x},${i.y},${i.z}`) || 0;
      this.world.lightMarks.set(`${i.x},${i.y},${i.z}`, existingLight + i.b);
    }
    const i = Math.floor(x / this.chunkWidth);
    const j = Math.floor(y / this.chunkWidth);
    const k = Math.floor(z / this.chunkWidth);
    const chunkReloadSpots = [
      {x:i, y:j, z:k},
      {x:i+1, y:j, z:k},
      {x:i-1, y:j, z:k},
      {x:i, y:j+1, z:k},
      {x:i, y:j-1, z:k},
      {x:i, y:j, z:k+1},
      {x:i, y:j, z:k-1},
      {x:i+1, y:j, z:k+1},
      {x:i-1, y:j, z:k-1},
      {x:i+1, y:j, z:k-1},
      {x:i-1, y:j, z:k+1},
    ];
    for(let c of chunkReloadSpots) {
      if (this.mappedChunks.has(`${c.x},${c.y},${c.z}`)) {
        this.mappedChunks.get(`${c.x},${c.y},${c.z}`).buildMeshInPlace();
      } else {
        if (!this.neededChunks.has(`${c.x},${c.y},${c.z}`)) {
          this.neededChunks.set(`${c.x},${c.y},${c.z}`, { x: c.x, y: c.y, z: c.z });
        }
      }
    }
  }

  removePointLight(x, y, z) {
    // const pointLight = new THREE.PointLight(0xffffff, 0.7, 10);
    // pointLight.distance = 3;
    // pointLight.position.set(x, y + 1 , z);
    // this.scene.add(pointLight);

    const lightSpots = [
      {x:x,y:y,z:z, b: b[0]},

      {x:x+1,y:y,z:z, b: b[1]},
      {x:x-1,y:y,z:z, b: b[1]},
      {x:x,y:y,z:z+1, b: b[1]},
      {x:x,y:y,z:z-1, b: b[1]},
      {x:x,y:y+1,z:z, b: b[1]},
      {x:x,y:y-1,z:z, b: b[1]},

      {x:x+2,y:y,z:z, b: b[2]},
      {x:x-2,y:y,z:z, b: b[2]},
      {x:x,y:y,z:z+2, b: b[2]},
      {x:x,y:y,z:z-2, b: b[2]},
      {x:x,y:y+2,z:z, b: b[2]},
      {x:x,y:y-2,z:z, b: b[2]},

      {x:x+3,y:y,z:z, b: b[3]},
      {x:x-3,y:y,z:z, b: b[3]},
      {x:x,y:y,z:z+3, b: b[3]},
      {x:x,y:y,z:z-3, b: b[3]},
      {x:x,y:y+3,z:z, b: b[3]},
      {x:x,y:y-3,z:z, b: b[3]},

      {x:x+4,y:y,z:z, b: b[4]},
      {x:x-4,y:y,z:z, b: b[4]},
      {x:x,y:y,z:z+4, b: b[4]},
      {x:x,y:y,z:z-4, b: b[4]},
      {x:x,y:y+4,z:z, b: b[4]},
      {x:x,y:y-4,z:z, b: b[4]},

      {x:x+1,y:y,z:z+1, b: b[2]},
      {x:x+2,y:y,z:z+1, b: b[3]},
      {x:x+3,y:y,z:z+1, b: b[4]},

      {x:x+2,y:y,z:z+2, b: b[3]},

      {x:x+1,y:y,z:z+1, b: b[2]},
      {x:x+1,y:y,z:z+2, b: b[3]},
      {x:x+1,y:y,z:z+3, b: b[4]},

      {x:x+1,y:y,z:z-1, b: b[2]},
      {x:x+2,y:y,z:z-1, b: b[3]},
      {x:x+3,y:y,z:z-1, b: b[4]},

      {x:x+2,y:y,z:z-2, b: b[3]},

      {x:x+1,y:y,z:z-1, b: b[2]},
      {x:x+1,y:y,z:z-2, b: b[3]},
      {x:x+1,y:y,z:z-3, b: b[4]},

      {x:x-1,y:y,z:z+1, b: b[2]},
      {x:x-2,y:y,z:z+1, b: b[3]},
      {x:x-3,y:y,z:z+1, b: b[4]},

      {x:x-2,y:y,z:z+2, b: b[3]},

      {x:x-1,y:y,z:z+1, b: b[2]},
      {x:x-1,y:y,z:z+2, b: b[3]},
      {x:x-1,y:y,z:z+3, b: b[4]},


      {x:x-1,y:y,z:z-1, b: b[2]},
      {x:x-2,y:y,z:z-1, b: b[3]},
      {x:x-3,y:y,z:z-1, b: b[4]},

      {x:x-2,y:y,z:z-2, b: b[3]},
      
      {x:x-1,y:y,z:z-1, b: b[2]},
      {x:x-1,y:y,z:z-2, b: b[3]},
      {x:x-1,y:y,z:z-3, b: b[4]},

      //  

      {x:x+1,y:y+1,z:z, b: b[2]},
      {x:x-1,y:y+1,z:z, b: b[2]},
      {x:x,y:y+1,z:z+1, b: b[2]},
      {x:x,y:y+1,z:z-1, b: b[2]},
      {x:x+2,y:y+1,z:z, b: b[3]},
      {x:x-2,y:y+1,z:z, b: b[3]},
      {x:x,y:y+1,z:z+2, b: b[3]},
      {x:x,y:y+1,z:z-2, b: b[3]},
      {x:x+1,y:y+1,z:z+1, b: b[2]},
      {x:x+2,y:y+1,z:z+1, b: b[3]},
      {x:x+1,y:y+1,z:z+1, b: b[2]},
      {x:x+1,y:y+1,z:z+2, b: b[3]},
      {x:x+1,y:y+1,z:z-1, b: b[2]},
      {x:x+2,y:y+1,z:z-1, b: b[3]},
      {x:x+1,y:y+1,z:z-1, b: b[2]},
      {x:x+1,y:y+1,z:z-2, b: b[3]},
      {x:x-1,y:y+1,z:z+1, b: b[2]},
      {x:x-2,y:y+1,z:z+1, b: b[3]},
      {x:x-1,y:y+1,z:z+1, b: b[2]},
      {x:x-1,y:y+1,z:z+2, b: b[3]},
      {x:x-1,y:y+1,z:z-1, b: b[2]},
      {x:x-2,y:y+1,z:z-1, b: b[3]},
      {x:x-1,y:y+1,z:z-1, b: b[2]},
      {x:x-1,y:y+1,z:z-2, b: b[3]},
       //  

       {x:x+1,y:y+2,z:z, b: b[3]},
       {x:x-1,y:y+2,z:z, b: b[3]},
       {x:x,y:y+2,z:z+1, b: b[3]},
       {x:x,y:y+2,z:z-1, b: b[3]},
       {x:x+1,y:y+2,z:z+1, b: b[3]},
       {x:x+1,y:y+2,z:z+1, b: b[3]},
       {x:x+1,y:y+2,z:z-1, b: b[3]},
       {x:x+1,y:y+2,z:z-1, b: b[3]},
       {x:x-1,y:y+2,z:z+1, b: b[3]},
       {x:x-1,y:y+2,z:z+1, b: b[3]},
       {x:x-1,y:y+2,z:z-1, b: b[3]},
       {x:x-1,y:y+2,z:z-1, b: b[3]},
             //  

      {x:x+1,y:y-1,z:z, b: b[2]},
      {x:x-1,y:y-1,z:z, b: b[2]},
      {x:x,y:y-1,z:z+1, b: b[2]},
      {x:x,y:y-1,z:z-1, b: b[2]},
      {x:x+2,y:y-1,z:z, b: b[3]},
      {x:x-2,y:y-1,z:z, b: b[3]},
      {x:x,y:y-1,z:z+2, b: b[3]},
      {x:x,y:y-1,z:z-2, b: b[3]},
      {x:x+1,y:y-1,z:z+1, b: b[3]},
      {x:x+2,y:y-1,z:z+1, b: b[4]},
      {x:x+1,y:y-1,z:z+1, b: b[3]},
      {x:x+1,y:y-1,z:z+2, b: b[4]},
      {x:x+1,y:y-1,z:z-1, b: b[3]},
      {x:x+2,y:y-1,z:z-1, b: b[4]},
      {x:x+1,y:y-1,z:z-1, b: b[3]},
      {x:x+1,y:y-1,z:z-2, b: b[4]},
      {x:x-1,y:y-1,z:z+1, b: b[3]},
      {x:x-2,y:y-1,z:z+1, b: b[4]},
      {x:x-1,y:y-1,z:z+1, b: b[3]},
      {x:x-1,y:y-1,z:z+2, b: b[4]},
      {x:x-1,y:y-1,z:z-1, b: b[3]},
      {x:x-2,y:y-1,z:z-1, b: b[4]},
      {x:x-1,y:y-1,z:z-1, b: b[3]},
      {x:x-1,y:y-1,z:z-2, b: b[4]},
       //  

       {x:x+1,y:y-2,z:z, b: b[3]},
       {x:x-1,y:y-2,z:z, b: b[3]},
       {x:x,y:y-2,z:z+1, b: b[3]},
       {x:x,y:y-2,z:z-1, b: b[3]},
       {x:x+1,y:y-2,z:z+1, b: b[4]},
       {x:x+1,y:y-2,z:z+1, b: b[4]},
       {x:x+1,y:y-2,z:z-1, b: b[4]},
       {x:x+1,y:y-2,z:z-1, b: b[4]},
       {x:x-1,y:y-2,z:z+1, b: b[4]},
       {x:x-1,y:y-2,z:z+1, b: b[4]},
       {x:x-1,y:y-2,z:z-1, b: b[4]},
       {x:x-1,y:y-2,z:z-1, b: b[4]},

    ];
    for(let i of lightSpots)
    {
      let existingLight = this.world.lightMarks.get(`${i.x},${i.y},${i.z}`) || 0;
      this.world.lightMarks.set(`${i.x},${i.y},${i.z}`, Math.max(minBrightness, existingLight-i.b));
    }
    const i = Math.floor(x / this.chunkWidth);
    const j = Math.floor(y / this.chunkWidth);
    const k = Math.floor(z / this.chunkWidth);
    const chunkReloadSpots = [
      {x:i, y:j, z:k},
      {x:i+1, y:j, z:k},
      {x:i-1, y:j, z:k},
      {x:i, y:j+1, z:k},
      {x:i, y:j-1, z:k},
      {x:i, y:j, z:k+1},
      {x:i, y:j, z:k-1},
      {x:i+1, y:j, z:k+1},
      {x:i-1, y:j, z:k-1},
      {x:i+1, y:j, z:k-1},
      {x:i-1, y:j, z:k+1},
    ];
    for(let c of chunkReloadSpots) {
      if (this.mappedChunks.has(`${c.x},${c.y},${c.z}`)) {
        this.mappedChunks.get(`${c.x},${c.y},${c.z}`).buildMeshInPlace();
      } else {
        if (!this.neededChunks.has(`${c.x},${c.y},${c.z}`)) {
          this.neededChunks.set(`${c.x},${c.y},${c.z}`, { x: c.x, y: c.y, z: c.z });
        }
      }
    }
  }

  populateChunkPool() {
    const chunkWidth = this.chunkWidth;
    const world = this.world;
    const clock = this.clock;

    class Chunk {
      constructor() {
        this.meshGeometry = new THREE.BufferGeometry();
        this.vertices = new Float32Array(3);
        this.mesh = new THREE.Mesh(this.meshGeometry, meshMaterial);
        this.x = 0; //multiply x and z by 16 to get real-this.world position
        this.z = 0;
        this.y = 2000;
        this.timeLastMeshed = 0;
      }
      buildMesh(newX, newY, newZ) {
        this.timeLastMeshed = Date.now();
        this.x = newX;
        this.z = newZ;
        this.y = newY;
        this.mesh.position.set(
          this.x * chunkWidth,
          this.y * chunkWidth,
          this.z * chunkWidth
        );
        this.buildMeshInPlace();
      }
      buildMeshInPlace() {
        let newVerts = [];
        let newNorms = [];

        let newUVs = [];
        let newCols = [];

        const setLight = (i, j, k) => {
          let bright = minBrightness;
          const x = Math.floor(this.x * chunkWidth + i);
          const y = Math.floor(this.y * chunkWidth + j);
          const z = Math.floor(this.z * chunkWidth + k);
          const ky = `${x},${y + 1},${z}`;

          if (world.lightMarks.has(ky)) {
            bright = Math.min(Math.max(world.lightMarks.get(ky), bright), maxBrightness);
          }

          newCols.push(bright, bright, bright, bright);
          newCols.push(bright, bright, bright, bright);
          newCols.push(bright, bright, bright, bright);
          newCols.push(bright, bright, bright, bright);
          newCols.push(bright, bright, bright, bright);
          newCols.push(bright, bright, bright, bright);
        };


        const add4UVs = (id, side) => {
          const values = blockTypes[id].texture.uniform
            ? blockTypes[id].texture.all
            : blockTypes[id].texture[side];

          newUVs.push(values[0], values[1]);
          newUVs.push(values[2], values[3]);
          newUVs.push(values[4], values[5]);
          newUVs.push(values[4], values[5]);
          newUVs.push(values[6], values[7]);
          newUVs.push(values[0], values[1]);
        };
        const addVert = (i, j, k, x, y, z) => {
          newVerts.push(x,y,z);
          return 1;
        };

        if (
          world.fullBlockMarks.has("" + this.x + "," + this.y + "," + this.z)
        ) {
          if (
            !world.fullBlockMarks.has(
              "" + (this.x - 1) + "," + this.y + "," + this.z
            ) ||
            !world.hasBlocksMarks.has(
              "" + (this.x - 1) + "," + this.y + "," + this.z
            )
          ) {
            //left
            let v = 0;
            v += addVert(this.x, this.y, this.z, 0, 0, 0);
            v += addVert(this.x, this.y, this.z, 0, 0, chunkWidth);
            v += addVert(this.x, this.y, this.z, 0, chunkWidth, chunkWidth);
            v += addVert(this.x, this.y, this.z, 0, chunkWidth, chunkWidth);
            v += addVert(this.x, this.y, this.z, 0, chunkWidth, 0);
            v += addVert(this.x, this.y, this.z, 0, 0, 0);                    setLight(this.x*chunkWidth, this.y*chunkWidth, this.z*chunkWidth);
            add4UVs("1", "all");
            for (let h = 0; h < v; h++) {
              newNorms.push(-1, 0, 0);
            }
          }
          if (
            !world.fullBlockMarks.has(
              "" + this.x + "," + this.y + "," + (this.z - 1)
            ) ||
            !world.hasBlocksMarks.has(
              "" + (this.x - 1) + "," + this.y + "," + this.z
            )
          ) {
            //front
            let v = 0;
            v += addVert(this.x, this.y, this.z, 0, 0, 0);
            v += addVert(this.x, this.y, this.z, 0, chunkWidth, 0);
            v += addVert(this.x, this.y, this.z, chunkWidth, chunkWidth, 0);
            v += addVert(this.x, this.y, this.z, chunkWidth, chunkWidth, 0);
            v += addVert(this.x, this.y, this.z, chunkWidth, 0, 0);
            v += addVert(this.x, this.y, this.z, 0, 0, 0);
            add4UVs("1", "all");setLight(this.x*chunkWidth, this.y*chunkWidth, this.z*chunkWidth);
            for (let h = 0; h < v; h++) {
              newNorms.push(0, 0, -1);
            }
          }
          if (
            !world.fullBlockMarks.has(
              "" + (this.x + 1) + "," + this.y + "," + this.z
            ) ||
            !world.hasBlocksMarks.has(
              "" + (this.x - 1) + "," + this.y + "," + this.z
            )
          ) {
            //right
            let v = 0;
            v += addVert(this.x, this.y, this.z, chunkWidth, 0, 0);
            v += addVert(this.x, this.y, this.z, chunkWidth, chunkWidth, 0);
            v += addVert(
              this.x,
              this.y,
              this.z,
              chunkWidth,
              chunkWidth,
              chunkWidth
            );
            v += addVert(
              this.x,
              this.y,
              this.z,
              chunkWidth,
              chunkWidth,
              chunkWidth
            );
            v += addVert(this.x, this.y, this.z, chunkWidth, 0, chunkWidth);
            v += addVert(this.x, this.y, this.z, chunkWidth, 0, 0);
            add4UVs("1", "all");setLight(this.x*chunkWidth, this.y*chunkWidth, this.z*chunkWidth);
            for (let h = 0; h < v; h++) {
              newNorms.push(1, 0, 0);
            }
          }
          if (
            !world.fullBlockMarks.has(
              "" + this.x + "," + this.y + "," + this.z + 1
            ) ||
            !world.hasBlocksMarks.has(
              "" + (this.x - 1) + "," + this.y + "," + this.z
            )
          ) {
            //back
            let v = 0;
            v += addVert(this.x, this.y, this.z, chunkWidth, 0, chunkWidth);
            v += addVert(
              this.x,
              this.y,
              this.z,
              chunkWidth,
              chunkWidth,
              chunkWidth
            );
            v += addVert(this.x, this.y, this.z, 0, chunkWidth, chunkWidth);
            v += addVert(this.x, this.y, this.z, 0, chunkWidth, chunkWidth);
            v += addVert(this.x, this.y, this.z, 0, 0, chunkWidth);
            v += addVert(this.x, this.y, this.z, chunkWidth, 0, chunkWidth);
            add4UVs("1", "all");setLight(this.x*chunkWidth, this.y*chunkWidth, this.z*chunkWidth);
            for (let h = 0; h < v; h++) {
              newNorms.push(0, 0, 1);
            }
          }
          if (
            !world.fullBlockMarks.has(
              "" + this.x + "," + (this.y - 1) + "," + this.z
            ) ||
            !world.hasBlocksMarks.has(
              "" + (this.x - 1) + "," + this.y + "," + this.z
            )
          ) {
            //bottom
            let v = 0;
            v += addVert(this.x, this.y, this.z, 0, 0, chunkWidth);
            v += addVert(this.x, this.y, this.z, 0, 0, 0);
            v += addVert(this.x, this.y, this.z, chunkWidth, 0, 0);
            v += addVert(this.x, this.y, this.z, chunkWidth, 0, 0);
            v += addVert(this.x, this.y, this.z, chunkWidth, 0, chunkWidth);
            v += addVert(this.x, this.y, this.z, 0, 0, chunkWidth);
            add4UVs("1", "all");setLight(this.x*chunkWidth, this.y*chunkWidth, this.z*chunkWidth);
            for (let h = 0; h < v; h++) {
              newNorms.push(0, -1, 0);
            }
          }
          if (
            !world.fullBlockMarks.has(
              "" + this.x + "," + (this.y + 1) + "," + this.z
            ) ||
            !world.hasBlocksMarks.has(
              "" + (this.x - 1) + "," + this.y + "," + this.z
            )
          ) {
            //top
            let v = 0;
            v += addVert(this.x, this.y, this.z, 0, chunkWidth, 0);
            v += addVert(this.x, this.y, this.z, 0, chunkWidth, chunkWidth);
            v += addVert(
              this.x,
              this.y,
              this.z,
              chunkWidth,
              chunkWidth,
              chunkWidth
            );
            v += addVert(
              this.x,
              this.y,
              this.z,
              chunkWidth,
              chunkWidth,
              chunkWidth
            );
            v += addVert(this.x, this.y, this.z, chunkWidth, chunkWidth, 0);
            v += addVert(this.x, this.y, this.z, 0, chunkWidth, 0);
            add4UVs("1", "all");setLight(this.x*chunkWidth, this.y*chunkWidth, this.z*chunkWidth);
            for (let h = 0; h < v; h++) {
              newNorms.push(0, 1, 0);
            }
          }
        } else 
        {
          for (let j = 0; j < chunkWidth; j++) {
            for (let i = 0; i < chunkWidth; i++) {
              for (let k = 0; k < chunkWidth; k++) {
                if (
                  world.data.has(
                    "" +
                      (this.x * chunkWidth + i) +
                      "," +
                      (this.y * chunkWidth + j) +
                      "," +
                      (this.z * chunkWidth + k)
                  )
                ) {
                  const ID = world.data.get(
                    "" +
                      (this.x * chunkWidth + i) +
                      "," +
                      (this.y * chunkWidth + j) +
                      "," +
                      (this.z * chunkWidth + k)
                  );
                  if (
                    !world.data.has(
                      "" +
                        (this.x * chunkWidth + i - 1) +
                        "," +
                        (this.y * chunkWidth + j) +
                        "," +
                        (this.z * chunkWidth + k)
                    )
                  ) {
                    let v = 0;
                    v += addVert(i, j, k, i, j, k);
                    v += addVert(i, j, k, i, j, k + 1);
                    v += addVert(i, j, k, i, j + 1, k + 1);
                    v += addVert(i, j, k, i, j + 1, k + 1);
                    v += addVert(i, j, k, i, j + 1, k);
                    v += addVert(i, j, k, i, j, k);
                    add4UVs(ID, "sides");
                    setLight(i, j, k);
                    for (let h = 0; h < v; h++) {
                      newNorms.push(-1, 0, 0);
                    }
                  }
                  if (
                    !world.data.has(
                      "" +
                        (this.x * chunkWidth + i) +
                        "," +
                        (this.y * chunkWidth + j) +
                        "," +
                        (this.z * chunkWidth + k - 1)
                    )
                  ) {
                    let v = 0;
                    v += addVert(i, j, k, i + 1, j, k);
                    v += addVert(i, j, k, i, j, k);
                    v += addVert(i, j, k, i, j + 1, k);
                    v += addVert(i, j, k, i, j + 1, k);
                    v += addVert(i, j, k, i + 1, j + 1, k);
                    v += addVert(i, j, k, i + 1, j, k);
                    add4UVs(ID, "sides");
                    setLight(i, j, k);
                    for (let h = 0; h < v; h++) {
                      newNorms.push(0, 0, -1);
                    }
                  }
                  if (
                    !world.data.has(
                      "" +
                        (this.x * chunkWidth + i + 1) +
                        "," +
                        (this.y * chunkWidth + j) +
                        "," +
                        (this.z * chunkWidth + k)
                    )
                  ) {
                    let v = 0;
                    v += addVert(i, j, k, i + 1, j, k + 1);
                    v += addVert(i, j, k, i + 1, j, k);

                    v += addVert(i, j, k, i + 1, j + 1, k);
                    v += addVert(i, j, k, i + 1, j + 1, k);
                    v += addVert(i, j, k, i + 1, j + 1, k + 1);
                    v += addVert(i, j, k, i + 1, j, k + 1);
                    add4UVs(ID, "sides");
                    setLight(i, j, k);
                    for (let h = 0; h < v; h++) {
                      newNorms.push(1, 0, 0);
                    }
                  }
                  if (
                    !world.data.has(
                      "" +
                        (this.x * chunkWidth + i) +
                        "," +
                        (this.y * chunkWidth + j) +
                        "," +
                        (this.z * chunkWidth + k + 1)
                    )
                  ) {
                    let v = 0;
                    v += addVert(i, j, k, i, j, k + 1);
                    v += addVert(i, j, k, i + 1, j, k + 1);
                    v += addVert(i, j, k, i + 1, j + 1, k + 1);
                    v += addVert(i, j, k, i + 1, j + 1, k + 1);
                    v += addVert(i, j, k, i, j + 1, k + 1);
                    v += addVert(i, j, k, i, j, k + 1);
                    add4UVs(ID, "sides");
                    setLight(i, j, k);
                    for (let h = 0; h < v; h++) {
                      newNorms.push(0, 0, 1);
                    }
                  }
                  if (
                    !world.data.has(
                      "" +
                        (this.x * chunkWidth + i) +
                        "," +
                        (this.y * chunkWidth + j - 1) +
                        "," +
                        (this.z * chunkWidth + k)
                    )
                  ) {
                    let v = 0;
                    v += addVert(i, j, k, i, j, k);
                    v += addVert(i, j, k, i + 1, j, k);
                    v += addVert(i, j, k, i + 1, j, k + 1);
                    v += addVert(i, j, k, i + 1, j, k + 1);
                    v += addVert(i, j, k, i, j, k + 1);
                    v += addVert(i, j, k, i, j, k);
                    add4UVs(ID, "bottom");
                    setLight(i, j, k);
                    for (let h = 0; h < v; h++) {
                      newNorms.push(0, -1, 0);
                    }
                  }
                  if (
                    !world.data.has(
                      "" +
                        (this.x * chunkWidth + i) +
                        "," +
                        (this.y * chunkWidth + j + 1) +
                        "," +
                        (this.z * chunkWidth + k)
                    )
                  ) {
                    let v = 0;
                    v += addVert(i, j, k, i, j + 1, k);
                    v += addVert(i, j, k, i, j + 1, k + 1);
                    v += addVert(i, j, k, i + 1, j + 1, k + 1);
                    v += addVert(i, j, k, i + 1, j + 1, k + 1);
                    v += addVert(i, j, k, i + 1, j + 1, k);
                    v += addVert(i, j, k, i, j + 1, k);
                    add4UVs(ID, "top");
                    setLight(i, j, k);
                    for (let h = 0; h < v; h++) {
                      newNorms.push(0, 1, 0);
                    }
                  }
                }
              }
            }
          }
        }
        this.vertices = new Float32Array(newVerts);
        this.meshGeometry.setAttribute(
          "position",
          new THREE.BufferAttribute(this.vertices, 3)
        );
        this.meshGeometry.setAttribute(
          "color",
          new THREE.BufferAttribute(new Float32Array(newCols), 4)
        );
        this.meshGeometry.setAttribute(
          "normal",
          new THREE.BufferAttribute(new Float32Array(newNorms), 3)
        );
        this.meshGeometry.setAttribute(
          "uv",
          new THREE.BufferAttribute(new Float32Array(newUVs), 2)
        );

        this.mesh.geometry = this.meshGeometry;
      }
    }
    for (let i = 0; i < 10; i++) {
      for (let k = 0; k < 10; k++) {
        for (let a = 0; a < 10; a++) {
          let testChunk = new Chunk();
          testChunk.mesh.frustumCulled = false;
          this.chunkpool.push(testChunk);
          this.scene.add(testChunk.mesh);
        }
      }
    }
  }

  rebuildQueuedMeshes() {
    if (
      this.chunkQueueTimer > this.chunkQueueInterval &&
      this.neededChunks.size !== 0
    ) {
      this.chunkQueueTimer = 0;
      const needed = Array.from(this.neededChunks.values());

      // needed.sort(
      //   (a,b)=>
      //   {
      //     if(this.camera.position.distanceTo(new THREE.Vector3(a.x, a.y, a.z)) < 50 &&
      //     this.camera.position.distanceTo(new THREE.Vector3(b.x, b.y, b.z)) < 50)
      //     {
      //       return 0;
      //     }
      //     return (this.camera.position.distanceTo(new THREE.Vector3(a.x, a.y, a.z))
      //     <
      //     this.camera.position.distanceTo(new THREE.Vector3(b.x, b.y, b.z))
      //     ) ? -1 : 1;

      //   }
      // );

      /*needed.sort(
        (a,b) => {
          return (this.isCameraFacingThis(this.getCameraDirection(), a.x, a.y, a.z)) ? -1: 1;
        }
      );*/

      const neededSpot = needed[0];

      if (
        !this.mappedChunks.has(
          `${neededSpot.x},${neededSpot.y},${neededSpot.z}`
        )
      ) {
        let cp = this.chunkpool.filter((c) => {
          return Date.now() - c.timeLastMeshed > 12000;
        });

        cp.sort((a, b) => {
          return this.camera.position.distanceTo(
            new THREE.Vector3(a.x, a.y, a.z)
          ) < this.camera.position.distanceTo(new THREE.Vector3(b.x, b.y, b.z))
            ? -1
            : 1;
        });

        let grabbedMesh = cp.pop();
        if (grabbedMesh !== null && grabbedMesh !== undefined) {
          if (
            this.mappedChunks.has(
              "" + grabbedMesh.x + "," + grabbedMesh.y + "," + grabbedMesh.z
            )
          ) {
            this.mappedChunks.delete(
              "" + grabbedMesh.x + "," + grabbedMesh.y + "," + grabbedMesh.z
            );
          }
          //this.scene.remove(grabbedMesh.mesh);
          grabbedMesh.buildMesh(neededSpot.x, neededSpot.y, neededSpot.z);
          //this.scene.add(grabbedMesh.mesh);
          this.mappedChunks.set(
            "" + neededSpot.x + "," + neededSpot.y + "," + neededSpot.z,
            grabbedMesh
          );
          this.lightedChunks.set(
            "" + neededSpot.x + "," + neededSpot.y + "," + neededSpot.z,
            "i"
          );
          this.neededChunks.delete(
            "" + neededSpot.x + "," + neededSpot.y + "," + neededSpot.z
          );
        }
      } else {
        //console.log("Mapped chunks already has", neededSpot.x, neededSpot.y, neededSpot.z)
        this.mappedChunks
          .get(`${neededSpot.x},${neededSpot.y},${neededSpot.z}`)
          .buildMeshInPlace();
        this.neededChunks.delete(
          `${neededSpot.x},${neededSpot.y},${neededSpot.z}`
        );
      }
    } else {
      this.chunkQueueTimer++;
    }
  }

  surveyNeededChunks() {
    if (this.camera !== null && this.camera !== undefined) {
      let y = -this.chunkWidth * 2;
      for (
        let i = -this.chunkWidth * 5;
        i < this.chunkWidth * 5;
        i += this.chunkWidth
      ) {
        for (
          let k = -this.chunkWidth * 5;
          k < this.chunkWidth * 5;
          k += this.chunkWidth
        ) {
          let THERIGHTX =
            this.camera.position.x - (this.camera.position.x % this.chunkWidth);
          let THERIGHTY =
            this.camera.position.y - (this.camera.position.y % this.chunkWidth);
          let THERIGHTZ =
            this.camera.position.z - (this.camera.position.z % this.chunkWidth);
          let x = Math.round((i + THERIGHTX) / this.chunkWidth);
          let z = Math.round((k + THERIGHTZ) / this.chunkWidth);
          let yy = Math.round((y + THERIGHTY) / this.chunkWidth);
          let obj = { x: x, y: yy, z: z };
          if (this.world.ishandledmarks.has("" + x + "," + yy + "," + z)) {
            if (
              this.world.hasBlocksMarks.has("" + x + "," + yy + "," + z) &&
              !this.mappedChunks.has("" + x + "," + yy + "," + z)
            ) {
              

              if (!this.neededChunks.has("" + x + "," + yy + "," + z)) {
                // if it needs to tell neededchunks it needs this
                this.neededChunks.set("" + x + "," + yy + "," + z, obj);
                let h = 1;

                while (
                  this.world.hasBlocksMarks.has(
                    "" + x + "," + (yy + h) + "," + z
                  )
                  
                ) {
                  this.neededChunks.set("" + x + "," + (yy + h) + "," + z, {
                    x,
                    y: yy + h,
                    z,
                  });
                  h += 1;
                }
              }
            } else if(!this.lightedChunks.has("" + x + "," + yy + "," + z)){
              if (!this.neededChunks.has("" + x + "," + yy + "," + z)) {
                // if it needs to tell neededchunks it needs this
                this.neededChunks.set("" + x + "," + yy + "," + z, obj);
                let h = 1;

                while (
                  this.world.hasBlocksMarks.has(
                    "" + x + "," + (yy + h) + "," + z
                  )
                  && !this.lightedChunks.has("" + x + "," + (yy + h) + "," + z)
                ) {
                  this.neededChunks.set("" + x + "," + (yy + h) + "," + z, {
                    x,
                    y: yy + h,
                    z,
                  });
                  h += 1;
                }
              }
            }
          } else {
            let h = 0;
            let cont = true;
            while (
              !this.world.ishandledmarks.has(
                "" + x + "," + (yy + h) + "," + z
              ) &&
              h < 4 &&
              cont
            ) {
              const resrej = (resolve, reject) => {
                try {
                  const res = this.world.generateOneChunk(x, yy + h, z);
                  cont = res !== "air";
                  resolve(null);
                } catch (e) {
                  reject(e);
                }
              };
              let prom = new Promise(resrej);
              prom
                .then((result) => {
                  //yay
                })
                .catch((error) => {
                  console.log(error);
                });
              h += 1;
            }
          }
        }
      }
    }
  }

  castRayBlocking = (x, y, z, dx, dy, dz, maxDistance) => {
    let distanceTraveled = 0;
    let currentX = x;
    let currentY = y;
    let currentZ = z;

    while (distanceTraveled < maxDistance) {
      currentX += dx;
      currentY += dy;
      currentZ += dz;
      distanceTraveled += 0.1;

      const key = `${Math.floor(currentX)},${Math.floor(currentY)},${Math.floor(
        currentZ
      )}`;
      if (this.world.data.has(key)) {
        return key;
      }
    }

    return null;
  };

  sendChat = (event) => {
    event.preventDefault();
    if(!this.props.isSinglePlayer)
    {
    const key = generateUUID();
    const chat = {
      id: this.props.name,
      message: this.state.chat,
      key,
    };
    this.props.socket.emit("chat", chat);
    this.setState((prevState) => {
      return { chats: [...prevState.chats, chat] };
    });
    window.setTimeout(() => {
      this.setState({
        chats: this.state.chats.filter((chat) => {
          return chat.key !== key;
        }),
      });
    }, 10000);
    this.setState({
      chat: "",
    });
    this.chatBoxRef.current.blur();
  }
  };

  runGameLoop(input) {
    let myPrevGO = {
      x: 0,
      y: 0,
      z: 0,
      zrotation: 0,
    };
    const collisionDistance = 0.1;
    const animate = () => {
      this.input.ActiveState.isGrounded =
        !this.isReady ||
        this.castRayBlocking(
          this.camera.position.x,
          this.camera.position.y - 1,
          this.camera.position.z,
          0,
          -1,
          0,
          collisionDistance
        ) !== null;
      if (!this.input.ActiveState.isGrounded) {
        this.input.ActiveState.jumpTimer += this.delt * 12;
        this.camera.position.y +=
          (6 - this.input.ActiveState.jumpTimer) * this.delt;
      }
      //console.log(this.mappedChunks.size );
      if (this.mappedChunks.size < 125) {
        this.props.handle()("messageToClient")("loadingworld");
      } else {
        this.props.handle()("messageToClient")("none");
        this.isReady = true;
      }
      if(!this.props.isSinglePlayer) {
      if (this.updatePlayersTimer > this.updatePlayersInterval) {
        this.updatePlayersTimer = 0;
        if (
          this.controls.camera.rotation.y !== myPrevGO.zrotation ||
          this.controls.camera.position.x !== myPrevGO.x ||
          this.controls.camera.position.y !== myPrevGO.y ||
          this.controls.camera.position.z !== myPrevGO.z
        ) {
          let upd = {
            id: this.props.pid,
            zrotation: this.controls.camera.rotation.y,
            x: this.camera.position.x,
            y: this.camera.position.y,
            z: this.camera.position.z,
          };
          this.props.socket.emit("playerUpdate", upd);
          myPrevGO = {
            ...upd,
          };
        }
      } else {
        this.delt = this.clock.getDelta();
        this.updatePlayersTimer += this.delt;
        for (const [key, value] of this.allPlayerModels.entries()) {
          if (this.playerOldAndNewPositions.has(key)) {
            let model = value;
            model.position.lerpVectors(
              this.playerOldAndNewPositions.get(key).prevPosition,
              this.playerOldAndNewPositions.get(key).newPosition,
              this.updatePlayersTimer
            );
            this.allPlayerModels.set(key, model);
          }
        }
      }
    } else {
      this.delt = this.clock.getDelta();
    }
      if (this.isReady) {
        if (input.ActiveState.forward) {
          let dir = this.getCameraDirection();
          dir.y = 0;
          if (
            this.castRayBlocking(
              this.camera.position.x,
              this.camera.position.y,
              this.camera.position.z,
              dir.x,
              dir.y,
              dir.z,
              collisionDistance
            ) === null &&
            this.castRayBlocking(
              this.camera.position.x,
              this.camera.position.y - 1,
              this.camera.position.z,
              dir.x,
              dir.y,
              dir.z,
              collisionDistance
            ) === null
          ) {
            this.controls.moveForward(this.delt * 6);
          }
        }
        if (input.ActiveState.back) {
          let dir = this.getCameraDirection();
          dir.y = 0;
          if (
            this.castRayBlocking(
              this.camera.position.x,
              this.camera.position.y,
              this.camera.position.z,
              -dir.x,
              -dir.y,
              -dir.z,
              collisionDistance
            ) === null &&
            this.castRayBlocking(
              this.camera.position.x,
              this.camera.position.y - 1,
              this.camera.position.z,
              -dir.x,
              -dir.y,
              -dir.z,
              collisionDistance
            ) === null
          ) {
            this.controls.moveForward(-this.delt * 6);
          }
        }
        if (input.ActiveState.right) {
          let dir = this.getCameraDirectionRight();
          dir.y = 0;
          if (
            this.castRayBlocking(
              this.camera.position.x,
              this.camera.position.y,
              this.camera.position.z,
              dir.x,
              dir.y,
              dir.z,
              collisionDistance
            ) === null &&
            this.castRayBlocking(
              this.camera.position.x,
              this.camera.position.y - 1,
              this.camera.position.z,
              dir.x,
              dir.y,
              dir.z,
              collisionDistance
            ) === null
          ) {
            this.controls.moveRight(this.delt * 6);
          }
        }
        if (input.ActiveState.left) {
          let dir = this.getCameraDirectionRight();
          dir.y = 0;
          if (
            this.castRayBlocking(
              this.camera.position.x,
              this.camera.position.y,
              this.camera.position.z,
              -dir.x,
              -dir.y,
              -dir.z,
              collisionDistance
            ) === null &&
            this.castRayBlocking(
              this.camera.position.x,
              this.camera.position.y - 1,
              this.camera.position.z,
              -dir.x,
              -dir.y,
              -dir.z,
              collisionDistance
            ) === null
          ) {
            this.controls.moveRight(-this.delt * 6);
          }
        }
        if (input.ActiveState.jump) {
          if (
            !this.input.ActiveState.isGrounded ||
            this.input.ActiveState.jumpTimer < 0.1
          ) {
            this.camera.position.y +=
              (6 - this.input.ActiveState.jumpTimer) * this.delt;
          } else {
            this.input.ActiveState.jumpTimer = 0;
            this.input.ActiveState.jump = false;
          }
        }
        if (input.ActiveState.down) {
          this.camera.position.y -= this.delt * 8;
        }
      }

      this.rebuildQueuedMeshes();
      this.renderer.render(this.scene, this.camera);
      if (this.isOpen) {
        window.requestAnimationFrame(animate);
      }
    };
    if (this.isOpen) {
      animate();
    }
  }
}
