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
    const arg = args[a];
    if (arg == '--') {
      options.extras = args.slice(a + 1);
      break;
    }
    let matchFound = false;
    for (const currentSwitch of switches) {
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

module.exports = {
  ArgType,
  Switch,
  ProcessClArgs
};
