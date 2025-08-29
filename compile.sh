LIST=( backend frontend nginx pm )

if [ ! -d "build" ]; then
    mkdir "build"
fi

echo -e "compilling playroom backend"
for ITEM in "${LIST[@]}" ; do

    if [ -f "./build/$ITEM" ]; then
        echo "-> compiled   $ITEM"; continue
    fi; echo "-> compilling $ITEM"

    g++ -o ./build/$ITEM ./service/$ITEM.cpp -I./include/backend -lssl -lcrypto -lz

    if [ ! $? -eq 0 ]; then
        echo "exit error"; exit;
    fi

done

echo -e "compilling playroom frontend"
# place the logic here

echo -e "running playRoom" ; ./build/pm
