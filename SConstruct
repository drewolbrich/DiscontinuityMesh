
import os
import os.path
import subprocess

baseBuildDirectory = 'build'
binDirectory = 'bin'
sourceDirectory = 'src'
cppExtension = '.cpp'
hExtension = '.h'
cExtension = '.c'
programMainFilename = 'Main.cpp'
localBuildFilename = 'Build'

# Build the dictionary of source files in each directory,
# and the list of source files to pass to etags.
def findSourceFiles():
    for root, dirs, files in os.walk(sourceDirectory):
        # Don't waste time following Subversion directories.
        if '.svn' in dirs:
            dirs.remove('.svn')
        # Add all C++ files for this directory to the dictionary.
        for file in files:
            if (file.endswith(cppExtension) or file.endswith(cExtension) \
                   or file.endswith(hExtension)) \
                   and not file.startswith('.'):
                etagsSourceFileList.append(os.path.join(root, file))
            if (file.endswith(cppExtension) or file.endswith(cExtension)) \
               and not file.startswith('.'):
                directory = root
                if not directory in directoryDictionary:
                    directoryDictionary[directory] = []
                directoryDictionary[directory].append(os.path.join(root, file))
    
# Run a unit test, and then record the fact that the test ran
# successfully, so that it won't ever be run again unless the
# unit test executable changes.
def runUnitTest(env, target, source):
    # Only write out the file stamp if the unit test succeeds and returns exit code 0.
    print env['RETRO_BUILD_OUTPUT_PREFIX'],
    print 'Running unit test', str(source[0])
    args = [str(source[0].abspath)]
    if not env['RETRO_VERBOSE']:
        args.append('--quiet')
    if subprocess.call(args, cwd=env['RETRO_UNIT_TEST_DIRECTORY']) == 0:
        open(str(target[0]), 'w').write('')
        return 0  # Success.
    else:
        return 1  # Failure.

# This function is used instead of the regular SCons Install function
# so that programs will be installed even if SCons is executed from
# a source directory with 'scons -u'. With Install and -u, this would not happen,
# because it would be optimized out.
# We want to be able to sit in a source directory and only build the corresponding
# executable, and run it, even though we're copying all the executables to
# a top level bin directory.
def installExecutable(env, target, source):
    # print env['RETRO_BUILD_OUTPUT_PREFIX'],
    # print 'Copying', str(source[0])
    # Create the bin directory if it doesn't already exist.
    if not os.path.exists(binDirectory):
        subprocess.call(['mkdir', binDirectory])
    command = ['cp', str(source[0].abspath), binDirectory]
    if subprocess.call(command) == 0:
        open(str(target[0]), 'w').write('')
        return 0  # Success.
    else:
        return 1  # Failure.

# Print out a summary of the command line for a particular build step,
# rather than printing out the entire build step.
def printCommandLine(command, target, source, env):
    targetFilename = str(target[0])
    sourceFilename = str(source[0])
    baseCommand = command.split()[0]
    if baseCommand == 'g++' or baseCommand == 'gcc':
        print env['RETRO_BUILD_OUTPUT_PREFIX'],
        extension = sourceFilename.split('.')[-1]
        if extension == 'cpp' or extension == 'c':
            print 'Compiling', sourceFilename
        else:
            print 'Linking', targetFilename
    # elif baseCommand == 'ar':
        # print env['RETRO_BUILD_OUTPUT_PREFIX'],
        # print 'Updating archive', targetFilename

# Generate the Emacs TAGS file.
def generateEtagsFile(env, target, source):
    print "Rebuilding", str(target[0])
    args = ""
    command = ['etags', '-o', str(target[0])]
    for index in range(0, len(source)):
        command.append(str(source[index].abspath))
    if subprocess.call(command) == 0:
        return 0  # Success.
    else:
        return 1  # Failure.

