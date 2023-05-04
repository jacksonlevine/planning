import React, { Component } from "react";
import "firebase/app";
import firebase from "firebase/compat/app";
import "firebase/compat/database";
import * as THREE from "three";
import { PointerLockControls } from "three/examples/jsm/controls/PointerLockControls.js";
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



class InputHandler {
  constructor() {
    this.ActiveState = new InputState();
  }
}
const texture = new THREE.TextureLoader().load( "textures/textures.png" );
texture.magFilter = THREE.NearestFilter;
texture.minFilter = THREE.LinearFilter;
texture.generateMipmaps = false;
texture.antialias = false;

const meshMaterial = new THREE.MeshLambertMaterial({
  color: 0xffffff,
  depthWrite: true
});
meshMaterial.map = texture;
meshMaterial.emissiveIntensity = 0;

class PrevAndNewPosition {
    constructor(vector1,vector2)
    {
        this.prevPosition = vector1;
        this.newPosition = vector2;
    }
}
const texPad = .0015

const blockTypes = {
  "1": { //stone
    texture: {
      uniform:true,
      all: [
                0.0+.0625-texPad, 1.0-.0625+texPad,
                0.0+texPad, 1.0-.0625+ texPad,
                0.0+texPad ,      1.0-texPad,
                0.0+.0625-texPad, 1.0-texPad
      ]
    }
  },
  "2": { //b rick
    texture: {
      uniform:true,
      all: [
        .0625+ 0.0+.0625-texPad, 1.0-.0625+texPad,
        .0625 +0.0+texPad, 1.0-.0625+ texPad,
        .0625 +0.0+texPad ,      1.0-texPad,
        .0625 +0.0+.0625-texPad, 1.0-texPad
      ]
    }
  },
  "3": { //sand
    texture: {
      uniform:true,
      all: [
        .0625*2+ 0.0+.0625-texPad, 1.0-.0625+texPad,
        .0625*2 +0.0+texPad, 1.0-.0625+ texPad,
        .0625*2 +0.0+texPad ,      1.0-texPad,
        .0625*2 +0.0+.0625-texPad, 1.0-texPad
      ]
    }
  },
  "4": { //grass
    texture: {
      uniform:false,
      "top": [
        .0625*3+ 0.0+.0625-texPad, 1.0-.0625+texPad,
        .0625*3 +0.0+texPad, 1.0-.0625+ texPad,
        .0625*3 +0.0+texPad ,      1.0-texPad,
        .0625*3 +0.0+.0625-texPad, 1.0-texPad
      ],
      "sides": [
        .0625*4+ 0.0+.0625-texPad, 1.0-.0625+texPad,
        .0625*4 +0.0+texPad, 1.0-.0625+ texPad,
        .0625*4 +0.0+texPad ,      1.0-texPad,
        .0625*4 +0.0+.0625-texPad, 1.0-texPad
      ],
      "bottom": [
        .0625*5+ 0.0+.0625-texPad, 1.0-.0625+texPad,
        .0625*5 +0.0+texPad, 1.0-.0625+ texPad,
        .0625*5 +0.0+texPad ,      1.0-texPad,
        .0625*5 +0.0+.0625-texPad, 1.0-texPad
      ]
    }
  }
}

