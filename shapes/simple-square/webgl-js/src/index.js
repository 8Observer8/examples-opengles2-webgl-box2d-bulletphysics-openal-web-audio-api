import { gl, initWebGLContext } from "./webgl-context.js";
import createProgram from "./shader-program.js";

let aPositionLocation, vertPosBuffer;

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

    gl.clearColor(48 / 255, 56 / 255, 65 / 255, 1);
    draw();
}

init();

function draw() {
    gl.clear(gl.COLOR_BUFFER_BIT);

    gl.bindBuffer(gl.ARRAY_BUFFER, vertPosBuffer);
    gl.vertexAttribPointer(aPositionLocation, 2, gl.FLOAT, false, 0, 0);
    gl.enableVertexAttribArray(aPositionLocation);

    gl.drawArrays(gl.TRIANGLE_STRIP, 0, 4)
}