# For every directory under the source directory, build a library or
# program out of all of the C++ source files in that directory.
def createTargets(debug, verbose):
    if debug:
        buildDirectory = os.path.join(baseBuildDirectory, 'debug')
    else:
        buildDirectory = os.path.join(baseBuildDirectory, 'opt')
    buildObjDirectory = os.path.join(buildDirectory, 'obj')
    buildLibDirectory = os.path.join(buildDirectory, 'lib')
    
    def convertSourcePathToBuildObjPath(sourcePath):
        return sourcePath.replace(sourceDirectory + '/', buildObjDirectory + '/')

    env = baseEnv.Clone()
    
    env.Append(CPPPATH=[sourceDirectory])
    env.Append(LIBPATH=[buildLibDirectory])
    env.Append(LIBPATH=['/opt/local/lib'])

    if debug:
        env.Append(RETRO_BUILD_OUTPUT_PREFIX="(debug)")
    else:
        env.Append(RETRO_BUILD_OUTPUT_PREFIX="(opt)")

    # Tell SCons to put built object files and libraries in a separate directory.
    env.VariantDir(buildObjDirectory, sourceDirectory, duplicate=0)

    cxxFlags = ''
    # Specify /opt/local/include via -isystem, rather than in CPPPATH, above,
    # so the compiler won't complain about Boost library warnings.
    cxxFlags += ' -isystem /opt/local/include'
    cxxFlags += ' -ansi'                # Disable GNU C++ extensions.
    cxxFlags += ' -std=c++98'           # Conform to 1998 ISO C++ standard plus amendments.
    cxxFlags += ' -pedantic'            # Issue all warnings demanded by strict ISO C++.
    cxxFlags += ' -Wall'                # Enable all warnings.
    cxxFlags += ' -Wextra'              # Enable several other warnings.
    cxxFlags += ' -Wcast-qual'          # Warn when pointer cast removes type qualifier.
    cxxFlags += ' -Wcast-align'         # Warn when cast increases required alignment.
    cxxFlags += ' -Wwrite-strings'      # Warn about conversions from string constants to char *.
    cxxFlags += ' -Woverloaded-virtual' # Warn when hiding virtual function from base class.
    cxxFlags += ' -Werror'              # Treat all warnings as errors.
    cxxFlags += ' -pthread'             # Add support for multithreading.
    cxxFlags += ' -pipe'                # Use pipes to communicate between compilation stages.
    env.Append(CXXFLAGS=cxxFlags)

    ccFlags = ''
    if debug:
        ccFlags += ' -ggdb'             # Produce debugging information.
        ccFlags += ' -DDEBUG'           # Enable debugging code.
    else:
        ccFlags += ' -fast'             # Maximum optimization.
        ccFlags += ' -DNDEBUG'          # Suppress assert statements.
    env.Append(CCFLAGS=ccFlags)

    # Build programs and libraries out of the directories.
    for directory in directoryDictionary:
        buildDirectorySourceFileList = []
        for file in directoryDictionary[directory]:
            if file != os.path.join(directory, programMainFilename):
                buildDirectorySourceFileList.append(
                    convertSourcePathToBuildObjPath(file))
        buildFilename = os.path.join(directory, localBuildFilename)

        # Parse the Build file if it exists. The optional Build file may contain SCons
        # construction variables specific to this directory.
        globals = {}
        locals = {}
        if os.path.exists(buildFilename):
            execfile(buildFilename, globals, locals)
            if 'LIBS' in locals:
                # Append the appropriate suffixes to all Boost library names.
                new_libs = []
                for lib in locals['LIBS']:
                    if lib[0:6] == 'boost_':
                        if debug:
                            lib += '-mt-d'
                        else:
                            lib += '-mt'
                    new_libs.append(lib)
                locals['LIBS'] = new_libs

        # Starting with the full directory path, replace 'src/' with 'build/',
        # to determine the directory where targets will be placed.
        targetDirectory = convertSourcePathToBuildObjPath(directory)

        baseDirectory = directory.split('/')[-1]
        if baseDirectory == 'test':
            # If the directory is called 'test', we assume it's a unit test.
            
            libraryName = directory.split('/')[-2]
            testProgramFilename = os.path.join(targetDirectory, 'test_' + libraryName)
            if debug:
                testProgramFilename += '.debug'
            testProgram = env.Program(testProgramFilename, buildDirectorySourceFileList, **locals)
            env.Requires(testProgram, etagsFile)

            # Install the unit test program in the top level bin directory.
            env.Command(os.path.join(targetDirectory, 'executable_installed'),
                        testProgram, installExecutable)

            # Run the unit test program whenever we've determined that it has changed.
            # RETRO_UNIT_TEST_DIRECTORY is passed so the unit test can be
            # run in the directory where its source files are located,
            # so that it may have access to special input files.
            env.Command(os.path.join(targetDirectory, 'unit_test_passed'),
                        testProgram, runUnitTest,
                        RETRO_UNIT_TEST_DIRECTORY=directory, RETRO_VERBOSE=verbose)

        elif len(directory.split('/')) == 2 \
                 or 'sandbox' in directory.split('/'):

            # Except for the case of 'test' above, and everything under src/sandbox,
            # ignore all subdirectories of libraries and programs.

            if os.path.join(directory, programMainFilename) in directoryDictionary[directory]:
                # If the directory contains Main.cpp, we assume it's a program.

                # If any source files other than Main.cpp exist for this program,
                # build a separate library for them with a name that matches
                # the name of the program. This allows us to create unit tests
                # for the program in a test subdirectory, and link in
                # all of the program's object files, with the exception of the
                # object file created from Main.cpp.
                if buildDirectorySourceFileList != []:
                    env.Library(os.path.join(buildLibDirectory, baseDirectory),
                                buildDirectorySourceFileList, **locals)

                baseExecutableFilename = baseDirectory
                if debug:
                    baseExecutableFilename += '.debug'
                executableFilename = os.path.join(targetDirectory, baseExecutableFilename)

                mainCppOnlyBuildDirectorySourceFileList = [convertSourcePathToBuildObjPath(
                    os.path.join(directory, programMainFilename))]

                # Add the library that rides along with the program, if it exists.
                if buildDirectorySourceFileList != []:
                    locals['LIBS'].insert(0, baseDirectory)

                # Only Main.cpp is specified in the list of source files to compile,
                # everything else gets linked in via the library that is
                # built out of all the other .cpp files in the directory.
                program = env.Program(executableFilename,
                                      mainCppOnlyBuildDirectorySourceFileList, **locals)
                env.Requires(program, etagsFile)

                # Install the program in the top level bin directory.
                command = env.Command(os.path.join(targetDirectory, 'executable_installed'),
                            program, installExecutable)

            else:
                # Otherwise, we assume it's a library.
                
                library = env.Library(os.path.join(buildLibDirectory, baseDirectory),
                            buildDirectorySourceFileList, **locals)
                env.Requires(library, etagsFile)

baseEnv = Environment()

directoryDictionary = {}
etagsSourceFileList = []

findSourceFiles()

debug = ARGUMENTS.get('debug', '0') != '0'
opt = ARGUMENTS.get('opt', '0') != '0'
verbose = ARGUMENTS.get('verbose', '0') != '0'
tags = ARGUMENTS.get('tags', '1') != '0'

# If neither the debug or opt flags were specified, build only debug.
if not debug and not opt:
    debug=True

if not verbose:
    baseEnv.Append(PRINT_CMD_LINE_FUNC=printCommandLine)

# Completely rebuild the TAGS file whenever a source file changes.
etagsFile = os.path.join(baseBuildDirectory, 'TAGS')
if tags:
    baseEnv.Command(etagsFile, etagsSourceFileList, generateEtagsFile)

if debug:
    createTargets(debug=True, verbose=verbose)

if opt:
    createTargets(debug=False, verbose=verbose)