export default class Game extends Component {
  constructor(props) {
    super(props);
    this.allPlayerModels = new Map();
    this.allPlayersRef = firebase.database().ref("players");
    this.allBlockActionsRef = firebase.database().ref("blockActions");
    
    this.width = 1024;
    this.height = 640;
    this.fov = 100;

        // Create the Neck object and add it to the scene

    this.camera = new THREE.PerspectiveCamera(
      this.fov,
      this.width /this.height,
      0.001,
      1000
    );
    this.players = {};
    this.isOpen = true;
    this.styles = {
      display:"flex",
      flexDirection: "column",
      alignItems: "center"
    };
    this.canvas = null;
    this.controls = null;
    this.chunk_width = 11;
    this.scene = new THREE.Scene();
    this.world = null;
    this.surveyNeededChunksTimer = 0;
    this.surveyNeededChunksInterval = 5;
    this.chunkQueueTimer = 0;
    this.chunkQueueInterval = 0;
    this.mappedChunks = new Map();
    this.neededChunks = new Map();
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
      smallMode: false,
      currentlyPlacingId: 2
    }
    this.chatBoxRef = React.createRef();
    this.canvasRef = React.createRef();
    this.thingyDown = false; //small mode thing
    this.moveyDown = false; //small mode thing
  }

  isCameraFacingThis = (direction, x, y, z) => {
    const spot = new THREE.Vector3(x,y,z)
    return (spot.distanceTo(this.camera.position) 
    < 
    spot.distanceTo(this.camera.position.add(direction.addScalar(2.0))));
  }

  render() {

    //console.log(this.props.pid)
    return (
      <div style={this.styles}>
        <div style = {{
          textAlign: "center"
        }}>
        
        <div
        style={{
          display:"flex",
          flexDirection:"column",
          alignItems:"center"
        }}>

          <div style={{
            position:"relative"
          }}>
          <img style = {{
           position:"absolute",
           zIndex:"10",
           margin:"0px",
           top:"50%",
           left:"50%",
           transform: "translate(-50%, -50%)"
        }}src="/textures/hairsmall.png" alt=""/>
          <UpAndDownArrow isHidden={!this.state.smallMode}/>
          <MoveArrow isHidden={!this.state.smallMode}/>
          <p><strong>WARNING:</strong> The site is in development and may have strange issues or random interruptions. We are working to solve your issues!</p>
          <canvas 
          ref={this.canvasRef}
          style={{
            zIndex:"-1"
          }} id="canvas"></canvas>
          </div>
          <p>Chat: Press 't' to type and 'Enter' to send.</p>
          <ChatView chats = {this.state.chats}/>
          <Inventory id={this.state.currentlyPlacingId} />
        </div>
        </div>
        <form onSubmit={this.sendChat}>
          <input ref = {this.chatBoxRef} value={this.state.chat} tabIndex={1} type="text" id="inp" name="inp"
          onChange={(e)=>{this.setState({
            chat: e.target.value
          })}}></input>
          <button type="submit">Send chat</button>
        </form>
        <button
          onClick={() => {
            this.isOpen = false;
            this.props.switcher("default");
          }}
        >
          Back to main
        </button>
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
        currentX += dx*.05;
        currentY += dy*.05;
        currentZ += dz*.05;
        distanceTraveled += .05;
  
        const key = `${Math.floor(currentX)},${Math.floor(currentY)},${Math.floor(currentZ)}`;
        if (this.world.data.has(key)) {
          resolve(
            {
              x: Math.floor(currentX- (dx*stepback)) ,
              y: Math.floor(currentY- (dy*stepback)) ,
              z: Math.floor(currentZ- (dz*stepback)) 
            }
          );
          return;
        }
      }
  
      reject('Raycast did not hit anything');
    });
  }

  getCameraDirection = () => {
    var vector = new THREE.Vector3( 0, 0, - 1 );
    vector.applyQuaternion( this.camera.quaternion );
    return vector;
  }

  getCameraDirectionRight = () => {
    const cameraDirection = this.getCameraDirection();
    cameraDirection.y = 0;
    const upVector = new THREE.Vector3(0, 1, 0);
    const rightVector = cameraDirection.clone().cross(upVector).normalize();
    return rightVector;
  }

  onClick = (event) => {
    switch(event.which) {
      case 1: //Left click
      if(!this.controls.isLocked)
      {
        this.controls.lock();
      }
      else {
        let vector = this.getCameraDirection();

        this.castRay(this.camera.position.x, 
          this.camera.position.y, 
          this.camera.position.z, vector.x,
          vector.y, vector.z, 5, 0)
          .then((pos) => {
            this.breakBlock(pos);
            let actionRef = firebase.database().ref(`blockActions/${generateUUID()}`);
            actionRef.set(
              {
                action: "break",
                time: new Date().getTime(),
                ...pos
              });
          })
          .catch((error) => {
            console.log(error);
          });
      }
      break;
      case 3: //Right click
        event.preventDefault();
        let vector = this.getCameraDirection();

      this.castRay(this.camera.position.x, 
        this.camera.position.y, 
        this.camera.position.z, vector.x,
        vector.y, vector.z, 5, .3)
        .then((pos) => {
          let placed = `${this.state.currentlyPlacingId}`;
          this.placeBlock(pos, placed);
          let actionRef = firebase.database().ref(`blockActions/${generateUUID()}`);
          actionRef.set(
            {
              action: "place",
              type: placed,
              time: new Date().getTime(),
              ...pos
            });
        })
        .catch((error) => {
        });
        break;
        default:
          break;
  }
  };

  breakBlock(pos)
  {
    const blockKey = `${pos.x},${pos.y},${pos.z}`; 
          this.world.data.delete(blockKey);
          
          const chunkX = Math.floor((pos.x)/this.chunk_width);
          const chunkY = Math.floor((pos.y)/this.chunk_width);
          const chunkZ = Math.floor((pos.z)/this.chunk_width);
          if(this.world.fullblockmarks.has(`${chunkX},${chunkY},${chunkZ}`)){
            this.world.fullblockmarks.delete(`${chunkX},${chunkY},${chunkZ}`);
          }
          if(this.mappedChunks.has(`${chunkX},${chunkY},${chunkZ}`))
          {
           const chunk = this.mappedChunks.get(`${chunkX},${chunkY},${chunkZ}`);
            chunk.buildmeshinplace();
            
          
          } else {
            this.neededChunks.set(`${chunkX},${chunkY},${chunkZ}`, {x:chunkX, y:chunkY, z:chunkZ})
          }
            //6 surrounders
            if(this.mappedChunks.has(`${chunkX-1},${chunkY},${chunkZ}`))
          {
            this.mappedChunks.get(`${chunkX-1},${chunkY},${chunkZ}`)
            .buildmeshinplace();
          }
          if(this.mappedChunks.has(`${chunkX+1},${chunkY},${chunkZ}`))
          {
            this.mappedChunks.get(`${chunkX+1},${chunkY},${chunkZ}`)
            .buildmeshinplace();
          }
          if(this.mappedChunks.has(`${chunkX},${chunkY-1},${chunkZ}`))
          {
            this.mappedChunks.get(`${chunkX},${chunkY-1},${chunkZ}`)
            .buildmeshinplace();
          }
          if(this.mappedChunks.has(`${chunkX},${chunkY+1},${chunkZ}`))
          {
            this.mappedChunks.get(`${chunkX},${chunkY+1},${chunkZ}`)
            .buildmeshinplace();
          }
          if(this.mappedChunks.has(`${chunkX},${chunkY},${chunkZ-1}`))
          {
            this.mappedChunks.get(`${chunkX},${chunkY},${chunkZ-1}`)
            .buildmeshinplace();
          }
            if(this.mappedChunks.has(`${chunkX},${chunkY},${chunkZ+1}`))
          {
            this.mappedChunks.get(`${chunkX},${chunkY},${chunkZ+1}`)
            .buildmeshinplace();
          }
  }

  placeBlock(pos, id)
  {
    
    let [chunkx, chunky, chunkz] = [Math.floor(pos.x/this.chunk_width), Math.floor(pos.y/this.chunk_width), Math.floor(pos.z/this.chunk_width)];

    if(!this.world.ishandledmarks.has(
      `${chunkx},${chunky},${chunkz}`
    )){
      this.world.generateOneChunk(chunkx, chunky, chunkz);
    }
    const blockKey = `${pos.x},${pos.y},${pos.z}`; 
          this.world.data.set(blockKey, id);
          const chunkX = Math.floor(pos.x/this.chunk_width);
          const chunkY = Math.floor(pos.y/this.chunk_width);
          const chunkZ = Math.floor(pos.z/this.chunk_width);
          if(!this.world.hasblockmarks.has(`${chunkX},${chunkY},${chunkZ}`)){
            this.world.hasblockmarks.set(`${chunkX},${chunkY},${chunkZ}`, "1");
          }
          if(this.mappedChunks.has(`${chunkX},${chunkY},${chunkZ}`))
          {
            this.mappedChunks.get(`${chunkX},${chunkY},${chunkZ}`)
            .buildmeshinplace();
          }
          else{
            this.neededChunks.set(`${chunkX},${chunkY},${chunkZ}`, {x:chunkX, y:chunkY, z:chunkZ})
          }
  }

  componentWillUnmount() {
    window.removeEventListener("keydown", this.onKeyDown, false);

    window.removeEventListener("keyup", this.onKeyUp, false);
    
    window.removeEventListener("touchstart", this.onTouchStart, false)
    window.removeEventListener("touchmove", this.onTouchMove, false)
    window.removeEventListener("touchend", this.onTouchEnd, false)
    window.removeEventListener("mousedown", this.onClick);
  }

  mountListeners = () => {
    window.addEventListener("wheel", (event) => {

       this.setState({
         currentlyPlacingId: Math.max(Math.min(this.state.currentlyPlacingId + (event.deltaY/100), 4), 1)
      })
    });
    window.setInterval(()=>{
      this.surveyNeededChunks()}, 2000);

      window.addEventListener( 'resize', this.onWindowResize, false );
      
    window.addEventListener("keydown", this.onKeyDown, false);
    window.addEventListener("mousedown", this.onClick, false);
    window.addEventListener("keyup", this.onKeyUp, false);
    window.addEventListener("touchstart", this.onTouchStart, false)
    window.addEventListener("touchmove", this.onTouchMove, false)
    window.addEventListener("touchend", this.onTouchEnd, false)
  }
  onWindowResize = () => {
    this.camera.aspect = this.props.width / this.props.height;
    this.camera.updateProjectionMatrix();
    this.width = this.props.width
    this.height = this.props.height
    this.renderer.setSize( this.props.width, this.props.height );
    
    this.setSmallMode();
}

  setSmallMode = () => {
    if(window.innerWidth < 1200) {
      this.setState({
        smallMode: true
      });
    } else {
      this.setState({
        smallMode: false
      });
    }
  }

  onTouchStart = (event) => {
    this.touchIndex = 0;
    for(let i = 0; i < Array.from(event.touches).length; i++)
    {
      this.currentTouchX[i] = event.touches[i].clientX
    this.currentTouchY[i] = event.touches[i].clientY
    if(this.state.smallMode)
    {
      const element = document.getElementById("uad");
      const element2 = document.getElementById("ma");
      const clientRect = element.getBoundingClientRect();
      const clientRectMove = element2.getBoundingClientRect();
      const thingyX = clientRect.left;
      const thingyY = clientRect.top;
      const thingyWidth = clientRect.width;
      const thingyHeight = clientRect.height;


      const thingyX2 = clientRectMove.left;
      const thingyY2 = clientRectMove.top;
      const thingyWidth2 = clientRectMove.width;
      const thingyHeight2 = clientRectMove.height;
      if(this.currentTouchX[i] >= thingyX && this.currentTouchX[i] <= thingyX+thingyWidth
        && this.currentTouchY[i] >= thingyY && this.currentTouchY[i] <= thingyY+thingyHeight)
        {
          this.thingyDown = true;
          this.input.ActiveState.jump = true;
          this.input.ActiveState.isGrounded = false;
        } 
        if(this.currentTouchX[i] >= thingyX2 && this.currentTouchX[i] <= thingyX2+thingyWidth2
          && this.currentTouchY[i] >= thingyY2 && this.currentTouchY[i] <= thingyY2+thingyHeight2)
          {

            
      this.touchIndex += 1;
            this.moveyDown = true;
          } 
    }
    }

  }

  onTouchMove = (event) => {
    //event.preventDefault();

    // if(event.touches[0].clientX !== this.currentTouchX)
    // {
    //   let differenceX = (event.touches[0].clientX - this.currentTouchX);
    //   //this.neck.rotation.y -= differenceX/20;

    //   this.currentTouchX = event.touches[0].clientX;
    // }

    if(this.moveyDown)
    {
      event.preventDefault();
            this.input.jump = true;
      this.input.isGrounded = false;
    if(event.touches[0].clientY !== this.currentTouchY[0] || event.touches[0].clientX !== this.currentTouchX[0])
    {
      let differenceX = (event.touches[0].clientX - this.currentTouchX[0]);
      let differenceY = -(event.touches[0].clientY - this.currentTouchY[0]);
      if(differenceX > 0)
      {
        this.input.ActiveState.right = true;
      }
      if(differenceX < 0)
      {
        this.input.ActiveState.left = true;
      }
      if(differenceY > 0)
      {
        this.input.ActiveState.forward = true;
      }
      if(differenceY < 0)
      {
        this.input.ActiveState.back = true;
      }

    }
  }
  }

  onTouchEnd = (event) => {
    this.input.ActiveState.right = false;
    
    this.input.ActiveState.left = false;
    
    this.input.ActiveState.forward = false;
    
    this.input.ActiveState.back = false;
    const element = document.getElementById("uad");
    const element2 = document.getElementById("ma");
    if(element !== null && element2 !== null) {
    const clientRect = element.getBoundingClientRect();
    const clientRectMove = element2.getBoundingClientRect();
    const thingyX = clientRect.left;
    const thingyY = clientRect.top;
    const thingyWidth = clientRect.width;
    const thingyHeight = clientRect.height;


    const thingyX2 = clientRectMove.left;
    const thingyY2 = clientRectMove.top;
    const thingyWidth2 = clientRectMove.width;
    const thingyHeight2 = clientRectMove.height;

      delete this.currentTouchX[this.touchIndex];
      delete this.currentTouchY[this.touchIndex];
    if(this.currentTouchX[this.touchIndex] >= thingyX && this.currentTouchX[this.touchIndex] <= thingyX+thingyWidth
      && this.currentTouchY[this.touchIndex] >= thingyY && this.currentTouchY[this.touchIndex] <= thingyY+thingyHeight)
      {
        this.thingyDown = false;
      } 
      if(this.currentTouchX[this.touchIndex] >= thingyX2 && this.currentTouchX[this.touchIndex] <= thingyX2+thingyWidth2
        && this.currentTouchY[this.touchIndex] >= thingyY2 && this.currentTouchY[this.touchIndex] <= thingyY2+thingyHeight2)
        {
          this.moveyDown = false;
        }
      }


  }
  onKeyDown = (event) => {
    switch (event.code) {
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
        if(document.activeElement !== document.getElementById("inp"))
        {
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
      // case "Space":
      //   this.input.ActiveState.up = false;
      //   break;
      case "ShiftLeft":
        this.input.ActiveState.crouch = false;
        break;
    }
  };
  componentDidMount = () => {
    this.setSmallMode();
    this.scene.fog = new THREE.Fog(0x000000, 20, 160)
    const gltfLoader = new GLTFLoader();
    const loadAsync = url => {
        return new Promise(resolve => {
          gltfLoader.load(url, gltf => {
            resolve(gltf)
          })
        })
      }
    Promise.all([loadAsync("/player/scene.gltf")]).then(
        models => {
            saturn = models[0].scene.children[0];
            saturn.scale.set(2,2,2);

    this.props.socket.on("chat", (theChat) => {
      let key = generateUUID();
      this.setState(prevState => {
        return {chats: [...prevState.chats, { key,
                                              id: theChat.id, 
                                              message: theChat.message }] }
      })
      window.setTimeout(()=>{
        this.setState({chats: this.state.chats.filter((chat) => { 
          return chat.key !== key

      })});
      console.log("DOINGTHIS")}, 2000);
    });
            
    this.props.socket.on("playerUpdate", (data) => {

        const key = data.id;
          const playerState = data;
          //console.log(data);
          if(this.allPlayerModels.has(key)) {
            let model = this.allPlayerModels.get(key);
            //console.log("Model", model);

            model.rotation.z = playerState.zrotation;
             this.playerOldAndNewPositions.set(key, new PrevAndNewPosition(
                 model.position,
                 new THREE.Vector3(data.x, data.y-1.6, data.z)
             ));
            // model.position.x = data.x;
            //   model.position.y = data.y-1.6; //odd positioning problem on the model I'm using atm, this will be gone!
            //   model.position.z = data.z;

            this.allPlayerModels.set(key, model);

          } else {

              const newPlayer = saturn.clone();
              this.scene.add(newPlayer);
              this.allPlayerModels.set(data.id, newPlayer);
              newPlayer.position.x = data.x;
              newPlayer.position.y = data.y-1.6; //odd positioning problem on the model I'm using atm, this will be gone!
              newPlayer.position.z = data.z;

          }

      });

      this.allBlockActionsRef.on("child_added", snapshot => {
        const addedAct = snapshot.val();
        switch(addedAct.action)
        {
          case "break":
            this.breakBlock({ x: addedAct.x, y: addedAct.y, z: addedAct.z});
            break;
          case "place":
            if(addedAct.type === "light")
            {
              this.addPointLight(addedAct.x, addedAct.y, addedAct.z);
            }
            else{

              this.placeBlock({ x: addedAct.x, y: addedAct.y, z: addedAct.z}, addedAct.type || "1");
            }
            break;
          default:
            break;
        }
      })
  
      // this.allPlayersRef.on("child_added", (snapshot) => {
        
        
      // });

      this.props.socket.on("playerDisconnect", (id) => {
        if(this.allPlayerModels.has(id)){
            this.scene.remove(this.allPlayerModels.get(id))
            this.allPlayerModels.delete(id);
        }
      });

        }
    );


    this.mountListeners();
    const chunk_width = this.chunk_width;
    class World {
      constructor() {
        this.data = new LinkedHashMap();
        this.hasblockmarks = new LinkedHashMap();
        this.fullblockmarks = new LinkedHashMap();
        this.ishandledmarks = new LinkedHashMap();
      }
      // load = (string) => {

      //       const text = LZString.decompress(string);
      //       console.log(text);
      //       const json = JSON.parse(text);
            
      //       //const map = new Map(Object.entries(json));
      //       //this.data = new Map([...map, ...this.data]);
      // }
      generateOneChunk(A, B, C)
      {

        let blockCount = 0;
              for (var o = 0+(A*chunk_width); o < chunk_width+(A*chunk_width); o++) {
                for (var o2 = 0+(B*chunk_width); o2 < chunk_width+(B*chunk_width); o2++) {
                  for (var o3 = 0+(C*chunk_width); o3 < chunk_width+(C*chunk_width); o3++) {
                    let REAL_WORLD_X =  o;
                    let REAL_WORLD_Z = o3;
                    let REAL_WORLD_Y =o2;

                    let n =
                      ImprovedNoise.noise(
                        REAL_WORLD_X / 25.34,
                        34.425,
                        REAL_WORLD_Z / 25.65
                      ) * 15;

                      if (
                        !this.ishandledmarks.has("" + A + "," + B + "," + C)
                      ) {
                        this.ishandledmarks.set("" + A + "," + B + "," + C, "1")
                      }

                    if (REAL_WORLD_Y < n) {
                      blockCount++;
                      if (!this.hasblockmarks.has("" + A + "," + B + "," + C)) {
                        this.hasblockmarks.set("" +A + "," + B + "," + C, "1"); //Chunk level (zoomed out)
                      }
                      if (
                        blockCount >=
                        chunk_width * chunk_width * chunk_width
                      ) {
                        this.fullblockmarks.set(
                          "" + A + "," + B + "," + C,
                          "1"
                        ); // Remove it if its full for now
                      }
                      
                      let idToSet = (Math.abs(REAL_WORLD_Y - n)<2 ? ((REAL_WORLD_Y < 1) ? "3" : "4") : "1");
                      this.data.set(
                        "" +
                          REAL_WORLD_X +
                          "," +
                          REAL_WORLD_Y +
                          "," +
                          REAL_WORLD_Z,
                        idToSet
                      );  // Real this.world level (micro)
                      // try {
                      //   const docRef = addDoc(collection(db, "blocks"), {
                      //     x: REAL_WORLD_X,
                      //     y: REAL_WORLD_Y,
                      //     z: REAL_WORLD_Z,
                      //     id: 1
                      //   });
        
                      // } catch (e) {
                      //   console.error("Error adding document: ", e);
                      // }
                    }
                  }
                }
              }
        if(blockCount === 0){
          return "air";
        } else {
          return "go";
        }
      }
      generate = () => {
        let REAL_WORLD_X;
        let REAL_WORLD_Y;
        let REAL_WORLD_Z;

        for (let j = -2; j < 2; j++) {
          for (let i = -5; i < 5; i++) {
            for (let k = -5; k < 5; k++) {
              let blockCount = 0;
              for (var o = 0; o < chunk_width; o++) {
                for (var o2 = 0; o2 < chunk_width; o2++) {
                  for (var o3 = 0; o3 < chunk_width; o3++) {
                    REAL_WORLD_X = i * chunk_width + o;
                    REAL_WORLD_Z = k * chunk_width + o3;
                    REAL_WORLD_Y = j * chunk_width + o2;

                    let n =
                      ImprovedNoise.noise(
                        REAL_WORLD_X / 25.34,
                        34.425,
                        REAL_WORLD_Z / 25.65
                      ) * 15;

                      if (
                        !this.ishandledmarks.has("" + i + "," + j + "," + k)
                      ) {
                        this.ishandledmarks.set("" + i + "," + j + "," + k, "1")
                      }
                    if (REAL_WORLD_Y < n) {
                      blockCount++;
                      if (!this.hasblockmarks.has("" + i + "," + j + "," + k)) {
                        this.hasblockmarks.set("" + i + "," + j + "," + k, "1"); //Chunk level (zoomed out)
                      }
                      if (
                        blockCount >=
                        chunk_width * chunk_width * chunk_width
                      ) {
                        this.fullblockmarks.set(
                          "" + i + "," + j + "," + k,
                          "1"
                        ); // Remove it if its full for now
                      }
                      let idToSet = (Math.abs(REAL_WORLD_Y - n)<2 ? ((REAL_WORLD_Y < 1) ? "3" : "4") : "1");
                      this.data.set(
                        "" +
                          REAL_WORLD_X +
                          "," +
                          REAL_WORLD_Y +
                          "," +
                          REAL_WORLD_Z,
                        idToSet
                      ); // Real this.world level (micro)
                      // try {
                      //   const docRef = addDoc(collection(db, "blocks"), {
                      //     x: REAL_WORLD_X,
                      //     y: REAL_WORLD_Y,
                      //     z: REAL_WORLD_Z,
                      //     id: 1
                      //   });
        
                      // } catch (e) {
                      //   console.error("Error adding document: ", e);
                      // }
                    }
                  }
                }
              }
            }
          }
        }
        //console.log("Done generating this.world.");
      };
    }

    this.world = new World();

    this.camera.position.z = 0;
    this.camera.position.y = 0;
    this.camera.position.x = 0;
    

    this.scene.add(this.camera);


    // Lights
    const ambientLight = new THREE.AmbientLight(0xffffff, 0.2);
    ambientLight.position.set(0, 0, 0);
    this.scene.add(ambientLight);

    const pointLight = new THREE.DirectionalLight(0xffffff, 0.5);
    pointLight.position.set(0, 50, 0);
    this.scene.add(pointLight);
    this.canvas = document.getElementById("canvas");

    this.renderer = new THREE.WebGLRenderer({
      canvas: this.canvas,
      antialias: false,
    });

    this.renderer.setSize(this.width, this.height);
    this.renderer.setClearColor(0x000000, 1);


    this.controls = new PointerLockControls2(this.camera, this.canvas);
    //MAIN STUFF

    this.world.generate();
   // const docRef = doc(db, "blobs", "1");
    // try {
    //   const docSnap = getDoc(docRef);
    //   docSnap.then(result =>
    //     {
    //       console.log("Result", result);
    //       let x = result._document.data.value.mapValue.fields.data.stringValue;
    //       //console.log(x);
    //       //this.world.load(""+x);
    //     }).catch(error => {console.log(error)});
      
    //   } catch(error) {
    //       console.log(error)
    //   }
 
    // let gms = new GiantMapSaver(this.world.data);
    // let deconstructedWorld = gms.deconstruct();
    // for(let i = 0; i < deconstructedWorld.length; ++i)
    // {
    //   let s = JSON.stringify(deconstructedWorld[i]);
    //   //console.log(s);
    //   let u8arr = LZString.compressToUint8Array(s);
    //   let kson = JSON.stringify(u8arr);
    //   try {
    //     setDoc(doc(db, "blobs", `${i}`), {
    //       data:kson
    //     });


    //     } catch (e) {
    //       console.error("Error adding document: ", e);
    //     }
    // }

    this.populateChunkPool();
    

    this.runGameLoop(this.input);

    this.props.resize();
    this.onWindowResize();
  }

  addPointLight(x, y, z) 
  {
    const pointLight = new THREE.DirectionalLight(0xffffff, 1);
    pointLight.position.set(x, y , z);
    this.scene.add(pointLight);
  }

  populateChunkPool() {
    const chunk_width = this.chunk_width;
    const world = this.world;
    const clock = this.clock

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
      buildmesh(newx, newy, newz) {
        this.timeLastMeshed = Date.now();
        this.x = newx;
        this.z = newz;
        this.y = newy;
        let newVerts = [];
        let newNorms = [];
        let newUVs = [];

        const add4UVs = (id, side) =>
        {
          const values = blockTypes[id].texture.uniform ? 
          blockTypes[id].texture.all : blockTypes[id].texture[side];


          newUVs.push(values[0], values[1]); 
          newUVs.push(values[2], values[3]);
          newUVs.push(values[4], values[5]);
          newUVs.push(values[4], values[5]);
          newUVs.push(values[6], values[7]);
          newUVs.push(values[0], values[1]);
          

        }

        const addVert = (x,y,z) => {


            newVerts.push(x,y,z);
            return 1;

        }
        if (
          world.fullblockmarks.has("" + this.x + "," + this.y + "," + this.z)
        ) {
          if (
            !world.fullblockmarks.has(
              "" + (this.x - 1) + "," + this.y + "," + this.z
            ) ||
            !world.hasblockmarks.has(
              "" + (this.x - 1) + "," + this.y + "," + this.z
            )
          ) {
            //left
            let v = 0;
            v += addVert(0, 0, 0);          
            v += addVert(0, 0, chunk_width);
            v += addVert(0, chunk_width, chunk_width);
            v += addVert(0, chunk_width, chunk_width);
            v += addVert(0, chunk_width, 0);
            v += addVert(0, 0, 0);

            add4UVs("1", "all");
            for (let h = 0; h < v; h++) {
              newNorms.push(-1, 0, 0);

            }
          }
          if (
            !world.fullblockmarks.has(
              "" + this.x + "," + this.y + "," + (this.z - 1)
            ) ||
            !world.hasblockmarks.has(
              "" + (this.x - 1) + "," + this.y + "," + this.z
            )
          ) {
            //front
            let v = 0;
            v += addVert(0, 0, 0);
            v += addVert(0, chunk_width, 0);
            v += addVert(chunk_width, chunk_width, 0);
            v += addVert(chunk_width, chunk_width, 0);
            v += addVert(chunk_width, 0, 0);
            v += addVert(0, 0, 0);
            add4UVs("1", "all");
            for (let h = 0; h < v; h++) {
              newNorms.push(0, 0, -1);
            }
          }
          if (
            !world.fullblockmarks.has(
              "" + (this.x + 1) + "," + this.y + "," + this.z
            ) ||
            !world.hasblockmarks.has(
              "" + (this.x - 1) + "," + this.y + "," + this.z
            )
          ) {
            //right
            let v = 0;
            v += addVert(chunk_width, 0, 0);
            v += addVert(chunk_width, chunk_width, 0);
            v += addVert(chunk_width, chunk_width, chunk_width);
            v += addVert(chunk_width, chunk_width, chunk_width);
            v += addVert(chunk_width, 0, chunk_width);
            v += addVert(chunk_width, 0, 0);add4UVs("1", "all");
            for (let h = 0; h < v; h++) {
              newNorms.push(1, 0, 0);
            }
          }
          if (
            !world.fullblockmarks.has(
              "" + this.x + "," + this.y + "," + this.z + 1
            ) ||
            !world.hasblockmarks.has(
              "" + (this.x - 1) + "," + this.y + "," + this.z
            )
          ) {
            //back
            let v = 0;
            v += addVert(chunk_width, 0, chunk_width);
            v += addVert(chunk_width, chunk_width, chunk_width);
            v += addVert(0, chunk_width, chunk_width);
            v += addVert(0, chunk_width, chunk_width);
            v += addVert(0, 0, chunk_width);
            v += addVert(chunk_width, 0, chunk_width);add4UVs("1", "all");
            for (let h = 0; h < v; h++) {
              newNorms.push(0, 0, 1);
            }
          }
          if (
            !world.fullblockmarks.has(
              "" + this.x + "," + (this.y - 1) + "," + this.z
            ) ||
            !world.hasblockmarks.has(
              "" + (this.x - 1) + "," + this.y + "," + this.z
            )
          ) {
            //bottom
            let v = 0;
            v += addVert(0, 0, chunk_width);
            v += addVert(0, 0, 0);
            v += addVert(chunk_width, 0, 0);
            v += addVert(chunk_width, 0, 0);
            v += addVert(chunk_width, 0, chunk_width);
            v += addVert(0, 0, chunk_width);add4UVs("1", "all");
            for (let h = 0; h < v; h++) {
              newNorms.push(0, -1, 0);
            }
          }
          if (
            !world.fullblockmarks.has(
              "" + this.x + "," + (this.y + 1) + "," + this.z
            ) ||
            !world.hasblockmarks.has(
              "" + (this.x - 1) + "," + this.y + "," + this.z
            )
          ) {
            //top
            let v = 0;
            v += addVert(0, chunk_width, 0);
            v += addVert(0, chunk_width, chunk_width);
            v += addVert(chunk_width, chunk_width, chunk_width);
            v += addVert(chunk_width, chunk_width, chunk_width);
            v += addVert(chunk_width, chunk_width, 0);
            v += addVert(0, chunk_width, 0);add4UVs("1", "all");
            for (let h = 0; h < v; h++) {
              newNorms.push(0, 1, 0);
            }
          }
        } else {
          for (let j = 0; j < chunk_width; j++) {
            for (let i = 0; i < chunk_width; i++) {
              for (let k = 0; k < chunk_width; k++) {
                if (
                  world.data.has(
                    "" +
                      (this.x * chunk_width + i) +
                      "," +
                      (this.y * chunk_width + j) +
                      "," +
                      (this.z * chunk_width + k)
                  )
                ) {
                  const ID = world.data.get(
                    "" +
                      (this.x * chunk_width + i) +
                      "," +
                      (this.y * chunk_width + j) +
                      "," +
                      (this.z * chunk_width + k)
                  )
                  if (
                    !world.data.has(
                      "" +
                        (this.x * chunk_width + i - 1) +
                        "," +
                        (this.y * chunk_width + j) +
                        "," +
                        (this.z * chunk_width + k)
                    )
                  ) {
                    let v = 0;
                    v += addVert(i, j, k);
                    v += addVert(i, j, k + 1);
                    v += addVert(i, j + 1, k + 1);
                    v += addVert(i, j + 1, k + 1);
                    v += addVert(i, j + 1, k);
                    v += addVert(i, j, k);add4UVs(ID, "sides");
                    for (let h = 0; h < v; h++) {
                      newNorms.push(-1, 0, 0);
                    }
                  }
                  if (
                    !world.data.has(
                      "" +
                        (this.x * chunk_width + i) +
                        "," +
                        (this.y * chunk_width + j) +
                        "," +
                        (this.z * chunk_width + k - 1)
                    )
                  ) {
                    let v = 0;
                    v += addVert(i+1, j, k);
                    v += addVert(i, j, k);
                    v += addVert(i, j + 1, k);
                    v += addVert(i, j + 1, k);
                    v += addVert(i + 1, j+1, k);
                    v += addVert(i+1, j, k);         add4UVs(ID, "sides");
                    for (let h = 0; h < v; h++) {
                      newNorms.push(0, 0, -1);
                    }
                  }
                  if (
                    !world.data.has(
                      "" +
                        (this.x * chunk_width + i + 1) +
                        "," +
                        (this.y * chunk_width + j) +
                        "," +
                        (this.z * chunk_width + k)
                    )
                  ) {
                    let v = 0;
                    v += addVert(i + 1, j, k + 1);
                    v += addVert(i + 1, j, k)
      
                    v += addVert(i + 1, j + 1, k);
                    v += addVert(i + 1, j+1, k);
                    v += addVert(i + 1, j + 1, k + 1);
                    v += addVert(i + 1, j, k + 1);add4UVs(ID, "sides");
                    for (let h = 0; h < v; h++) {
                      newNorms.push(1, 0, 0);
                    }
                  }
                  if (
                    !world.data.has(
                      "" +
                        (this.x * chunk_width + i) +
                        "," +
                        (this.y * chunk_width + j) +
                        "," +
                        (this.z * chunk_width + k + 1)
                    )
                  ) {
                    let v = 0;
                    v += addVert(i, j, k + 1);
                    v += addVert(i + 1, j, k + 1);
                    v += addVert(i + 1, j + 1, k + 1);
                    v += addVert(i + 1, j + 1, k + 1);
                    v += addVert(i, j + 1, k + 1);
                    v += addVert(i, j, k + 1);add4UVs(ID, "sides");
                    for (let h = 0; h < v; h++) {
                      newNorms.push(0, 0, 1);
                    }
                  }
                  if (
                    !world.data.has(
                      "" +
                        (this.x * chunk_width + i) +
                        "," +
                        (this.y * chunk_width + j - 1) +
                        "," +
                        (this.z * chunk_width + k)
                    )
                  ) {
                    let v = 0;
                    v += addVert(i, j, k);
                    v += addVert(i + 1, j, k);
                    v += addVert(i + 1, j, k + 1);
                    v += addVert(i + 1, j, k + 1);
                    v += addVert(i, j, k + 1);
                    v += addVert(i, j, k);add4UVs(ID, "bottom");
                    for (let h = 0; h < v; h++) {
                      newNorms.push(0, -1, 0);
                    }
                  }
                  if (
                    !world.data.has(
                      "" +
                        (this.x * chunk_width + i) +
                        "," +
                        (this.y * chunk_width + j + 1) +
                        "," +
                        (this.z * chunk_width + k)
                    )
                  ) {
                    let v = 0;
                    v += addVert(i, j + 1, k);
                    v += addVert(i, j + 1, k + 1);
                    v += addVert(i + 1, j + 1, k + 1);
                    v += addVert(i + 1, j + 1, k + 1);
                    v += addVert(i + 1, j + 1, k);
                    v += addVert(i, j + 1, k);add4UVs(ID, "top");
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
          "normal",
          new THREE.BufferAttribute(new Float32Array(newNorms), 3)
        );
        this.meshGeometry.setAttribute(
          "uv",
          new THREE.BufferAttribute(new Float32Array(newUVs), 2)
        )
        this.mesh.position.set(this.x * chunk_width, this.y * chunk_width, this.z * chunk_width);
        this.mesh.geometry = this.meshGeometry;
      }
      buildmeshinplace() {
        
        let newVerts = [];
        let newNorms = [];
        
        let newUVs = [];
        const add4UVs = (id, side) =>
        {
          const values = blockTypes[id].texture.uniform ? 
          blockTypes[id].texture.all : blockTypes[id].texture[side];


          newUVs.push(values[0], values[1]); 
          newUVs.push(values[2], values[3]);
          newUVs.push(values[4], values[5]);
          newUVs.push(values[4], values[5]);
          newUVs.push(values[6], values[7]);
          newUVs.push(values[0], values[1]);
          

        }
        const addVert = (x,y,z) => {


          newVerts.push(x,y,z);
          return 1;

      }

        if (
          world.fullblockmarks.has("" + this.x + "," + this.y + "," + this.z)
        ) {
          if (
            !world.fullblockmarks.has(
              "" + (this.x - 1) + "," + this.y + "," + this.z
            ) ||
            !world.hasblockmarks.has(
              "" + (this.x - 1) + "," + this.y + "," + this.z
            )
          ) {
            //left
            let v = 0;
            v += addVert(0, 0, 0);
            v += addVert(0, 0, chunk_width);
            v += addVert(0, chunk_width, chunk_width);
            v += addVert(0, chunk_width, chunk_width);
            v += addVert(0, chunk_width, 0);
            v += addVert(0, 0, 0);add4UVs("1", "all");
            for (let h = 0; h < v; h++) {
              newNorms.push(-1, 0, 0);
            }
          }
          if (
            !world.fullblockmarks.has(
              "" + this.x + "," + this.y + "," + (this.z - 1)
            ) ||
            !world.hasblockmarks.has(
              "" + (this.x - 1) + "," + this.y + "," + this.z
            )
          ) {
            //front
            let v = 0;
            v += addVert(0, 0, 0);
            v += addVert(0, chunk_width, 0);
            v += addVert(chunk_width, chunk_width, 0);
            v += addVert(chunk_width, chunk_width, 0);
            v += addVert(chunk_width, 0, 0);
            v += addVert(0, 0, 0);add4UVs("1", "all");
            for (let h = 0; h < v; h++) {
              newNorms.push(0, 0, -1);
            }
          }
          if (
            !world.fullblockmarks.has(
              "" + (this.x + 1) + "," + this.y + "," + this.z
            ) ||
            !world.hasblockmarks.has(
              "" + (this.x - 1) + "," + this.y + "," + this.z
            )
          ) {
            //right
            let v = 0;
            v += addVert(chunk_width, 0, 0);
            v += addVert(chunk_width, chunk_width, 0);
            v += addVert(chunk_width, chunk_width, chunk_width);
            v += addVert(chunk_width, chunk_width, chunk_width);
            v += addVert(chunk_width, 0, chunk_width);
            v += addVert(chunk_width, 0, 0);add4UVs("1", "all");
            for (let h = 0; h < v; h++) {
              newNorms.push(1, 0, 0);
            }
          }
          if (
            !world.fullblockmarks.has(
              "" + this.x + "," + this.y + "," + this.z + 1
            ) ||
            !world.hasblockmarks.has(
              "" + (this.x - 1) + "," + this.y + "," + this.z
            )
          ) {
            //back
            let v = 0;
            v += addVert(chunk_width, 0, chunk_width);
            v += addVert(chunk_width, chunk_width, chunk_width);
            v += addVert(0, chunk_width, chunk_width);
            v += addVert(0, chunk_width, chunk_width);
            v += addVert(0, 0, chunk_width);
            v += addVert(chunk_width, 0, chunk_width);add4UVs("1", "all");
            for (let h = 0; h < v; h++) {
              newNorms.push(0, 0, 1);
            }
          }
          if (
            !world.fullblockmarks.has(
              "" + this.x + "," + (this.y - 1) + "," + this.z
            ) ||
            !world.hasblockmarks.has(
              "" + (this.x - 1) + "," + this.y + "," + this.z
            )
          ) {
            //bottom
            let v = 0;
            v += addVert(0, 0, chunk_width);
            v += addVert(0, 0, 0);
            v += addVert(chunk_width, 0, 0);
            v += addVert(chunk_width, 0, 0);
            v += addVert(chunk_width, 0, chunk_width);
            v += addVert(0, 0, chunk_width);add4UVs("1", "all");
            for (let h = 0; h < v; h++) {
              newNorms.push(0, -1, 0);
            }
          }
          if (
            !world.fullblockmarks.has(
              "" + this.x + "," + (this.y + 1) + "," + this.z
            ) ||
            !world.hasblockmarks.has(
              "" + (this.x - 1) + "," + this.y + "," + this.z
            )
          ) {
            //top
            let v = 0;
            v += addVert(0, chunk_width, 0);
            v += addVert(0, chunk_width, chunk_width);
            v += addVert(chunk_width, chunk_width, chunk_width);
            v += addVert(chunk_width, chunk_width, chunk_width);
            v += addVert(chunk_width, chunk_width, 0);
            v += addVert(0, chunk_width, 0);add4UVs("1", "all");
            for (let h = 0; h < v; h++) {
              newNorms.push(0, 1, 0);
            }
          }
        } else {
          for (let j = 0; j < chunk_width; j++) {
            for (let i = 0; i < chunk_width; i++) {
              for (let k = 0; k < chunk_width; k++) {
                if (
                  world.data.has(
                    "" +
                      (this.x * chunk_width + i) +
                      "," +
                      (this.y * chunk_width + j) +
                      "," +
                      (this.z * chunk_width + k)
                  )
                ) {
                  const ID = world.data.get(
                    "" +
                      (this.x * chunk_width + i) +
                      "," +
                      (this.y * chunk_width + j) +
                      "," +
                      (this.z * chunk_width + k)
                  );
                  if (
                    !world.data.has(
                      "" +
                        (this.x * chunk_width + i - 1) +
                        "," +
                        (this.y * chunk_width + j) +
                        "," +
                        (this.z * chunk_width + k)
                    )
                  ) {
                    let v = 0;
                    v += addVert(i, j, k);
                    v += addVert(i, j, k + 1);
                    v += addVert(i, j + 1, k + 1);
                    v += addVert(i, j + 1, k + 1);
                    v += addVert(i, j + 1, k);
                    v += addVert(i, j, k);add4UVs(ID, "sides");
                    for (let h = 0; h < v; h++) {
                      newNorms.push(-1, 0, 0);
                    }
                  }
                  if (
                    !world.data.has(
                      "" +
                        (this.x * chunk_width + i) +
                        "," +
                        (this.y * chunk_width + j) +
                        "," +
                        (this.z * chunk_width + k - 1)
                    )
                  ) {
                    let v = 0;
                    v += addVert(i+1, j, k);
                    v += addVert(i, j, k);
                    v += addVert(i, j + 1, k);
                    v += addVert(i, j + 1, k);
                    v += addVert(i + 1, j+1, k);
                    v += addVert(i+1, j, k);  add4UVs(ID, "sides");
                    for (let h = 0; h < v; h++) {
                      newNorms.push(0, 0, -1);
                    }
                  }
                  if (
                    !world.data.has(
                      "" +
                        (this.x * chunk_width + i + 1) +
                        "," +
                        (this.y * chunk_width + j) +
                        "," +
                        (this.z * chunk_width + k)
                    )
                  ) {
                    let v = 0;
                    v += addVert(i + 1, j, k + 1);
                    v += addVert(i + 1, j, k)
      
                    v += addVert(i + 1, j + 1, k);
                    v += addVert(i + 1, j+1, k);
                    v += addVert(i + 1, j + 1, k + 1);
                    v += addVert(i + 1, j, k + 1);
                   ;add4UVs(ID, "sides");
                    for (let h = 0; h < v; h++) {
                      newNorms.push(1, 0, 0);
                    }
                  }
                  if (
                    !world.data.has(
                      "" +
                        (this.x * chunk_width + i) +
                        "," +
                        (this.y * chunk_width + j) +
                        "," +
                        (this.z * chunk_width + k + 1)
                    )
                  ) {
                    let v = 0;
                    v += addVert(i, j, k + 1);
                    v += addVert(i + 1, j, k + 1);
                    v += addVert(i + 1, j + 1, k + 1);
                    v += addVert(i + 1, j + 1, k + 1);
                    v += addVert(i, j + 1, k + 1);
                    v += addVert(i, j, k + 1);add4UVs(ID, "sides");
                    for (let h = 0; h < v; h++) {
                      newNorms.push(0, 0, 1);
                    }
                  }
                  if (
                    !world.data.has(
                      "" +
                        (this.x * chunk_width + i) +
                        "," +
                        (this.y * chunk_width + j - 1) +
                        "," +
                        (this.z * chunk_width + k)
                    )
                  ) {
                    let v = 0;
                    v += addVert(i, j, k);
                    v += addVert(i + 1, j, k);
                    v += addVert(i + 1, j, k + 1);
                    v += addVert(i + 1, j, k + 1);
                    v += addVert(i, j, k + 1);
                    v += addVert(i, j, k);add4UVs(ID, "bottom");
                    for (let h = 0; h < v; h++) {
                      newNorms.push(0, -1, 0);
                    }
                  }
                  if (
                    !world.data.has(
                      "" +
                        (this.x * chunk_width + i) +
                        "," +
                        (this.y * chunk_width + j + 1) +
                        "," +
                        (this.z * chunk_width + k)
                    )
                  ) {
                    let v = 0;
                    v += addVert(i, j + 1, k);
                    v += addVert(i, j + 1, k + 1);
                    v += addVert(i + 1, j + 1, k + 1);
                    v += addVert(i + 1, j + 1, k + 1);
                    v += addVert(i + 1, j + 1, k);
                    v += addVert(i, j + 1, k);add4UVs(ID, "top");
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
          "normal",
          new THREE.BufferAttribute(new Float32Array(newNorms), 3)
        );
        this.meshGeometry.setAttribute(
          "uv",
          new THREE.BufferAttribute(new Float32Array(newUVs), 2)
        )
      
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
   
      if(!this.mappedChunks.has(`${neededSpot.x},${neededSpot.y},${neededSpot.z}`)) {

        let cp = this.chunkpool.filter(c=>{return Date.now()-c.timeLastMeshed > 12000});
        
        cp.sort(
          (a,b)=> 
          {

            return (this.camera.position.distanceTo(new THREE.Vector3(a.x, a.y, a.z))
            <
            this.camera.position.distanceTo(new THREE.Vector3(b.x, b.y, b.z))
            ) ? -1 : 1;
            
          }
        );

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
        this.scene.remove(grabbedMesh.mesh);
        grabbedMesh.buildmesh(neededSpot.x, neededSpot.y, neededSpot.z);
        this.scene.add(grabbedMesh.mesh);
        this.mappedChunks.set(
          "" + neededSpot.x + "," + neededSpot.y + "," + neededSpot.z,
          grabbedMesh
        );
        this.neededChunks.delete(
          "" + neededSpot.x + "," + neededSpot.y + "," + neededSpot.z
        );
      }
    } else {
      //console.log("Mapped chunks already has", neededSpot.x, neededSpot.y, neededSpot.z)
      this.mappedChunks.get(`${neededSpot.x},${neededSpot.y},${neededSpot.z}`).buildmeshinplace();
      this.neededChunks.delete(`${neededSpot.x},${neededSpot.y},${neededSpot.z}`)
    }
    } else {
      this.chunkQueueTimer++;
    }
  }

  surveyNeededChunks() {
    if (this.camera !== null && this.camera !== undefined) {
      let y = -this.chunk_width * 2
        for (
          let i = -this.chunk_width * 5;
          i < this.chunk_width * 5;
          i += this.chunk_width
        ) {
          for (
            let k = -this.chunk_width * 5;
            k < this.chunk_width * 5;
            k += this.chunk_width
          ) {
            let THERIGHTX = this.camera.position.x - (this.camera.position.x%this.chunk_width);
            let THERIGHTY = this.camera.position.y -  (this.camera.position.y%this.chunk_width);
            let THERIGHTZ = this.camera.position.z - (this.camera.position.z%this.chunk_width);
            let x = Math.round((i + THERIGHTX) / this.chunk_width);
            let z = Math.round((k + THERIGHTZ) / this.chunk_width);
            let yy = Math.round((y + THERIGHTY) /  this.chunk_width);
            if(this.world.ishandledmarks.has("" + x + "," + yy + "," + z)) {
            if (
              this.world.hasblockmarks.has("" + x + "," + yy + "," + z) &&
              !this.mappedChunks.has("" + x + "," + yy + "," + z)
            ) {
              let obj = { x: x, y: yy, z: z };

              if (!this.neededChunks.has("" + x + "," + yy + "," + z)) {
                // if it needs to tell neededchunks it needs this
                this.neededChunks.set("" + x + "," + yy + "," + z, obj);
                let h = 1;
     
                while(this.world.hasblockmarks.has("" + x + "," + (yy+h) + "," + z)) {
                  this.neededChunks.set("" + x + "," + (yy+h) + "," + z,
                  {x, y: yy+h, z});
                  h+=1;
                }
              }
            }

          } else {
            let h = 0;
            let cont = true;
            while(!this.world.ishandledmarks.has("" + x + "," + (yy+h) + "," + z) && h < 4 && cont) {
              const resrej = (resolve, reject) => {
                try{
                const res = this.world.generateOneChunk(x, yy+h, z);
                cont = (res !== "air")
                resolve(null);
                }catch(e)
                {
                  reject(e);
                }
              }
            let prom = new Promise(
              resrej
            );
            prom.then(result=>{
              //yay
            }).catch(error=>{
              console.log(error);
            });
            h +=1;

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
      distanceTraveled += .1;
  
      const key = `${Math.floor(currentX)},${Math.floor(currentY)},${Math.floor(currentZ)}`;
      if (this.world.data.has(key)) {
        return key;
      }
    }
  
    return null;
  }

  sendChat = (event) => {
    event.preventDefault();
    const key = generateUUID();
    const chat = {
      id: this.props.pid,
      message: this.state.chat,
      key
    }
    this.props.socket.emit('chat', chat);
    this.setState(prevState => {
      return {chats: [...prevState.chats, chat] }
    })
    window.setTimeout(()=>{
      this.setState({chats: this.state.chats.filter((chat) => { 
        return chat.key !== key

    })});}, 10000)
    this.setState({
      chat: ""
    });
    this.chatBoxRef.current.blur();
  }


  runGameLoop(input) {

    let myPrevGO  = {
      x:0,
      y:0,
      z:0,
      zrotation:0
    };
    const collisionDistance = 0.1;
    const animate = () => {
      
      this.input.ActiveState.isGrounded = (!this.isReady || this.castRayBlocking(this.camera.position.x,
        this.camera.position.y-1,
        this.camera.position.z, 0, -1, 0, collisionDistance) !== null)
        if(!this.input.ActiveState.isGrounded)
        {
          this.input.ActiveState.jumpTimer += this.delt*12;
          this.camera.position.y += ((6) - this.input.ActiveState.jumpTimer)*this.delt;
        }
      //console.log(this.mappedChunks.size );
        if(this.mappedChunks.size < 125)
        {
          this.props.handle()("messageToClient")("loadingworld");
        } else {
          this.props.handle()("messageToClient")("none");
          this.isReady = true;
        }
        if(this.updatePlayersTimer > this.updatePlayersInterval) {
            this.updatePlayersTimer = 0;
            if(this.controls.camera.rotation.y !== myPrevGO.zrotation
            || this.controls.camera.position.x !== myPrevGO.x
            || this.controls.camera.position.y !== myPrevGO.y
            || this.controls.camera.position.z !== myPrevGO.z) 
            {
                let upd = {
                    id: this.props.pid,
                    zrotation: this.controls.camera.rotation.y,
                    x: this.camera.position.x,
                    y: this.camera.position.y,
                    z: this.camera.position.z,
                };
                this.props.socket.emit('playerUpdate', upd);
              myPrevGO = 
              {
                ...upd
              }
            }
          }
        else {
            this.delt = this.clock.getDelta()
            this.updatePlayersTimer += this.delt;
            for(const [key, value] of this.allPlayerModels.entries()) {
                if(this.playerOldAndNewPositions.has(key)) {
                let model = value;
                model.position.lerpVectors(this.playerOldAndNewPositions.get(key).prevPosition, this.playerOldAndNewPositions.get(key).newPosition,this.updatePlayersTimer);
                this.allPlayerModels.set(key, model);
                }
              }

        }
      if(this.isReady) {
      if (input.ActiveState.forward) {
        let dir = this.getCameraDirection();
        dir.y = 0;
        if(this.castRayBlocking(this.camera.position.x,
          this.camera.position.y, this.camera.position.z,
          dir.x,
          dir.y,
          dir.z,
          collisionDistance) === null
          &&
          this.castRayBlocking(this.camera.position.x,
            this.camera.position.y-1, this.camera.position.z,
            dir.x,
            dir.y,
            dir.z,
            collisionDistance) === null)
          {
          this.controls.moveForward(this.delt*6);
          }
        
      }
      if (input.ActiveState.back) {
        let dir = this.getCameraDirection();
        dir.y = 0;
        if(this.castRayBlocking(this.camera.position.x,
          this.camera.position.y, this.camera.position.z,
          -dir.x,
          -dir.y,
          -dir.z,
          collisionDistance) === null
          &&
          this.castRayBlocking(this.camera.position.x,
            this.camera.position.y-1, this.camera.position.z,
            -dir.x,
            -dir.y,
            -dir.z,
            collisionDistance) === null)
          {
          this.controls.moveForward(-this.delt*6);
          }
      }
      if (input.ActiveState.right) {
       let dir = this.getCameraDirectionRight();
        dir.y = 0;
        if(this.castRayBlocking(this.camera.position.x,
          this.camera.position.y, this.camera.position.z,
          dir.x,
          dir.y,
          dir.z,
          collisionDistance) === null
          &&
          this.castRayBlocking(this.camera.position.x,
            this.camera.position.y-1, this.camera.position.z,
            dir.x,
            dir.y,
            dir.z,
            collisionDistance) === null)
          {
          this.controls.moveRight(this.delt*6);
          }
        
      }
      if (input.ActiveState.left) {
        let dir = this.getCameraDirectionRight();
        dir.y = 0;
        if(this.castRayBlocking(this.camera.position.x,
          this.camera.position.y, this.camera.position.z,
          -dir.x,
          -dir.y,
          -dir.z,
          collisionDistance) === null
          &&
          this.castRayBlocking(this.camera.position.x,
            this.camera.position.y-1, this.camera.position.z,
            -dir.x,
            -dir.y,
            -dir.z,
            collisionDistance) === null)
          {
          this.controls.moveRight(-this.delt*6);
          }
        
      }
      if (input.ActiveState.jump) {
        if(!this.input.ActiveState.isGrounded || this.input.ActiveState.jumpTimer < 0.1) {

          this.camera.position.y += ((6) - this.input.ActiveState.jumpTimer)*this.delt;
        }
        else{
          this.input.ActiveState.jumpTimer = 0;
          this.input.ActiveState.jump = false;
        }
      }
      if (input.ActiveState.down) {
        this.camera.position.y -= this.delt*8;
        
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
