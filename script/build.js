let helpText = `\
node build.js [options] [-- targetArgs]
[] - optional
<> - required
[-c |--compiler] <compiler> - The compiler directory.
[-cf|--configuration] <configuration> - The configuration directory.
[-t |--target] <target> - The target to build.
[-r |--run] <yes/no> - Decide whether to run the target.
[-h |--help] - Show help text.
[-s |--show-saved-options] - Exactly what it sounds like.
[targetArgs] - The command line arguments passed to the built target.`
function HelpText() {
  console.log(helpText);
}

const childProcess = require('child_process');
const fs = require('fs');
const path = require('path');

// Parses command line arguments and saves certain options to file so the
// script can be run without having to enter the same arguments.
class OptionCache {

  AddSwitch(long, short, defaultValue) {
    if (typeof this.switches == 'undefined') {
      this.switches = [];
    }
    this.switches.push({});
    let newSwitch = this.switches[this.switches.length - 1];
    newSwitch.name = long;
    newSwitch.longSwitch = '--' + long;
    newSwitch.shortSwitch = '-' + short;
    newSwitch.defaultValue = defaultValue;
  }

  IsSwitch(selectedSwitch, arg) {
    return selectedSwitch.longSwitch == arg
      || selectedSwitch.shortSwitch == arg;
  }

  constructor(args) {
    this.AddSwitch('compiler', 'c', '');
    this.AddSwitch('configuration', 'cf', '');
    this.AddSwitch('target', 't', '');
    this.AddSwitch('run', 'r', 'no');
    this.AddSwitch('help', 'h', null);
    this.AddSwitch('show-saved-options', 's');

    // Initialize all saved options.
    this.options = {};
    this.options.help = false;
    this.options.showSavedOptions = false;
    this.options.saved = {};
    try { this.options.saved = require('./options.json'); } catch (error) { }
    for (let i = 0; i < this.switches.length; ++i) {
      let savable = this.switches[i].defaultValue != null;
      let name = this.switches[i].name;
      if (savable && typeof this.options.saved[name] == 'undefined') {
        this.options.saved[name] = this.switches[i].defaultValue;
      }
    }

    // Set options gathered from command line arguments.
    let a = 0;
    while (a < args.length) {
      if (args[a] == '--') {
        a += 1;
        this.options.saved.targetArgs = [];
        break;
      }

      let s = 0;
      for (; s < this.switches.length; ++s) {
        let currentSwitch = this.switches[s];
        if (!this.IsSwitch(currentSwitch, args[a])) {
          continue;
        }
        if (currentSwitch.name == 'help') {
          this.options.help = true;
          a += 1;
          break;
        }
        if (currentSwitch.name == 'show-saved-options') {
          this.options.showSavedOptions = true;
          a += 1;
          break;
        }

        let nextArg = args[a + 1];
        if (typeof nextArg == 'undefined') {
          throw '\'' + currentSwitch.shortSwitch + '\'/\''
          + currentSwitch.longSwitch + '\' requires an argument.';
        }
        this.options.saved[currentSwitch.name] = nextArg;
        a += 2;
        break;
      }
      if (s == this.switches.length) {
        throw 'Invalid arg \'' + args[a] + '\' used.';
      }
    }
    for (; a < args.length; ++a) {
      this.options.saved.targetArgs.push(args[a]);
    }


    // Check the validity of options and derive options the from saved options.
    if (this.options.saved.target == '') {
      throw 'No target set. Set one with \'-t\'.';
    }
    if (this.options.saved.run != 'yes' && this.options.saved.run != 'no') {
      throw 'Invalid argument \'' + this.options.saved.run
      + '\' used for the \'run\' option. Expects \'yes\' or \'no\'.';
    }

    this.options.buildDir = path.join(__dirname, '..', 'build');
    if (this.options.saved.compiler != '') {
      this.options.buildDir = path.join(
        this.options.buildDir, this.options.saved.compiler);
    }
    if (this.options.saved.configuration != '') {
      this.options.buildDir = path.join(
        this.options.buildDir, this.options.saved.configuration);
    }
    if (!fs.existsSync(this.options.buildDir)) {
      throw 'Build directory \'' + this.options.buildDir + '\' does not exist.';
    }

    this.options.targetNinjaCommand = 'ninja ' + this.options.saved.target;
    this.options.targetCommand = path.join(
      this.options.buildDir, this.options.saved.target);
    for (let i = 0; i < this.options.saved.targetArgs.length; ++i) {
      this.options.targetCommand += ' ' + this.options.saved.targetArgs[i];
    }

    fs.writeFileSync('options.json', JSON.stringify(this.options.saved));
  }
}

let options;
try {
  let optionCache = new OptionCache(process.argv.slice(2));
  options = optionCache.options;
} catch (error) {
  console.log(error);
  return;
}

if (options.help) {
  HelpText();
  return;
}
if (options.showSavedOptions) {
  console.log(options.saved);
  return;
}

// Build the target.
process.chdir(options.buildDir);
try { childProcess.execSync(options.targetNinjaCommand, { stdio: 'inherit' }); }
catch (error) { return; }

// Run the built target if requested.
if (options.saved.run == 'no') {
  return;
}
let workingDir = path.join(__dirname, '..', 'working');
process.chdir(workingDir);
try { childProcess.execSync(options.targetCommand, { stdio: 'inherit' }); }
catch (error) { return; }