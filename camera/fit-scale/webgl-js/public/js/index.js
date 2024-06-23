import { mat4, glMatrix } from 'gl-matrix';

let gl = null;

function initWebGLContext(canvasName) {
    const canvas = document.getElementById(canvasName);
    if (canvas === null) {
        console.log(`Failed to get a canvas element with the name "${canvasName}"`);
        return false;
    }
    gl = canvas.getContext("webgl", { alpha: false, premultipliedAlpha: false });
    return true;
}

async function createProgram(path, vertShaderFileName,
    fragShaderFileName) //
{
    let response = await fetch(path + vertShaderFileName);
    const vertShaderSource = await response.text();
    response = await fetch(path + fragShaderFileName);
    const fragShaderSource = await response.text();

    const vShader = gl.createShader(gl.VERTEX_SHADER);
    gl.shaderSource(vShader, vertShaderSource);
    gl.compileShader(vShader);
    let ok = gl.getShaderParameter(vShader, gl.COMPILE_STATUS);
    if (!ok) {
        console.log(`${vertShaderFileName}: ${gl.getShaderInfoLog(vShader)}`);
        return null;
    }
    const fShader = gl.createShader(gl.FRAGMENT_SHADER);
    gl.shaderSource(fShader, fragShaderSource);
    gl.compileShader(fShader);
    ok = gl.getShaderParameter(fShader, gl.COMPILE_STATUS);
    if (!ok) {
        console.log(`${fragShaderFileName}: ${gl.getShaderInfoLog(fShader)}`);
        return null;
    }
    const program = gl.createProgram();
    gl.attachShader(program, vShader);
    gl.attachShader(program, fShader);
    gl.linkProgram(program);
    ok = gl.getProgramParameter(program, gl.LINK_STATUS);
    if (!ok) {
        console.log(`Link error with shaders ${vertShaderFileName}` +
            ` and ${fragShaderFileName}`);
        console.log(gl.getProgramInfoLog(program));
        return null;
    }    gl.useProgram(program);

    return program;
}

let aPositionLocation, vertPosBuffer;
let uColorLocation, uMvpMatrixLocation;
let viewportX, viewportY, viewportWidth, viewportHeight;

// Set up the model-view-projection matrix
const mvpMatrix = mat4.create();
const modelMatrix = mat4.create();
const viewMatrix = mat4.create();
const projMatrix = mat4.create();
const projViewMatrix = mat4.create();
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
    uColorLocation = gl.getUniformLocation(shaderProgram, "uColor");

    const worldWidth = 200;
    const worldHeight = 100;
    const worldAspect = worldWidth / worldHeight;

    window.onresize = () => {
        const w = gl.canvas.clientWidth;
        const h = gl.canvas.clientHeight;
        gl.canvas.width = w;
        gl.canvas.height = h;
        const canvasAspect = w / h;

        if (worldAspect > canvasAspect) {
            viewportWidth = w;
            viewportHeight = viewportWidth / worldAspect;
            viewportX = 0;
            viewportY = (h - viewportHeight) / 2;
        } else {
            viewportHeight = h;
            viewportWidth = viewportHeight * worldAspect;
            viewportX = (w - viewportWidth) / 2;
            viewportY = 0;
        }

        mat4.identity(projMatrix);
        mat4.ortho(projMatrix, 0, worldWidth, 0, worldHeight, 1, -1);
        mat4.mul(projViewMatrix, projMatrix, viewMatrix);

        draw();
    };
    window.onresize(null);
}

init();

function draw() {
    console.log(viewportX, viewportY, viewportWidth, viewportHeight);
    gl.clearColor(0.2, 0.2, 0.2, 1);
    gl.clear(gl.COLOR_BUFFER_BIT);
    gl.viewport(viewportX, viewportY, viewportWidth, viewportHeight);
    gl.scissor(viewportX, viewportY, viewportWidth, viewportHeight);
    gl.clearColor(0.04, 0.62, 0.48, 1);
    gl.enable(gl.SCISSOR_TEST);
    gl.clear(gl.COLOR_BUFFER_BIT);
    gl.disable(gl.SCISSOR_TEST);

    gl.bindBuffer(gl.ARRAY_BUFFER, vertPosBuffer);
    gl.vertexAttribPointer(aPositionLocation, 2, gl.FLOAT, false, 0, 0);
    gl.enableVertexAttribArray(aPositionLocation);

    // Square
    drawRectangle(100, 50, 50, 50, 0, [0.3, 0.07, 0.5]);
    // Border color
    const borderColor = [0.62, 0.04, 0.18];
    // Left border
    drawRectangle(5, 50, 5, 85, 0, borderColor);
    // Right border
    drawRectangle(195, 50, 5, 85, 0, borderColor);
    // Top border
    drawRectangle(100, 95, 185, 5, 0, borderColor);
    // Bottom border
    drawRectangle(100, 5, 185, 5, 0, borderColor);
}

function drawRectangle(x, y, w, h, angle, color) {
    mat4.identity(modelMatrix);
    mat4.translate(modelMatrix, modelMatrix, [x, y, 0]);
    mat4.rotateZ(modelMatrix, modelMatrix, glMatrix.toRadian(angle));
    mat4.scale(modelMatrix, modelMatrix, [w, h, 1]);
    mat4.mul(mvpMatrix, projViewMatrix, modelMatrix);
    gl.uniformMatrix4fv(uMvpMatrixLocation, false, mvpMatrix);
    gl.uniform3fv(uColorLocation, color);
    gl.drawArrays(gl.TRIANGLE_STRIP, 0, 4);
}
//# sourceMappingURL=index.js.map
