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
- Use the `coffee.pro` file to open the project in Qt Creator.
- Replace `MY_APP_ID` in coffee_manager.cpp with your Atlas App ID.
- Build and run!