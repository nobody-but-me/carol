
#!/usr/bin/env bash

if [ -f ./build/bin/carol ]; then
    echo "[INFO]: RUNNING...\n"
    
    cd ./build/bin/
    
    ./carol
    
    cd ..
else
	echo  "[ERROR]: Could not run application: Executable does not exit or has some error. \n"
fi
