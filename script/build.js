const helpText =
  `\
node build.js [options] [-- targetArgs]
[] - optional
<> - required
[-h |--help] - Show help text
[-s |--showSavedOptions] - Show the current option values
[-b |--subBuildDir] <directory> - Directory in build/ containing 'build.ninja'
[-t |--target] <target> - The target to build
[-r |--run] <yes|no> - Decide whether to run the target
[-tm|--testMode] <yes|no> - Enable or disable test mode
[-tt|--testTarget] <target|all> - Test target to build
[-ta|--testAction] <r|d|o|t|c> - The test action to perform
[targetArgs] - The command line arguments passed to the built target

Details
A <yes|no> option can be toggled by using the option without an argument.

--subBuildDir: It is expected that there is a directory called build/ in the
  root of the repository. 'subBuildDir' is the subdirectory within build/ that
  contains a 'ninja.build' file that's used for building with ninja.

--testTarget: A single test target can be specified but 'all' can also be used
  for building all tests.

--testAction: The possible actions are specified below. If the test target is
  'all', only the 't' and 'c' options are usable.
  r(un): Runs the target executable if successfully built.
  d(iff): Creates a file containing the output of the target executable named
    {target}/out_diff.txt and git diffs it against {target}/out.txt.
  o(verwrite): Creates a file containing the output of the target executable
    named {target}/out.txt. The file will be overwritten if it already exists.
  t(est): Does the same as the d option, but only prints out "{target}: Passed"
    or "{target}: Failed" depending on the result of the diff. When this
    option is used with the all argument, every existing test will be executed
    and dispalyed with "{target}: Passed" or {target}: Failed".
  c(overage): Creates a directory named {target}/coverage that contains a code
    coverage report. This makes finding the code that a unit test did and did
    not run easy. A build of OpenCppCoverage needs to be in your path. It can
    be found here (https://github.com/OpenCppCoverage/OpenCppCoverage). Only on
    Windows under msvc.`
function HelpText()
{
  console.log(helpText);
}

const childProcess = require('child_process');
const fs = require('fs');
const path = require('path');

const ArgType = {
  Single: "single",
  Optional: "optional",
  Required: "required"
};

// Used for identifying commandline arguments and what those arguments expect.
class Switch
{
  constructor(long, short, argType)
  {
    this.name = long;
    this.long = '--' + long;
    this.short = '-' + short;
    this.argType = argType
  }
  MatchingClArg(clArg)
  {
    return this.long == clArg || this.short == clArg;
  }
  static IsSwitch(arg)
  {
    return arg.slice(0, 1) == '-';
  }
}

// Processes the arguments given on the command line.
function ProcessClArgs(switches, args)
{
  let options = {};
  for (let a = 0; a < args.length; ++a) {
    if (args[a] == '--') {
      options.extras = args.slice(a + 1);
      break;
    }
    let matchFound = false;
    for (const currentSwitch of switches) {
      const arg = args[a];
      if (!currentSwitch.MatchingClArg(arg)) {
        continue;
      }
      matchFound = true;
      let subArg = args[a + 1];
      switch (currentSwitch.argType) {
      case ArgType.Single: options[currentSwitch.name] = true; break;
      case ArgType.Optional:
        options[currentSwitch.name] = [];
        if (subArg !== undefined && !Switch.IsSwitch(subArg)) {
          options[currentSwitch.name].push(subArg);
          ++a;
        }
        break;
      case ArgType.Required:
        if (subArg === undefined || Switch.IsSwitch(subArg)) {
          throw '\'' + currentSwitch.short + '\'/\'' + currentSwitch.long +
            '\' requires an argument.';
        }
        options[currentSwitch.name] = subArg;
        ++a;
        break;
      }
    }
    if (!matchFound) {
      throw 'Invalid arg \'' + arg + '\' used.';
    }
  }
  return options;
}

let switches = [
  new Switch('help', 'h', ArgType.Single),
  new Switch('showSavedOptions', 's', ArgType.Single),
  new Switch('subBuildDirectory', 'b', ArgType.Required),
  new Switch('target', 't', ArgType.Required),
  new Switch('run', 'r', ArgType.Optional),
  new Switch('testMode', 'tm', ArgType.Optional),
  new Switch('testTarget', 'tt', ArgType.Required),
  new Switch('testAction', 'ta', ArgType.Required),
];

// The default values of the saved options.
let savedOptions = {
  subBuildDirectory: '',
  target: '',
  targetArgs: {},
  run: 'no',
  testMode: 'no',
  testTarget: '',
};

// Process the options from the commandline and read the saved options.
let options;
try {
  options = ProcessClArgs(switches, process.argv.slice(2));
}
catch (error) {
  console.error(error);
  return;
}
try {
  savedOptions = require('./options.json');
}
catch (error) {
}

// Overwrite all saveable options.
for (const key in savedOptions) {
  if (options[key] === undefined) {
    continue;
  }
  if (['yes', 'no'].includes(savedOptions[key])) {
    if (options[key].length == 0) {
      savedOptions[key] = savedOptions[key] == 'yes' ? 'no' : 'yes';
      continue;
    }
    if (!['yes', 'no'].includes(options[key][0])) {
      console.error(
        'Invalid argument \'' + options[key] + '\' used for the \'' + key +
        '\' option. Expects \'yes\' or \'no\'.');
      continue;
    }
    savedOptions[key] = options[key][0];
    continue;
  }
  savedOptions[key] = options[key];
}
if (savedOptions.target != '' && options.extras !== undefined) {
  savedOptions.targetArgs[savedOptions.target] = options.extras;
}
fs.writeFileSync('options.json', JSON.stringify(savedOptions));

// Handle all early out arguments.
if (options.help) {
  HelpText();
  return;
}
if (options.showSavedOptions) {
  console.log(savedOptions);
  return;
}

// Create the full build diretory and ensure its validity.
const fullBuildDir =
  path.join(__dirname, '..', 'build', savedOptions.subBuildDirectory);
if (!fs.existsSync(fullBuildDir + '/build.ninja')) {
  console.error('build.ninja was not found in \'' + fullBuildDir + '\'.');
  return;
}

// Ensure that a target is set.
if (savedOptions.testMode == 'no' && savedOptions.target == '') {
  console.error('No target set. Set one with \'-t\'.');
  return;
}
if (savedOptions.testMode == 'yes' && savedOptions.testTarget == '') {
  console.error('No test target set. Set one with \'-tt\'.');
  return;
}

// Build the target.
const targetNinjaCommand = 'ninja ' + savedOptions.target;
process.chdir(fullBuildDir);
try {
  childProcess.execSync(targetNinjaCommand, {stdio: 'inherit'});
}
catch (error) {
  return;
}

// Run the built target if requested.
if (savedOptions.run == 'no') {
  return;
}
const workingDir = path.join(__dirname, '..', 'working');
process.chdir(workingDir);
try {
  let targetCommand = path.join(fullBuildDir, savedOptions.target);
  const targetArgs = savedOptions.targetArgs[savedOptions.target];
  if (targetArgs !== undefined) {
    for (const targetArg of targetArgs) {
      targetCommand += ' ' + targetArg;
    }
  }
  childProcess.execSync(targetCommand, {stdio: 'inherit'});
}
catch (error) {
  return;
}