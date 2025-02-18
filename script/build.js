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
[-st|--showTests] - Show the possible test targets
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
    be found here (https://github.com/OpenCppCoverage/OpenCppCoverage). Only
    works on Windows with msvc debug builds.`;
function HelpText() {
  console.log(helpText);
}

const childProcess = require('child_process');
const fs = require('fs');
const path = require('path');
const opt = require('./option');
const test = require('./test');

let switches = [
  new opt.Switch('help', 'h', opt.ArgType.Single),
  new opt.Switch('showSavedOptions', 's', opt.ArgType.Single),
  new opt.Switch('subBuildDirectory', 'b', opt.ArgType.Required),
  new opt.Switch('target', 't', opt.ArgType.Required),
  new opt.Switch('run', 'r', opt.ArgType.Optional),
  new opt.Switch('testMode', 'tm', opt.ArgType.Optional),
  new opt.Switch('showTests', 'st', opt.ArgType.Single),
  new opt.Switch('testTarget', 'tt', opt.ArgType.Required),
  new opt.Switch('testAction', 'ta', opt.ArgType.Required),
];

// Default values of the saved options.
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
  options = opt.ProcessClArgs(switches, process.argv.slice(2));
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
if (options.showTests) {
  console.log(test.GetTargets());
  return;
}

// Create the full build diretory and ensure its validity.
const fullBuildDir =
  path.join(__dirname, '..', 'build', savedOptions.subBuildDirectory);
if (!fs.existsSync(fullBuildDir + '/build.ninja')) {
  console.error('build.ninja was not found in \'' + fullBuildDir + '\'.');
  return;
}

function BuildTarget(target) {
  const targetNinjaCommand = 'ninja ' + target;
  process.chdir(fullBuildDir);
  childProcess.execSync(targetNinjaCommand, {stdio: 'inherit'});
}

if (savedOptions.testMode == 'no') {
  // Perform normal mode operations.
  if (savedOptions.target == '') {
    console.error('No target set. Set one with \'-t\'.');
    return;
  }

  // Build the target.
  try {
    BuildTarget(savedOptions.target);
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
}
else {
  // Perform test mode operations.
  if (savedOptions.testTarget == '') {
    console.error('No test target set. Set one with \'-tt\'.');
    return;
  }

  // Build the target.
  try {
    if (savedOptions.testTarget == 'all') {
      BuildTarget('tests');
    }
    else {
      BuildTarget(savedOptions.testTarget);
    }
  }
  catch (error) {
    return;
  }

  if (options.testAction === undefined) {
    return;
  }
  if (options.testAction === 'c' && process.platform !== 'win32') {
    console.error('Coverage reports only available on Windows under msvc');
    return;
  }

  // Run the desired action for all tests.
  if (savedOptions.testTarget === 'all') {
    const targets = test.GetTargets();
    for (const target of targets) {
      const testInfo = test.GetInfo(target, fullBuildDir);
      switch (options.testAction) {
      case 't': test.Test(testInfo); break;
      case 'c': test.Coverage(testInfo); break;
      default:
        console.error(
          '\'' + options.testAction + '\' is not a valid action for all.');
        return;
      }
    }
    return;
  }

  // Run the desired action for a single test.
  const testInfo = test.GetInfo(savedOptions.testTarget, fullBuildDir);
  switch (options.testAction) {
  case 'r': test.Run(testInfo); break;
  case 'd': test.Diff(testInfo); break;
  case 'o': test.Overwrite(testInfo); break;
  case 't': test.Test(testInfo); break;
  case 'c': test.Coverage(testInfo); break;
  default:
    console.error('\'' + options.testAction + '\' is not a valid action.');
  }
}
