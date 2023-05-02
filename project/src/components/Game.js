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

export default class Game extends Component {
  constructor() {
    super();
    this.allPlayerModels = new Map();
    this.allPlayersRef = firebase.database().ref("players");
    this.allBlockActionsRef = firebase.database().ref("blockActions");
    
    this.width = 1024;
    this.height = 640;
    this.fov = 100;
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
    this.chunk_width = 10;
    this.scene = new THREE.Scene();
    this.world = null;
    this.surveyNeededChunksTimer = 0;
    this.surveyNeededChunksInterval = 1;
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
  }

  render() {

    //console.log(this.props.pid)
    return (
      <div style={this.styles}>
        <canvas id="canvas"></canvas>
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
          vector.y, vector.z, 10, 0)
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
        vector.y, vector.z, 10, .3)
        .then((pos) => {
          this.placeBlock(pos, 1);
          let actionRef = firebase.database().ref(`blockActions/${generateUUID()}`);
          actionRef.set(
            {
              action: "place",
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
    window.removeEventListener("mousedown", this.onClick);
    window.removeEventListener("keydown", this.onKeyDown, false);

    window.removeEventListener("keyup", this.onKeyUp, false);
  }

  mountListeners() {
    window.addEventListener("mousedown", this.onClick, false);

    window.addEventListener("keydown", this.onKeyDown, false);

    window.addEventListener("keyup", this.onKeyUp, false);
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
  componentDidMount() {
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
            this.placeBlock({ x: addedAct.x, y: addedAct.y, z: addedAct.z}, 1);
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
                      
                      this.data.set(
                        "" +
                          REAL_WORLD_X +
                          "," +
                          REAL_WORLD_Y +
                          "," +
                          REAL_WORLD_Z,
                        "1"
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
                      
                      this.data.set(
                        "" +
                          REAL_WORLD_X +
                          "," +
                          REAL_WORLD_Y +
                          "," +
                          REAL_WORLD_Z,
                        "1"
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

    this.camera.position.z = 4;
    this.camera.position.y += 4;
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


    this.controls = new PointerLockControls(this.camera, this.canvas);

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
  }

  populateChunkPool() {
    const chunk_width = this.chunk_width;
    const world = this.world;
    class Chunk {
      constructor() {
        this.meshGeometry = new THREE.BufferGeometry();
        this.vertices = new Float32Array(3);
        this.mesh = new THREE.Mesh(this.meshGeometry, meshMaterial);
        this.x = 0; //multiply x and z by 16 to get real-this.world position
        this.z = 0;
        this.y = -500;
      }
      buildmesh(newx, newy, newz) {
        this.x = newx;
        this.z = newz;
        this.y = newy;
        let newVerts = [];
        let newNorms = [];
        let newUVs = [];

        const add4UVs = () =>
        {
          newUVs.push(0.0+.0625, 1.0-.0625);
          newUVs.push(0.0, 1.0-.0625);
          newUVs.push(0.0 ,      1.0);
          newUVs.push(0.0 ,      1.0);
          newUVs.push(0.0+.0625, 1.0);
          newUVs.push(0.0+.0625, 1.0-.0625);
          

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

            add4UVs();
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
            add4UVs();
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
            v += addVert(chunk_width, 0, 0);add4UVs();
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
            v += addVert(chunk_width, 0, chunk_width);add4UVs();
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
            v += addVert(0, 0, chunk_width);add4UVs();
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
            v += addVert(0, chunk_width, 0);add4UVs();
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
                    v += addVert(i, j, k);add4UVs();
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
                    v += addVert(i, j, k);
                    v += addVert(i, j + 1, k);
                    v += addVert(i + 1, j + 1, k);
                    v += addVert(i + 1, j + 1, k);
                    v += addVert(i + 1, j, k);
                    v += addVert(i, j, k);add4UVs();
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
                    v += addVert(i + 1, j, k);
                    v += addVert(i + 1, j + 1, k);
                    v += addVert(i + 1, j + 1, k + 1);
                    v += addVert(i + 1, j + 1, k + 1);
                    v += addVert(i + 1, j, k + 1);
                    v += addVert(i + 1, j, k);add4UVs();
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
                    v += addVert(i, j, k + 1);add4UVs();
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
                    v += addVert(i, j, k);add4UVs();
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
                    v += addVert(i, j + 1, k);add4UVs();
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
        const add4UVs = () =>
        {
          newUVs.push(0.0+.0625, 1.0-.0625);
          newUVs.push(0.0, 1.0-.0625);
          newUVs.push(0.0 ,      1.0);
          newUVs.push(0.0 ,      1.0);
          newUVs.push(0.0+.0625, 1.0);
          newUVs.push(0.0+.0625, 1.0-.0625);
          

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
            v += addVert(0, 0, 0);add4UVs();
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
            v += addVert(0, 0, 0);add4UVs();
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
            v += addVert(chunk_width, 0, 0);add4UVs();
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
            v += addVert(chunk_width, 0, chunk_width);add4UVs();
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
            v += addVert(0, 0, chunk_width);add4UVs();
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
            v += addVert(0, chunk_width, 0);add4UVs();
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
                    v += addVert(i, j, k);add4UVs();
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
                    v += addVert(i, j, k);
                    v += addVert(i, j + 1, k);
                    v += addVert(i + 1, j + 1, k);
                    v += addVert(i + 1, j + 1, k);
                    v += addVert(i + 1, j, k);
                    v += addVert(i, j, k);add4UVs();
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
                    v += addVert(i + 1, j, k);
                    v += addVert(i + 1, j + 1, k);
                    v += addVert(i + 1, j + 1, k + 1);
                    v += addVert(i + 1, j + 1, k + 1);
                    v += addVert(i + 1, j, k + 1);
                    v += addVert(i + 1, j, k);add4UVs();
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
                    v += addVert(i, j, k + 1);add4UVs();
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
                    v += addVert(i, j, k);add4UVs();
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
                    v += addVert(i, j + 1, k);add4UVs();
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
    for (let i = 0; i < 9; i++) {
      for (let k = 0; k < 9; k++) {
        for (let a = 0; a < 9; a++) {
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

      const neededSpot = needed[0];

      let grabbedMesh = this.chunkpool.pop();
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
        this.chunkpool.unshift(grabbedMesh);
        this.mappedChunks.set(
          "" + neededSpot.x + "," + neededSpot.y + "," + neededSpot.z,
          grabbedMesh
        );
        this.neededChunks.delete(
          "" + neededSpot.x + "," + neededSpot.y + "," + neededSpot.z
        );
      }
    } else {
      this.chunkQueueTimer++;
    }
  }

  surveyNeededChunks() {
    if (this.camera !== null && this.camera !== undefined) {
      let y = -this.chunk_width * 2
        for (
          let i = -this.chunk_width * 4;
          i < this.chunk_width * 4;
          i += this.chunk_width
        ) {
          for (
            let k = -this.chunk_width * 4;
            k < this.chunk_width * 4;
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
            while(!this.world.ishandledmarks.has("" + x + "," + (yy+h) + "," + z) && h < 6 && cont) {
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
          this.controls.moveForward(this.delt*8);
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
          this.controls.moveForward(-this.delt*8);
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
          this.controls.moveRight(this.delt*8);
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
          this.controls.moveRight(-this.delt*8);
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
      if (this.surveyNeededChunksTimer >= this.surveyNeededChunksInterval) {
        this.surveyNeededChunksTimer = 0;
        this.surveyNeededChunks();

      } else {
        this.surveyNeededChunksTimer += this.delt;
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
