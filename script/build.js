let helpText = `\
node build.js <compiler> <configuration> (target) [r] [args]
<arg> - required
[arg] - optional

Required Arguments
compiler - The compiler directory.
configuration - The configuration directory.
target - The target to build.

Optional Arguments
r - Optionally run the target.
args - The command line arguments passed to the target.\
`
function HelpText() {
  console.log(helpText);
}

// Ensure correct usage.
if (process.argv.length < 5) {
  console.log('Error: Missing required arguments.');
  HelpText();
  return;
}
let compiler = process.argv[2];
let configuration = process.argv[3];
let target = process.argv[4];

// Ensure the build directory exists and build the target.
const fs = require("fs");
const path = require('path')
let buildDir = path.join(__dirname, '..', 'build', compiler, configuration);
if (!fs.existsSync(buildDir)) {
  console.log('Error: Build directory \'' + buildDir + '\' does not exist.');
  return;
}
let ninjaCommand = 'ninja ' + target;
process.chdir(buildDir);
const childProcess = require('child_process');
try {
  childProcess.execSync(ninjaCommand, { stdio: 'inherit' });
}
catch (error) {
  return;
}

// If requested, construct the target command and run the target.
if (process.argv.length < 6) {
  return;
}
let runArg = process.argv[5];
if (runArg !== 'r') {
  console.log('Error: \'' + runArg + '\' provided but only \'r\' is valid.');
  return;
}

let targetCommand = path.join(buildDir, target);
for (let i = 6; i < process.argv.length; ++i) {
  targetCommand += ' ' + process.argv[i];
}

let workingDir = path.join(__dirname, '..', 'working');
process.chdir(workingDir);
childProcess.execSync(targetCommand, { stdio: 'inherit' });