#!/usr/bin/env node
'use strict'

const fs = require('fs')
const path = require('path')
const childProcess = require('child_process')

const PLUGIN_DIR = 'Plugins'
const CONFIG_FILENAME = '.ue4build.json'

// load custom config
let config = {}
if (fs.existsSync(CONFIG_FILENAME)) {
  config = JSON.parse(fs.readFileSync(CONFIG_FILENAME))
} else {
  console.log(`${CONFIG_FILENAME} not found. Generating.`)
  generateConfig()
  process.exit(0)
}

// log helpers
let logfh = null
function log (data) {
  if (!logfh) {
    logfh = fs.openSync(config.buildLog[1], 'w')
  }
  fs.writeSync(logfh, data)
}

// the modified 'outputPath' with '_#' appended to avoid conflicts
let outputDir = null

// make sure we can read the project file before proceeding
let project = JSON.parse(fs.readFileSync(config.projectFile[1]))
let projectBackup = config.projectFile[1] + '.bak'

runBuild()

// entrypoint build sequence.
// disables all mod plugins
// executes the main project build
function runBuild () {
  // first, backup the project file
  try { fs.unlinkSync(projectBackup) } catch (e) { /* pass */ }

  copyFile(config.projectFile[1], projectBackup).then(() => {
    // disable all dlc/mod plugins
    console.log('Disabling all mod plugins')
    for (let modPlug of config.modPlugins[1]) {
      if (modPlug.asMod[1]) {
        setPluginEnabled(modPlug.name[1], false)
      }
    }
    fs.writeFileSync(config.projectFile[1], JSON.stringify(project, null, '\t'))

    console.log('= Execute Main Build =')
    runUAT(getMainBuildParams()).then(runBuildStep2, (err) => {
      console.error('Exited with code: ' + err.code)
      process.exit(1)
    })
  })
}

// step 2 of the build sequence
// re-enables all mod plugins
// executes individual mod(dlc) builds in sequence
function runBuildStep2 () {
  // re-enable all dlc/mod plugins
  console.log('Reenabling all mod plugins')
  for (let modPlug of config.modPlugins[1]) {
    if (modPlug.asMod[1]) {
      setPluginEnabled(modPlug.name[1], true)
    }
  }
  fs.writeFileSync(config.projectFile[1], JSON.stringify(project, null, '\t'))

  // build all the mods
  let modsToBuild = []
  for (let modPlug of config.modPlugins[1]) {
    if (modPlug.asMod[1]) {
      modsToBuild.push(modPlug.name[1])
    }
  }
  let buildNext = () => {
    if (!modsToBuild.length) {
      runBuildStep3()
      return
    }
    let modName = modsToBuild.shift()
    console.log('= Execute Mod Build - ' + modName + ' =')
    runUAT(getModBuildParams(modName)).then(buildNext, (err) => {
      // check for update resource bug
      if (err.result.indexOf("Program.Main: ERROR: AutomationTool terminated with exception: System.Exception: Couldn't update resource") > -1 && err.result.indexOf('Project.RunUnrealPak: UnrealPak Done') > -1) {
        log('^-- NOTE: this is an editor bug... the build failed but not before generating\nthe needed .pak and AssetRegistry.bin files. Everything is OK, proceeding...\n')
        buildNext()
        return
      }
      console.error('Exited with code: ' + err.code)
      process.exit(1)
    })
  }
  buildNext()
}

// step 3 of the build sequence
// copies the main build to the output dir
function runBuildStep3 () {
  let source = path.resolve(path.normalize(`Saved/StagedBuilds/${config.platformDirName[1]}`))
  let dest = config.outputPath[1]
  outputDir = dest
  let num = 1
  while (fs.existsSync(outputDir)) {
    outputDir = dest + '_' + num
    ++num
  }
  console.log('Copying ' + source + ' to ' + outputDir)
  copyDir(source, outputDir).then(runBuildStep4, (err) => {
    console.error(err)
    process.exit(1)
  })
}

