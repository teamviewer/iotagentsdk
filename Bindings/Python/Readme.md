# Python Bindings

## Synopsys
The python bindings are currently at an experimental stage. The bindings cover the full functionality
set the AgentAPI provides.

## System Requirements
For building the python bindings, the following libraries need to be available in addition to the ones
mentioned in the main README:

* `libpython3-dev`

## Configuration
The bindings are disabled by default, but can be enabled during the CMake configuration step:

* Enable python bindings: Set `ENABLE_PYTHON_MODULE` to `ON` (e.g. by adding `-DENABLE_PYTHON_MODULE=ON` to the CMake command line)

## Usage
The tests and examples require the `tvagentapi.so` binary to be accessible via the paths listed in the `sys.path` variable.
This can be achieved either by copying/installing the module to one of the respective directories, or by setting
the environment variable `PYTHONPATH` to the directory containing the newly built `tvagentapi.so` binary (see below).
The built python binary is in the directory `Bindings/Python`, which results in the following command:
* `export PYTHONPATH=<pathToBuildFolder>/Bindings/Python`

Other input parameters within the examples and tests are specified via environment variables too.
Please consult the executable python scripts to retrieve the names of variables for the respective script.

## Troubleshooting

### Missing system requirements
If the `libpython3-dev` package is not installed you will receive an error like:
* Could NOT find Python3 (missing: Python3_INCLUDE_DIRS Development) (found suitable version "3.8", minimum required is "3.8")

#### Solution
Install libpython3-dev on your build system.

### Missing tvagentapi.so
If `sys.path` does not include the path to the directory where `tvagentapi.so` is located (e.g. the PYTHONPATH variable
is not set or the path is wrong), you receive an error like:
* ModuleNotFoundError: No module named 'tvagentapi'

#### Solution
Set the `PYTHONPATH` variable to the location of the `tvagentapi.so` with `<buildFolder>/Bindings/Python`
* `export PYTHONPATH=<pathToBuildFolder>/Bindings/Python`

### Missing script parameter
If the script expects input parameters (e.g. an access token), it ensures these parameters are available before continuing.
If the parameter is not set, you will receive an error like:
* AssertionError: Specify TV_ACCESS_TOKEN environment variable

#### Solution
Set the expected environment variable before running the script
* `export TV_ACCESS_TOKEN="12345678-LgxKf0bybuAESdNIelrY"`
