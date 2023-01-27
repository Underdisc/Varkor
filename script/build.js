let helpText = `\
Usage: node build.js (compiler) (configuration) (target) [r] [args]
(arg) - required
[arg] - optional
compiler - The compiler directory.
configuration - The configuration directory.
target - The target to build.
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
let buildDir = __dirname + '/../build/' + compiler + '/' + configuration
if (!fs.existsSync(buildDir)) {
  console.log('Error: Build directory ' + buildDir + ' does not exist.');
  return;
}
let ninjaCommand = 'ninja ' + target;
process.chdir(buildDir);
const childProcess = require('child_process');
childProcess.execSync(ninjaCommand, { stdio: 'inherit' });

// If requested, construct the target command and run the target.
if (process.argv.length < 6) {
  return;
}
let runArg = process.argv[5];
if (runArg !== 'r') {
  console.log('Error: \'' + runArg + '\' provided but only \'r\' is valid.');
  return;
}

let targetCommand = buildDir + '/' + target;
for (let i = 6; i < process.argv.length; ++i) {
  targetCommand += ' ' + process.argv[i];
}

let workingDir = __dirname + '/../working';
process.chdir(workingDir);
childProcess.execSync(targetCommand, { stdio: 'inherit' });