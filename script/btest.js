let helpText = `\
node btest.js <compiler> <configuration> <(all [t|c]) | (<target> [r|d|o|t|c])>
Build, test, and ensure tests are passing.
<arg> - required
[arg] - optional

Required Arguments.
compiler - The compiler directory.
configuration - The configuration directory.
all - Build all of the existing test targets.
target - Build only the provided target.

Optional Arguments.
r - Runs the target executable if successfully built.
d - Creates a file containing the output of the target executable named
  {target}/out_diff.txt and git diffs it against {target}/out.txt.
o - Creates a file containing the output of the target executable named
  {target}/out.txt. The file will be overwritten if it already exists.
t - Does the same as the d option, but only prints out "{target}: Passed"
  or "{target}: Failed" depending on the result of the diff. When this
  option is used with the all argument, every existing test will be executed
  and dispalyed with "{target}: Passed" or {target}: Failed".
c - Creates a directory named {target}/coverage that contains a code
  coverage report. This makes finding the code that a unit test did and did
  not run easy. A build of OpenCppCoverage needs to be in your path. It can
  be found here (https://github.com/OpenCppCoverage/OpenCppCoverage). Only on
  Windows under msvc.`
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
if (target === 'all') {
  target = 'tests'
}

// Ensure the build directory exists and build the target.
const fs = require('fs');
const path = require('node:path')
let repoDir = path.join(__dirname, '..');
let buildDir = path.join(repoDir, 'build', compiler, configuration);
if (!fs.existsSync(buildDir)) {
  console.log('Error: Build directory ' + buildDir + ' does not exist.');
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

// Exit if there is no action to perform.
if (process.argv.length < 6) {
  return;
}
let action = process.argv[5];
if (action === 'c' && process.platform !== 'win32') {
  console.log('Error: Coverage reports only available on Windows under msvc');
  return;
}

let generalTestDir = path.join(__dirname, '..', 'working', 'test');
function GetTestInfo(testName) {
  let testInfo = {};
  testInfo.workingDir = path.join(generalTestDir, testName);
  testInfo.command = path.join(buildDir, 'src', 'test', testName);
  testInfo.goldenFile = path.join(testInfo.workingDir, 'out.txt');
  testInfo.diffFile = path.join(testInfo.workingDir, 'out_diff.txt');
  return testInfo;
}

// Definitions for actions.
function RunTest(testName) {
  let testInfo = GetTestInfo(testName);
  process.chdir(testInfo.workingDir);
  childProcess.execSync(testInfo.command, { stdio: 'inherit' });
}

function DiffTest(testName) {
  let testInfo = GetTestInfo(testName);
  let command = testInfo.command + ' > out_diff.txt'
  process.chdir(testInfo.workingDir);
  childProcess.execSync(command, { stdio: 'inherit' });
  let diffCommand = 'diff --strip-trailing-cr --unified=10 --color=always '
    + testInfo.goldenFile + ' ' + testInfo.diffFile;
  try {
    childProcess.execSync(diffCommand, { stdio: 'inherit' });
  }
  catch (error) { }
}

function OverwriteTest(testName) {
  let testInfo = GetTestInfo(testName);
  process.chdir(testInfo.workingDir);
  let overwiteCommand = testInfo.command + ' > out.txt';
  childProcess.execSync(overwiteCommand, { stdio: 'inherit' });
}

function PerformTest(testName) {
  let testInfo = GetTestInfo(testName);
  let command = testInfo.command + ' > out_diff.txt'
  process.chdir(testInfo.workingDir);
  childProcess.execSync(command, { stdio: 'inherit' });
  let diffCommand = 'diff --strip-trailing-cr '
    + testInfo.goldenFile + ' ' + testInfo.diffFile;
  let testStatus = '\033[1;92mPassed\033[0m';
  try {
    childProcess.execSync(diffCommand);
  }
  catch (error) {
    testStatus = '\033[1;91mFailed\033[0m';
  }
  console.log(testName + ': ' + testStatus);
}

function CoverageReport(testName) {
  let testInfo = GetTestInfo(testName);
  let srcsDir = path.join(repoDir, 'src', '*');
  let coverageDir = path.join(testInfo.workingDir, 'coverage');
  process.chdir(path.dirname(testInfo.command))
  let coverageCommand = 'OpenCppCoverage -q'
    + ' --sources ' + srcsDir
    + ' --working_dir ' + testInfo.workingDir
    + ' --export_type html:' + coverageDir
    + ' -- ' + testInfo.command + '.exe';
  childProcess.execSync(coverageCommand);
}

// Run the desired action for all targets or a single target.
if (target === 'tests') {
  let dirEntries = fs.readdirSync(generalTestDir, { withFileTypes: true })
    .filter(dirent => dirent.isDirectory())
  if (action === 't') {
    for (let i = 0; i < dirEntries.length; ++i) {
      PerformTest(dirEntries[i].name);
    }
  }
  else if (action === 'c') {
    for (let i = 0; i < dirEntries.length; ++i) {
      CoverageReport(dirEntries[i].name);
    }
  }
  else {
    console.log('Error: \'' + action + '\' is not a valid action for all.');
  }
  return;
}

if (action === 'r') {
  RunTest(target);
}
else if (action === 'd') {
  DiffTest(target);
}
else if (action === 'o') {
  OverwriteTest(target);
}
else if (action === 't') {
  PerformTest(target);
}
else if (action == 'c') {
  CoverageReport(target);
}