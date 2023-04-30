import React, { Component } from "react";
import "firebase/app";
import firebase from "firebase/compat/app";
import "firebase/compat/database";

import * as THREE from "three";
import { PointerLockControls } from "three/examples/jsm/controls/PointerLockControls";
import ImprovedNoise from "./../perlin.js";

class InputState {
    constructor() {
      this.left = false;
      this.right = false;
      this.back = false;
      this.forward = false;
      this.up = false;
      this.down = false;
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

export default class Game extends Component {
  constructor() {
    super();
    this.allPlayersRef = firebase.database().ref("players");

    this.allPlayersRef.on("value", (snapshot) => {
      //Fires whenever a change occurs
    });

    this.allPlayersRef.on("child_added", (snapshot) => {
      const addedPlayer = snapshot.val();
      
    });
    this.camera = new THREE.PerspectiveCamera(
      this.fov,
      window.innerWidth / window.innerHeight,
      1,
      1000
    );
    this.isOpen = true;
    this.styles = {};
    this.canvas = null;
    this.controls = null;
    this.chunk_width = 16;
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
  }

  render() {
    return (
      <div style={this.styles}>
        <h1>Hello I am a Game</h1>
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

  lockControls = () => {
    this.controls.lock();
  };

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
        this.input.ActiveState.up = true;
        break;
      case "ShiftLeft":
        this.input.ActiveState.down = true;
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
      case "Space":
        this.input.ActiveState.up = false;
        break;
      case "ShiftLeft":
        this.input.ActiveState.down = false;
        break;
    }
  };
  componentDidMount() {
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

    this.renderer.setSize(500, 500);
    this.renderer.setClearColor(0x000000, 1);
    this.clock = new THREE.Clock();

    this.controls = new PointerLockControls(this.camera, this.canvas);

    //MAIN STUFF

    this.world.generate();

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
                      (this.x * 16 + i) +
                      "," +
                      (this.y * 16 + j) +
                      "," +
                      (this.z * 16 + k)
                  )
                ) {
                  if (
                    !world.data.has(
                      "" +
                        (this.x * 16 + i - 1) +
                        "," +
                        (this.y * 16 + j) +
                        "," +
                        (this.z * 16 + k)
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
                        (this.x * 16 + i) +
                        "," +
                        (this.y * 16 + j) +
                        "," +
                        (this.z * 16 + k - 1)
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
                        (this.x * 16 + i + 1) +
                        "," +
                        (this.y * 16 + j) +
                        "," +
                        (this.z * 16 + k)
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
                        (this.x * 16 + i) +
                        "," +
                        (this.y * 16 + j) +
                        "," +
                        (this.z * 16 + k + 1)
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
                        (this.x * 16 + i) +
                        "," +
                        (this.y * 16 + j - 1) +
                        "," +
                        (this.z * 16 + k)
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
                        (this.x * 16 + i) +
                        "," +
                        (this.y * 16 + j + 1) +
                        "," +
                        (this.z * 16 + k)
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
        this.mesh.position.set(this.x * 16, this.y * 16, this.z * 16);

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
          true
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
            let x = Math.round((i + this.camera.position.x) / 16);
            let z = Math.round((k + this.camera.position.z) / 16);
            let yy = Math.round((y + this.camera.position.y) / 16);

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

  runGameLoop(input) {
    const animate = () => {
      if (input.ActiveState.forward) {
        this.controls.moveForward(0.25);
      }
      if (input.ActiveState.back) {
        this.controls.moveForward(-0.25);
      }
      if (input.ActiveState.right) {
        this.controls.moveRight(0.25);
      }
      if (input.ActiveState.left) {
        this.controls.moveRight(-0.25);
      }
      if (input.ActiveState.up) {
        this.camera.position.y += 0.2;
      }
      if (input.ActiveState.down) {
        this.camera.position.y -= 0.2;
      }
      if (this.surveyNeededChunksTimer >= this.surveyNeededChunksInterval) {
        this.surveyNeededChunksTimer = 0;
        this.surveyNeededChunks();
      } else {
        this.surveyNeededChunksTimer += 1;
      }

      this.rebuildQueuedMeshes();
      this.renderer.render(this.scene, this.camera);
      if(this.isOpen)
      {
        window.requestAnimationFrame(animate);
      }

    };
    if(this.isOpen){
        animate();
    }
  }
}
