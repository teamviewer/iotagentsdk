# Python Bindings

The Python bindings are a wrapper around the C++ Agent API and provide the same set of features:

- Connection to the local IoT Agent process
- `SessionManagement` : tracking incoming connections
- `AccessControl` : granular permissions for Remote Control, Remote View, File Transfer
- `InstantSupport` : requesting remote assistance
- `Chat`

## Building

If you followed the **Development** steps listed in the main [README](../../README.md), you should already have a newly compiled Python library in `Bindings/Python/tvagentapi.so`.

👉 Before building, remember to additionally install the `libpython3-dev` package, and specify the `-DENABLE_PYTHON_MODULE=ON` argument to CMake.

## Usage

You can now use the TeamViewer Agent SDK in your Python scripts:

```Python
import tvagentapi
...
api = tvagentapi.TVAgentAPI()
connection = api.createAgentConnectionLocal()
connection.setCallbacks(statusChanged=connectionStatusChanged)
...
```

Sample scripts are avalable at [../../examples/py/](../../examples/py/).

👉 Before running your script, make sure Python can find our library:

```bash
export PYTHONPATH=<...>/iotagentsdk/build/Bindings/Python
```

Or directly from your code:

```Python
import sys
...
sys.path.append('<...>/iotagentsdk/build/Bindings/Python')
```

You can also just place a copy of (or a symlink to) `tvagentapi.so` next to your scripts.

## Running the example scripts

You may need to provide various other environment variables before running individual example scripts. For instance, [`instant_support.py`](../../examples/py/instant_support.py) expects the `TV_ACCESS_TOKEN` environment variable to be set to a valid value (refer to **Module example #3: Instant Support** [here](../../examples/py/) on how to obtain a valid access token):

* `export TV_ACCESS_TOKEN="12345678-LgxKf0bybuAESdNIelrY"`

Consult the script you are trying to run to find out what variables may be required.

## Documentation

Make sure the `python-dev-is-python3` package is installed.

View the documentation:

```bash
pydoc3 tvagentapi
```

View the documentation as HTML:

```bash
pydoc3 -w tvagentapi
```

