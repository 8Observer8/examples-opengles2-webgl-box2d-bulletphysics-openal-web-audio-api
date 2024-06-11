
const canvas = document.getElementById("renderCanvas");
const gl = (canvas as HTMLCanvasElement).getContext("webgl");

gl.clearColor(48 / 255, 56 / 255, 65 / 255, 1);
gl.clear(gl.COLOR_BUFFER_BIT);
