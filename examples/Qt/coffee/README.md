### Prerequisites: 

- Install the Realm C++ SDK like so:
```
mkdir build
cd build
cmake -GNinja ..
ninja install
```
- Create a Atlas App Services app then do the following:
    - Enable anonymous authentication.
    - Enable Sync with Development Mode turned on.
- Replace `MY_APP_ID` in coffee_manager.cpp with your Atlas App ID.
- Run the project