// step 4 of the build sequence
// copies all mod .pak and .bin files to the output dir
function runBuildStep4 () {
  // build all the mods
  let modsToCopy = []
  for (let modPlug of config.modPlugins[1]) {
    if (modPlug.asMod[1]) {
      modsToCopy.push(modPlug.name[1])
    }
  }
  let buildNext = () => {
    if (!modsToCopy.length) {
      runBuildStep5()
      return
    }
    let modName = modsToCopy.shift()
    let all = []
    let source = path.resolve(path.normalize(`Plugins/${modName}/Saved/Cooked/${config.platformDirName[1]}/${config.projectName[1]}/AssetRegistry.bin`))
    let dest = path.resolve(path.normalize(`${outputDir}/${config.projectName[1]}/Content/Paks/${modName}.bin`))
    console.log('Copying ' + source + ' to ' + dest)
    all.push(copyFile(source, dest))
    source = path.resolve(path.normalize(`Plugins/${modName}/Saved/StagedBuilds/${config.platformDirName[1]}/${config.projectName[1]}/Content/Paks/${config.projectName[1]}-${config.platformDirName[1]}.pak`))
    dest = path.resolve(path.normalize(`${outputDir}/${config.projectName[1]}/Content/Paks/${modName}.pak`))
    console.log('Copying ' + source + ' to ' + dest)
    all.push(copyFile(source, dest))
    Promise.all(all).then(() => {
      buildNext()
    }, (err) => {
      console.error(err)
      process.exit(1)
    })
  }
  buildNext()
}

// step 5 of the build sequence
// cleans up backup files and closes the log file
function runBuildStep5 () {
  // cleanup backup file
  try { fs.unlinkSync(projectBackup) } catch (e) { /* pass */ }
  if (logfh) {
    fs.closeSync(logfh)
    logfh = null
  }

  console.log('ue4build Complete. -> ' + outputDir)
}

// search through the .uproject json to enable/disable a plugin
function setPluginEnabled (name, enabled) {
  if (!('Plugins' in project)) {
    project.Plugins = []
  }
  for (let plug of project.Plugins) {
    if (plug.Name === name) {
      plug.Enabled = enabled
      return
    }
  }
  project.Plugins.push({
    Name: name,
    Enabled: enabled
  })
}

// if we don't have a config yet... scan the directory and build a default
function generateConfig () {
  const MOD_TPL = {
    name: ['# Name of the Mod (DLC) plugin', ''],
    asMod: ["# if 'true' will be handled as a mod", false]
  }

  config = {
    projectName: ['# the project name', ''],
    projectFile: ['# the .uproject file to build', ''],
    buildConfig: ["# 'DebugGame', 'Development', or 'Shipping'", 'Development'],
    targetPlatform: ['# Unreal build target platform', 'Win64'],
    platformDirName: ['# The name unreal will give to StagedBuild platform directories', 'WindowsNoEditor'],
    buildLog: ['# where to output AutomationTool.exe messages', '.ue4build.log'],
    outputPath: ['# will copy the build tree to this destination', 'Releases/ModBuild'],
    uatCommand: ['# path to the RunUAT batch or shell script', '/path/to/RunUAT'],
    modPlugins: ['# list of plugins to treat as dlc mods', []]
  }

  config.outputPath[1] = path.resolve(path.normalize(config.outputPath[1]))

  // determine project name
  let projectList = fs.readdirSync('.').filter((item) => {
    if (fs.statSync(item).isFile() && path.extname(item) === '.uproject') {
      return true
    }
  })
  if (projectList.length) {
    config.projectFile[1] = path.resolve(path.normalize(projectList[0]))
  }
  config.projectName[1] = path.parse(config.projectFile[1]).name

  // determine engine version
  let project = JSON.parse(fs.readFileSync(config.projectFile[1]))
  let engineVersion = project.EngineAssociation

  let disabledPlugins = {}
  if ('Plugins' in project) {
    for (let p of project.Plugins) {
      if (p.Enabled === false) {
        disabledPlugins[p.Name] = true
      }
    }
  }

  // determine target platform
  switch (process.platform) {
    case 'darwin':
      config.targetPlatform[1] = 'Darwin'
      config.platformDirName[1] = 'MacNoEditor'
      break
    default:
      config.targetPlatform[1] = 'Win64'
      config.platformDirName[1] = 'WindowsNoEditor'
      break
  }

  // determine uat path
  let uatSearch = [
    path.resolve(path.normalize(`C:\\Program Files\\Epic Games\\UE_${engineVersion}\\Engine\\Build\\BatchFiles\\RunUAT.bat`))
  ]
  for (let cmd of uatSearch) {
    if (fs.existsSync(cmd)) {
      config.uatCommand[1] = cmd
      break
    }
  }

  // scan for plugins
  let pluginList = fs.readdirSync(PLUGIN_DIR).filter((item) => {
    if (fs.statSync(path.join(PLUGIN_DIR, item)).isDirectory()) {
      return true
    }
  })

  let modPlugins = config.modPlugins[1]
  for (let pluginName of pluginList) {
    let mod = JSON.parse(JSON.stringify(MOD_TPL))
    mod.name[1] = pluginName
    mod.asMod[1] = !disabledPlugins[pluginName]
    modPlugins.push(mod)
  }

  // finish up
  fs.writeFileSync(CONFIG_FILENAME, JSON.stringify(config, null, '  '))
  console.log('generated config, please double check that everything is correct:')
  console.log(JSON.stringify(config, null, '  '))
}

