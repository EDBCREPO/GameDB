LIST=( backend frontend nginx pm )

#---------------------------------------------------------------------#

if [ ! -d "build" ]; then
    mkdir "build"
fi

#---------------------------------------------------------------------#

echo -e "compilling gameDB backend"
for ITEM in "${LIST[@]}" ; do

    if [ -f "./build/$ITEM" ]; then
        echo "-> compiled   $ITEM"; continue
    fi; echo "-> compilling $ITEM"

    g++ -o ./build/$ITEM ./service/$ITEM.cpp -I./include/backend -lssl -lcrypto -lz

    if [ ! $? -eq 0 ]; then
        echo "exit error"; exit;
    fi

done

#---------------------------------------------------------------------#

echo -e "compilling gameDB frontend"
FLAGS=(
    -s PTHREAD_POOL_SIZE=8
    -s FETCH=1 -s WASM=1
    -s USE_PTHREADS=1
    -s ASYNCIFY=1
)

SHELL="--shell-file ./view/assets/shell.html --bind"
INCLUDE="-I./include/frontend"
LIB="-pthread -lwebsocket.js"
FILE="./service/client.cpp"

em++ -o ./view/wasm.html $FILE $INCLUDE $LIB $SHELL "${FLAGS[@]}"

#---------------------------------------------------------------------#

echo -e "running gameDB" ; ./build/pm
