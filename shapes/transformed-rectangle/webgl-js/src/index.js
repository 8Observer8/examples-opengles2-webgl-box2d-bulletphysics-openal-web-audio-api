import { glMatrix, mat4, vec3 } from "gl-matrix";
import { gl, initWebGLContext } from "./webgl-context.js";
import createProgram from "./shader-program.js";

let aPositionLocation, uMvpMatrixLocation, vertPosBuffer;

// Set up the model-view-projection matrix
const mvpMatrix = mat4.create();
const modelMatrix = mat4.create();
const viewMatrix = mat4.create();
const projMatrix = mat4.create();
const projViewMatrix = mat4.create();
mat4.ortho(projMatrix, 0, 100, 0, 100, 1, -1);
mat4.lookAt(viewMatrix, [0, 0, 1], [0, 0, 0], [0, 1, 0]);

async function init() {
    if (!initWebGLContext("renderCanvas")) {
        return;
    }

    const shaderProgram = await createProgram("assets/shaders/",
        "color.vert", "color.frag");

    const vertPositions = [
        -0.5, -0.5,
        0.5, -0.5,
        -0.5, 0.5,
        0.5, 0.5
    ];
    vertPosBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vertPosBuffer);
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(vertPositions), gl.STATIC_DRAW);
    // Set up the position attribute
    aPositionLocation = gl.getAttribLocation(shaderProgram, "aPosition");

    uMvpMatrixLocation = gl.getUniformLocation(shaderProgram, "uMvpMatrix");

    gl.clearColor(0.188, 0.22, 0.255, 1);
    draw();
}

init();

function draw() {
    gl.clear(gl.COLOR_BUFFER_BIT);

    gl.bindBuffer(gl.ARRAY_BUFFER, vertPosBuffer);
    gl.vertexAttribPointer(aPositionLocation, 2, gl.FLOAT, false, 0, 0);
    gl.enableVertexAttribArray(aPositionLocation);

    mat4.identity(modelMatrix);
    mat4.mul(projViewMatrix, projMatrix, viewMatrix);
    mat4.translate(modelMatrix, modelMatrix, [50, 50, 0]);
    mat4.rotateZ(modelMatrix, modelMatrix, glMatrix.toRadian(10));
    mat4.scale(modelMatrix, modelMatrix, [80, 10, 1]);
    mat4.mul(mvpMatrix, projViewMatrix, modelMatrix);
    gl.uniformMatrix4fv(uMvpMatrixLocation, false, mvpMatrix);
    gl.drawArrays(gl.TRIANGLE_STRIP, 0, 4)
}