// RunUAT parameters for doing a main project build
function getMainBuildParams () {
  return [
    'BuildCookRun',
    '-project="' + config.projectFile[1] + '"',
    '-noP4',
    '-clientconfig=' + config.buildConfig[1],
    '-serverconfig=' + config.buildConfig[1],
    '-nocompile',
    '-nocompileeditor',
    '-installed',
    '-ue4exe=UE4Editor-Cmd.exe',
    '-utf8output',
    '-platform=' + config.targetPlatform[1],
    '-targetplatform=' + config.targetPlatform[1],
    '-build',
    '-cook',
    '-map=',
    '-pak',
    '-createreleaseversion=1.0',
    '-compressed',
    '-stage',
    '-package'
  ]
}

// RunUAT parameters for doing a mod(dlc) build
function getModBuildParams (modName) {
  return [
    'BuildCookRun',
    '-project="' + config.projectFile[1] + '"',
    '-noP4',
    '-clientconfig=' + config.buildConfig[1],
    '-serverconfig=' + config.buildConfig[1],
    '-nocompile',
    '-nocompileeditor',
    '-installed',
    '-ue4exe=UE4Editor-Cmd.exe',
    '-utf8output',
    '-platform=' + config.targetPlatform[1],
    '-targetplatform=' + config.targetPlatform[1],
    '-build',
    '-cook',
    '-map=',
    '-pak',
    '-dlcname=' + modName,
    '-basedonreleaseversion=1.0',
    '-compressed',
    '-stage',
    '-package'
  ]
}

// Execute the actual RunUAT process in a sub-shell-process
function runUAT (args) {
  let result = ''
  return new Promise((resolve, reject) => {
    let cmd = '"' + config.uatCommand[1] + '"'
    console.log(cmd + ' ' + args.join(' '))
    let proc = childProcess.spawn(cmd, args, {
      shell: true
    })
    proc.stdout.on('data', (data) => {
      result += data.toString()
      log(data.toString())
      process.stdout.write('.')
    })
    proc.stderr.on('data', (data) => {
      process.stderr.write(data)
    })
    proc.on('close', (code) => {
      process.stdout.write('\n')
      if (code === 0) {
        resolve(result)
      } else {
        let err = new Error()
        err.code = code
        err.result = result
        reject(err)
      }
    })
  })
}

// copy a single file
function copyFile (source, target) {
  return new Promise((resolve, reject) => {
    var rd = fs.createReadStream(source)
    rd.on('error', rejectCleanup)
    var wr = fs.createWriteStream(target)
    wr.on('error', rejectCleanup)
    function rejectCleanup (err) {
      rd.destroy()
      wr.end()
      reject(err)
    }
    wr.on('finish', resolve)
    rd.pipe(wr)
  })
}

// recursively copy a directory
function copyDir (source, target) {
  return new Promise((resolve, reject) => {
    let all = []
    fs.mkdirSync(target)
    let sub = fs.readdirSync(source)
    for (let file of sub) {
      let subtarget = path.join(target, file)
      file = path.join(source, file)
      let stat = fs.statSync(file)
      if (stat.isDirectory()) {
        all.push(copyDir(file, subtarget))
      } else if (stat.isFile()) {
        all.push(copyFile(file, subtarget))
      }
    }
    Promise.all(all).then(resolve, reject)
  })
}
