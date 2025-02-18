const childProcess = require('child_process');
const fs = require('fs');
const path = require('path');

const repoDir = path.join(__dirname, '..');
const testDirectory = path.join(repoDir, 'working', 'test');

function GetTargets() {
  const dirs = fs.readdirSync(testDirectory, {withFileTypes: true})
                 .filter(dirent => dirent.isDirectory());
  let targets = [];
  for (const dir of dirs) {
    targets.push(dir.name);
  }
  return targets;
}

function GetInfo(testName, fullBuildDir) {
  let testInfo = {};
  testInfo.name = testName;
  testInfo.workingDir = path.join(testDirectory, testName);
  testInfo.command = path.join(fullBuildDir, 'src', 'test', testName);
  testInfo.goldenFile = path.join(testInfo.workingDir, 'out.txt');
  testInfo.diffFile = path.join(testInfo.workingDir, 'out_diff.txt');
  return testInfo;
}

function Run(testInfo) {
  process.chdir(testInfo.workingDir);
  try {
    childProcess.execSync(testInfo.command, {stdio: 'inherit'});
  }
  catch (error) {
    return;
  }
}

function Diff(testInfo) {
  let command = testInfo.command + ' > out_diff.txt'
  process.chdir(testInfo.workingDir);
  childProcess.execSync(command, {stdio: 'inherit'});
  let diffCommand = 'diff --strip-trailing-cr --unified=10 --color=always ' +
    testInfo.goldenFile + ' ' + testInfo.diffFile;
  try {
    childProcess.execSync(diffCommand, {stdio: 'inherit'});
  }
  catch (error) {
  }
}

function Overwrite(testInfo) {
  process.chdir(testInfo.workingDir);
  let overwiteCommand = testInfo.command + ' > out.txt';
  childProcess.execSync(overwiteCommand, {stdio: 'inherit'});
}

function Test(testInfo) {
  let command = testInfo.command + ' > out_diff.txt'
  process.chdir(testInfo.workingDir);
  childProcess.execSync(command, {stdio: 'inherit'});
  let diffCommand =
    'diff --strip-trailing-cr ' + testInfo.goldenFile + ' ' + testInfo.diffFile;
  let testStatus = '\033[1;92mPassed\033[0m';
  try {
    childProcess.execSync(diffCommand);
  }
  catch (error) {
    testStatus = '\033[1;91mFailed\033[0m';
  }
  console.log(testInfo.name + ': ' + testStatus);
}

function Coverage(testInfo) {
  let srcsDir = path.join(repoDir, 'src', '*');
  let coverageDir = path.join(testInfo.workingDir, 'coverage');
  process.chdir(path.dirname(testInfo.command))
  let coverageCommand = 'OpenCppCoverage -q' +
    ' --sources ' + srcsDir + ' --working_dir ' + testInfo.workingDir +
    ' --export_type html:' + coverageDir + ' -- ' + testInfo.command + '.exe';
  childProcess.execSync(coverageCommand);
}

module.exports = {
  GetTargets,
  GetInfo,
  Run,
  Diff,
  Overwrite,
  Test,
  Coverage,
};
