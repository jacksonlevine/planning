import React, { Component } from "react";
import "firebase/app";
import firebase from "firebase/compat/app";
import "firebase/compat/database";
import * as THREE from "three";
import { PointerLockControls } from "three/examples/jsm/controls/PointerLockControls.js";
import { GLTFLoader } from "three/examples/jsm/loaders/GLTFLoader.js";
import ImprovedNoise from "./../perlin.js";
import GiantMapSaver from "../giantMapSaver.js";
import LZString from "lz-string/libs/lz-string.js";
import { generateUUID } from "three/src/math/MathUtils.js";


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
    this.isGrounded = false;
  }
}

class InputHandler {
  constructor() {
    this.ActiveState = new InputState();
  }
}

const meshMaterial = new THREE.MeshLambertMaterial({
  color: 0xff0000,
  depthWrite: true,
});

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
    this.chunk_width = 12;
    this.scene = new THREE.Scene();
    this.world = null;
    this.surveyNeededChunksTimer = 0;
    this.surveyNeededChunksInterval = 20;
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
    this.currentPushedActions = [];
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

  castRay = (x, y, z, dx, dy, dz, maxDistance) => {
    return new Promise((resolve, reject) => {
      let distanceTraveled = 0;
      let currentX = x;
      let currentY = y;
      let currentZ = z;
  
      while (distanceTraveled < maxDistance) {
        currentX += dx;
        currentY += dy;
        currentZ += dz;
        distanceTraveled += .05;
  
        const key = `${Math.floor(currentX)},${Math.floor(currentY)},${Math.floor(currentZ)}`;
        if (this.world.data.has(key)) {
          resolve(
            {
              x: Math.floor(currentX),
              y: Math.floor(currentY),
              z: Math.floor(currentZ)
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

  lockControls = () => {
    if(!this.controls.isLocked)
    {
      this.controls.lock();
    }
    else {
      var vector = this.getCameraDirection();

      this.castRay(this.camera.position.x, 
        this.camera.position.y, 
        this.camera.position.z, vector.x,
        vector.y, vector.z, 10)
        .then((pos) => {
          this.breakBlock(pos);
          let actionRef = firebase.database().ref(`blockActions/${generateUUID()}`);
          actionRef.set(
            {
              action: "break",
              time: new Date().getTime(),
              ...pos
            });
          this.currentPushedActions.push(actionRef);
        })
        .catch((error) => {
        });
    }
  };

  breakBlock(pos)
  {
    const blockKey = `${pos.x},${pos.y},${pos.z}`; 
          this.world.data.delete(blockKey);
          const chunkX = Math.floor(pos.x/this.chunk_width);
          const chunkY = Math.floor(pos.y/this.chunk_width);
          const chunkZ = Math.floor(pos.z/this.chunk_width);
          if(this.world.fullblockmarks.has(`${chunkX},${chunkY},${chunkZ}`)){
            this.world.fullblockmarks.delete(`${chunkX},${chunkY},${chunkZ}`);
          }
          if(this.mappedChunks.has(`${chunkX},${chunkY},${chunkZ}`))
          {
            this.mappedChunks.get(`${chunkX},${chunkY},${chunkZ}`)
            .buildmeshinplace();

            //6 surrounders
            this.mappedChunks.get(`${chunkX-1},${chunkY},${chunkZ}`)
            .buildmeshinplace();
            this.mappedChunks.get(`${chunkX+1},${chunkY},${chunkZ}`)
            .buildmeshinplace();
            this.mappedChunks.get(`${chunkX},${chunkY-1},${chunkZ}`)
            .buildmeshinplace();
            this.mappedChunks.get(`${chunkX},${chunkY+1},${chunkZ}`)
            .buildmeshinplace();
            this.mappedChunks.get(`${chunkX},${chunkY},${chunkZ-1}`)
            .buildmeshinplace();
            this.mappedChunks.get(`${chunkX},${chunkY},${chunkZ+1}`)
            .buildmeshinplace();
          }
  }

  componentWillUnmount() {
    window.removeEventListener("click", this.lockControls);
    window.removeEventListener("keydown", this.onKeyDown, false);

    window.removeEventListener("keyup", this.onKeyUp, false);
  }

  mountListeners() {
    window.addEventListener("click", this.lockControls, false);

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

            
    this.allPlayersRef.on("value", (snapshot) => {
        this.players = snapshot.val() || {};
        Object.keys(this.players).forEach((key) => {
          const playerState = this.players[key];
          if(this.allPlayerModels.has(key)) {
            let model = this.allPlayerModels.get(key);
            //console.log("Model", model);

            model.rotation.z = playerState.zrotation;
            this.playerOldAndNewPositions.set(key, new PrevAndNewPosition(
                model.position,
                new THREE.Vector3(playerState.x, playerState.y-1.6, playerState.z)
            ));


            this.allPlayerModels.set(key, model);

          }
        });
      });

      this.allBlockActionsRef.on("child_added", snapshot => {
        const addedAct = snapshot.val();
        switch(addedAct.action)
        {
          case "break":
            this.breakBlock({ x: addedAct.x, y: addedAct.y, z: addedAct.z});
            break;
          default:
            break;
        }
      })
  
      this.allPlayersRef.on("child_added", (snapshot) => {
        const addedPlayer = snapshot.val();
        if(!this.allPlayerModels.has(addedPlayer.id) && addedPlayer.id !== this.props.pid)
        {
          const newPlayer = saturn.clone();
          this.scene.add(newPlayer);
          this.allPlayerModels.set(addedPlayer.id, newPlayer);
          newPlayer.position.x = addedPlayer.x;
          newPlayer.position.y = addedPlayer.y-1.6; //odd positioning problem on the model I'm using atm, this will be gone!
          newPlayer.position.z = addedPlayer.z;
        }
        
      });

      this.allPlayersRef.on("child_removed", (oldChildSnapshot) => {
        if(this.allPlayerModels.has(oldChildSnapshot.key)){
            this.scene.remove(this.allPlayerModels.get(oldChildSnapshot.key))
        }
      });

        }
    );


    this.mountListeners();
    const chunk_width = this.chunk_width;
        
    class World {
      constructor() {
        this.data = new Map();
        this.hasblockmarks = new Map();
        this.fullblockmarks = new Map();
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
                    }
                  }
                }
              }
            }
          }
        }
        console.log("Done generating this.world.");
      };
    }

    this.world = new World();

    this.camera.position.z = 4;
    this.camera.position.y += 20;
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

    let gms = new GiantMapSaver(this.world.data);
    let deconstructedWorld = gms.deconstruct();
    for(let i = 0; i < deconstructedWorld.length; ++i)
    {
      let jsonData = LZString.compress(JSON.stringify(deconstructedWorld[i]));


    }

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
        this.y = 0;
      }
      buildmesh(newx, newy, newz) {
        this.x = newx;
        this.z = newz;
        this.y = newy;
        let newVerts = [];
        let newNorms = [];
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
            newVerts.push(0, 0, 0);
            newVerts.push(0, 0, chunk_width);
            newVerts.push(0, chunk_width, chunk_width);
            newVerts.push(0, chunk_width, chunk_width);
            newVerts.push(0, chunk_width, 0);
            newVerts.push(0, 0, 0);
            for (let h = 0; h < 6; h++) {
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
            newVerts.push(0, 0, 0);
            newVerts.push(0, chunk_width, 0);
            newVerts.push(chunk_width, chunk_width, 0);
            newVerts.push(chunk_width, chunk_width, 0);
            newVerts.push(chunk_width, 0, 0);
            newVerts.push(0, 0, 0);
            for (let h = 0; h < 6; h++) {
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
            newVerts.push(chunk_width, 0, 0);
            newVerts.push(chunk_width, chunk_width, 0);
            newVerts.push(chunk_width, chunk_width, chunk_width);
            newVerts.push(chunk_width, chunk_width, chunk_width);
            newVerts.push(chunk_width, 0, chunk_width);
            newVerts.push(chunk_width, 0, 0);
            for (let h = 0; h < 6; h++) {
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
            newVerts.push(chunk_width, 0, chunk_width);
            newVerts.push(chunk_width, chunk_width, chunk_width);
            newVerts.push(0, chunk_width, chunk_width);
            newVerts.push(0, chunk_width, chunk_width);
            newVerts.push(0, 0, chunk_width);
            newVerts.push(chunk_width, 0, chunk_width);
            for (let h = 0; h < 6; h++) {
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
            newVerts.push(0, 0, chunk_width);
            newVerts.push(0, 0, 0);
            newVerts.push(chunk_width, 0, 0);
            newVerts.push(chunk_width, 0, 0);
            newVerts.push(chunk_width, 0, chunk_width);
            newVerts.push(0, 0, chunk_width);
            for (let h = 0; h < 6; h++) {
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
            newVerts.push(0, chunk_width, 0);
            newVerts.push(0, chunk_width, chunk_width);
            newVerts.push(chunk_width, chunk_width, chunk_width);
            newVerts.push(chunk_width, chunk_width, chunk_width);
            newVerts.push(chunk_width, chunk_width, 0);
            newVerts.push(0, chunk_width, 0);
            for (let h = 0; h < 6; h++) {
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
                    newVerts.push(i, j, k);
                    newVerts.push(i, j, k + 1);
                    newVerts.push(i, j + 1, k + 1);
                    newVerts.push(i, j + 1, k + 1);
                    newVerts.push(i, j + 1, k);
                    newVerts.push(i, j, k);
                    for (let h = 0; h < 6; h++) {
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
                    newVerts.push(i, j, k);
                    newVerts.push(i, j + 1, k);
                    newVerts.push(i + 1, j + 1, k);
                    newVerts.push(i + 1, j + 1, k);
                    newVerts.push(i + 1, j, k);
                    newVerts.push(i, j, k);
                    for (let h = 0; h < 6; h++) {
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
                    newVerts.push(i + 1, j, k);
                    newVerts.push(i + 1, j + 1, k);
                    newVerts.push(i + 1, j + 1, k + 1);
                    newVerts.push(i + 1, j + 1, k + 1);
                    newVerts.push(i + 1, j, k + 1);
                    newVerts.push(i + 1, j, k);
                    for (let h = 0; h < 6; h++) {
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
                    newVerts.push(i, j, k + 1);
                    newVerts.push(i + 1, j, k + 1);
                    newVerts.push(i + 1, j + 1, k + 1);
                    newVerts.push(i + 1, j + 1, k + 1);
                    newVerts.push(i, j + 1, k + 1);
                    newVerts.push(i, j, k + 1);
                    for (let h = 0; h < 6; h++) {
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
                    newVerts.push(i, j, k);
                    newVerts.push(i + 1, j, k);
                    newVerts.push(i + 1, j, k + 1);
                    newVerts.push(i + 1, j, k + 1);
                    newVerts.push(i, j, k + 1);
                    newVerts.push(i, j, k);
                    for (let h = 0; h < 6; h++) {
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
                    newVerts.push(i, j + 1, k);
                    newVerts.push(i, j + 1, k + 1);
                    newVerts.push(i + 1, j + 1, k + 1);
                    newVerts.push(i + 1, j + 1, k + 1);
                    newVerts.push(i + 1, j + 1, k);
                    newVerts.push(i, j + 1, k);
                    for (let h = 0; h < 6; h++) {
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
        this.mesh.position.set(this.x * chunk_width, this.y * chunk_width, this.z * chunk_width);

        this.mesh.geometry = this.meshGeometry;
      }
      buildmeshinplace() {
        let newVerts = [];
        let newNorms = [];
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
            newVerts.push(0, 0, 0);
            newVerts.push(0, 0, chunk_width);
            newVerts.push(0, chunk_width, chunk_width);
            newVerts.push(0, chunk_width, chunk_width);
            newVerts.push(0, chunk_width, 0);
            newVerts.push(0, 0, 0);
            for (let h = 0; h < 6; h++) {
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
            newVerts.push(0, 0, 0);
            newVerts.push(0, chunk_width, 0);
            newVerts.push(chunk_width, chunk_width, 0);
            newVerts.push(chunk_width, chunk_width, 0);
            newVerts.push(chunk_width, 0, 0);
            newVerts.push(0, 0, 0);
            for (let h = 0; h < 6; h++) {
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
            newVerts.push(chunk_width, 0, 0);
            newVerts.push(chunk_width, chunk_width, 0);
            newVerts.push(chunk_width, chunk_width, chunk_width);
            newVerts.push(chunk_width, chunk_width, chunk_width);
            newVerts.push(chunk_width, 0, chunk_width);
            newVerts.push(chunk_width, 0, 0);
            for (let h = 0; h < 6; h++) {
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
            newVerts.push(chunk_width, 0, chunk_width);
            newVerts.push(chunk_width, chunk_width, chunk_width);
            newVerts.push(0, chunk_width, chunk_width);
            newVerts.push(0, chunk_width, chunk_width);
            newVerts.push(0, 0, chunk_width);
            newVerts.push(chunk_width, 0, chunk_width);
            for (let h = 0; h < 6; h++) {
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
            newVerts.push(0, 0, chunk_width);
            newVerts.push(0, 0, 0);
            newVerts.push(chunk_width, 0, 0);
            newVerts.push(chunk_width, 0, 0);
            newVerts.push(chunk_width, 0, chunk_width);
            newVerts.push(0, 0, chunk_width);
            for (let h = 0; h < 6; h++) {
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
            newVerts.push(0, chunk_width, 0);
            newVerts.push(0, chunk_width, chunk_width);
            newVerts.push(chunk_width, chunk_width, chunk_width);
            newVerts.push(chunk_width, chunk_width, chunk_width);
            newVerts.push(chunk_width, chunk_width, 0);
            newVerts.push(0, chunk_width, 0);
            for (let h = 0; h < 6; h++) {
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
                    newVerts.push(i, j, k);
                    newVerts.push(i, j, k + 1);
                    newVerts.push(i, j + 1, k + 1);
                    newVerts.push(i, j + 1, k + 1);
                    newVerts.push(i, j + 1, k);
                    newVerts.push(i, j, k);
                    for (let h = 0; h < 6; h++) {
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
                    newVerts.push(i, j, k);
                    newVerts.push(i, j + 1, k);
                    newVerts.push(i + 1, j + 1, k);
                    newVerts.push(i + 1, j + 1, k);
                    newVerts.push(i + 1, j, k);
                    newVerts.push(i, j, k);
                    for (let h = 0; h < 6; h++) {
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
                    newVerts.push(i + 1, j, k);
                    newVerts.push(i + 1, j + 1, k);
                    newVerts.push(i + 1, j + 1, k + 1);
                    newVerts.push(i + 1, j + 1, k + 1);
                    newVerts.push(i + 1, j, k + 1);
                    newVerts.push(i + 1, j, k);
                    for (let h = 0; h < 6; h++) {
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
                    newVerts.push(i, j, k + 1);
                    newVerts.push(i + 1, j, k + 1);
                    newVerts.push(i + 1, j + 1, k + 1);
                    newVerts.push(i + 1, j + 1, k + 1);
                    newVerts.push(i, j + 1, k + 1);
                    newVerts.push(i, j, k + 1);
                    for (let h = 0; h < 6; h++) {
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
                    newVerts.push(i, j, k);
                    newVerts.push(i + 1, j, k);
                    newVerts.push(i + 1, j, k + 1);
                    newVerts.push(i + 1, j, k + 1);
                    newVerts.push(i, j, k + 1);
                    newVerts.push(i, j, k);
                    for (let h = 0; h < 6; h++) {
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
                    newVerts.push(i, j + 1, k);
                    newVerts.push(i, j + 1, k + 1);
                    newVerts.push(i + 1, j + 1, k + 1);
                    newVerts.push(i + 1, j + 1, k + 1);
                    newVerts.push(i + 1, j + 1, k);
                    newVerts.push(i, j + 1, k);
                    for (let h = 0; h < 6; h++) {
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
      

        this.mesh.geometry = this.meshGeometry;
      }
    }
    for (let i = 0; i < 16; i++) {
      for (let k = 0; k < 16; k++) {
        for (let a = 0; a < 16; a++) {
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
      if (grabbedMesh != null) {
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
      for (let y = -this.chunk_width * 2; y < this.chunk_width * 8; y += 16) {
        for (
          let i = -this.chunk_width * 10;
          i < this.chunk_width * 10;
          i += 16
        ) {
          for (
            let k = -this.chunk_width * 10;
            k < this.chunk_width * 10;
            k += 16
          ) {
            let x = Math.round((i + this.camera.position.x) / this.chunk_width);
            let z = Math.round((k + this.camera.position.z) / this.chunk_width);
            let yy = Math.round((y + this.camera.position.y) /  this.chunk_width);

            if (
              this.world.hasblockmarks.has("" + x + "," + yy + "," + z) &&
              !this.mappedChunks.has("" + x + "," + yy + "," + z)
            ) {
              let obj = { x: x, y: yy, z: z };

              if (!this.neededChunks.has("" + x + "," + yy + "," + z)) {
                // if it needs to tell neededchunks it needs this
                this.neededChunks.set("" + x + "," + yy + "," + z, obj);
              }
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
    const collisionDistance = 0.1;
    const animate = () => {
      this.input.ActiveState.isGrounded = (this.castRayBlocking(this.camera.position.x,
        this.camera.position.y-1,
        this.camera.position.z, 0, -1, 0, collisionDistance) !== null)
        if(!this.input.ActiveState.isGrounded)
        {
          this.input.ActiveState.jumpTimer += this.delt*12;
          this.camera.position.y += ((6) - this.input.ActiveState.jumpTimer)*this.delt;
        }
      //console.log(this.mappedChunks.size );
        if(this.mappedChunks.size < 200)
        {
          this.props.handle()("messageToClient")("loadingworld");
        } else {
          this.props.handle()("messageToClient")("none");
          this.isReady = true;
        }
        if(this.updatePlayersTimer > this.updatePlayersInterval) {
            this.updatePlayersTimer = 0;
            this.props.pref.set({
                ...this.players[this.props.pid],
                zrotation: this.camera.rotation.z,
                x: this.camera.position.x,
                y: this.camera.position.y,
                z: this.camera.position.z,
            }
                );
        }
        else {
            this.delt = this.clock.getDelta()
            this.updatePlayersTimer += parseFloat(this.delt);

            Object.keys(this.players).forEach((key)=> {
                if(this.allPlayerModels.has(key) && this.playerOldAndNewPositions.has(key)) {
                let model = this.allPlayerModels.get(key);
                model.position.lerpVectors(this.playerOldAndNewPositions.get(key).prevPosition, this.playerOldAndNewPositions.get(key).newPosition,this.updatePlayersTimer);
                this.allPlayerModels.set(key, model);
                }
            });
        }
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
      if (this.surveyNeededChunksTimer >= this.surveyNeededChunksInterval) {
        this.surveyNeededChunksTimer = 0;
        this.surveyNeededChunks();
        let d = new Date();
        this.currentPushedActions.forEach(action => {
          if(d.getTime() - parseInt(action.time) > 3000)
          {
            action.remove();
          }
        });
      } else {
        this.surveyNeededChunksTimer += 1;
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
