/* eslint-disable camelcase */
import { Camera, Euler, EventDispatcher, Vector3 } from 'three';
import * as THREE from "three";
const _changeEvent = { type: 'change' };
const _lockEvent = { type: 'lock' };
const _unlockEvent = { type: 'unlock' };

const _PI_2 = Math.PI / 2;

class PointerLockControls2 extends EventDispatcher {
  domElement;
  camera;
  isLocked = false;

  minPolarAngle = 0; // radians
  maxPolarAngle = Math.PI; // radians

  vector = new Vector3();
  euler = new THREE.Euler(0, 0, 0, 'YXZ');

  previousTouch;

  onMouseMoveBind = this.onMouseMove.bind(this);
  onPointerlockChangeBind = this.onPointerlockChange.bind(this);
  onPointerlockErrorBind = this.onPointerlockError.bind(this);
  onTouchMoveBind = this.onTouchMove.bind(this);
  onTouchEndBind = this.onTouchEnd.bind(this);

  constructor(camera, domElement) {
    super();

    if (domElement === undefined) {
      console.warn(
        'THREE.PointerLockControls: The second parameter "domElement" is now mandatory.'
      );
      domElement = document.body;
    }

    this.camera = camera;
    this.domElement = domElement;
    
    this.mobileMoverDown = false;

    this.connect();
  }

  onTouchMove(e, override) {
    if(override === true) return;
    let touch;

    switch (e.touches.length) {
      case 1:
        if (e.touches[0].target === this.domElement) touch = e.touches[0];
        break;
      case 2:
        if (e.touches[0].target === this.domElement) touch = e.touches[0];
        else if (e.touches[1].target === this.domElement) touch = e.touches[1];
        break;
    }

    if (!touch) return;

    //console.log(touch.target);

    const movementX = this.previousTouch
      ? touch.pageX - this.previousTouch.pageX
      : 0;
    let movementY = this.mobileMoverDown ? 0 : (this.previousTouch
      ? touch.pageY - this.previousTouch.pageY
      : 0);

    this.updatePosition(movementX, movementY, 0.004);

    this.previousTouch = touch;
  }
  onTouchMoveSingle(touch) {
    if (!touch) return;

    //console.log(touch.target);

    const movementX = this.previousTouch
      ? touch.pageX - this.previousTouch.pageX
      : 0;
    let movementY = this.mobileMoverDown ? 0 : (this.previousTouch
      ? touch.pageY - this.previousTouch.pageY
      : 0);

    this.updatePosition(movementX, movementY, 0.004);

    this.previousTouch = touch;
  }

  onTouchEnd() {
    this.previousTouch = undefined;
  }

  onMouseMove(event) {
    if (this.isLocked === false) return;

    const movementX =
      // eslint-disable-next-line @typescript-eslint/ban-ts-comment
      // @ts-ignore
      event.movementX || event.mozMovementX || event.webkitMovementX || 0;
    const movementY =
      // eslint-disable-next-line @typescript-eslint/ban-ts-comment
      // @ts-ignore
      event.movementY || event.mozMovementY || event.webkitMovementY || 0;

    this.updatePosition(movementX, movementY, 0.002);
  }

  updatePosition(movementX, movementY, multiplier) {
    this.euler.setFromQuaternion(this.camera.quaternion);

    this.euler.y -= movementX * multiplier;
    this.euler.x -= movementY * multiplier;

    this.euler.x = Math.max(
      _PI_2 - this.maxPolarAngle,
      Math.min(_PI_2 - this.minPolarAngle, this.euler.x)
    );

    this.camera.quaternion.setFromEuler(this.euler);

    this.dispatchEvent(_changeEvent);
  }

  onPointerlockChange() {
    if (this.domElement.ownerDocument.pointerLockElement === this.domElement) {
      this.dispatchEvent(_lockEvent);

      this.isLocked = true;
    } else {
      this.dispatchEvent(_unlockEvent);

      this.isLocked = false;
    }
  }

  onPointerlockError() {
  }

  connect() {
    // this.domElement.addEventListener('touchmove', this.onTouchMoveBind, false);
     this.domElement.addEventListener('touchend', this.onTouchEndBind, false);
    this.domElement.ownerDocument.addEventListener(
      'mousemove',
      this.onMouseMoveBind
    );
    this.domElement.ownerDocument.addEventListener(
      'pointerlockchange',
      this.onPointerlockChangeBind
    );

  }

  disconnect() {
    this.domElement.removeEventListener(
      'touchmove',
      this.onTouchMoveBind,
      false
    );
    this.domElement.removeEventListener('touchend', this.onTouchEndBind, false);
    this.domElement.ownerDocument.removeEventListener(
      'mousemove',
      this.onMouseMoveBind
    );
    this.domElement.ownerDocument.removeEventListener(
      'pointerlockchange',
      this.onPointerlockChangeBind
    );
    this.domElement.ownerDocument.removeEventListener(
      'pointerlockerror',
      this.onPointerlockErrorBind
    );
  }

  dispose() {
    this.disconnect();
  }

  getObject() {
    return this.camera;
  }

  getDirection() {
    const direction = new Vector3(0, 0, -1);

    return (v) => {
      return v.copy(direction).applyQuaternion(this.camera.quaternion);
    };
  }

  moveForward(distance) {
    this.vector.setFromMatrixColumn(this.camera.matrix, 0);

    this.vector.crossVectors(this.camera.up, this.vector);

    this.camera.position.addScaledVector(this.vector, distance);
  }

  moveRight(distance) {
    this.vector.setFromMatrixColumn(this.camera.matrix, 0);

    this.camera.position.addScaledVector(this.vector, distance);
  }

  lock() {
    if (typeof this.domElement.requestPointerLock !== 'undefined')
      this.domElement.requestPointerLock();
  }

  unlock() {
    if (typeof this.domElement.requestPointerLock !== 'undefined')
      this.domElement.ownerDocument.exitPointerLock();
  }
}

export { PointerLockControls2 };