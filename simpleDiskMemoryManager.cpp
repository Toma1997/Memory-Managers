#include <bits/stdc++.h>
#include <stdio.h>
using namespace std;

// Convert all Gb and Mb to Kb if necessary.
int converter(string valueOfBytes) {
    int size = valueOfBytes.length();
    string byt = valueOfBytes.substr(size-2,size);
    int value =  stoi(valueOfBytes.substr(0,size-2));
    if (byt == "Mb")
        value *= 1024;
    else if (byt == "Gb")
        value *= 1024*1024;
    return value;
}

// Insert file in memory blocks
bool insert(int &fileSize, string fileName, int *diskBlocks,  map<string, vector<vector<int>>> &filePointers, int &currentSize, const int &blockMaxSize) {
    for (int i = 0;i < 8; i++) 	{
        if (diskBlocks[i] >= fileSize) { // Free space in block is >= fileSize.
            diskBlocks[i] -= fileSize;
            filePointers[fileName] = {{i, fileSize}}; // Value {blockIndex, file size part in that block}.
            currentSize += fileSize;
            return true;
        } else { 
            // File can be inserted in 2 blocks, so make chunks of file.
            if (i<7  &&  diskBlocks[i] > 0 && diskBlocks[i+1] >= fileSize - diskBlocks[i]) {
                filePointers[fileName] = {{i, diskBlocks[i]}, {i+1, fileSize - diskBlocks[i]}};
                diskBlocks[i+1] -= fileSize - diskBlocks[i];
                diskBlocks[i] = 0;
                currentSize += fileSize;
                return true;
            }
            // File must be chunked in more than 2 blocks.
            if (i<7  &&  diskBlocks[i] > 0  &&  diskBlocks[i+1] == blockMaxSize  &&  fileSize >= 2*blockMaxSize) {
                int restBlocks = (fileSize - diskBlocks[i])/blockMaxSize;
                int pieceOfLastBlock = (fileSize - diskBlocks[i]) % blockMaxSize;
                int indeksBlokBound = i + restBlocks + 1;
                if (pieceOfLastBlock > 0)
                    indeksBlokBound += 1;
                if (indeksBlokBound <= 8) {
                    int freeSpace = 0;
                    for (int ind = i;ind < indeksBlokBound; ind++) 	{
                        freeSpace += diskBlocks[ind];
                    }
                    // Free space must be >= than part of fileSize.
                    if (freeSpace >= diskBlocks[i] + restBlocks * blockMaxSize + pieceOfLastBlock) {
                        filePointers[fileName] = {{i, diskBlocks[i]}, {indeksBlokBound-1, blockMaxSize}};
                        if (pieceOfLastBlock > 0) { // In last block is only part occupied.
                            diskBlocks[indeksBlokBound-1] -= pieceOfLastBlock;
                            filePointers[fileName][1][1] = pieceOfLastBlock;
                            indeksBlokBound--;
                        }
                        diskBlocks[i] = 0;
                        i++;
                        while (i < indeksBlokBound) {
                            diskBlocks[i] = 0;
                            i++;
                        }
                        currentSize += fileSize;
                        return true;
                    }
                } else { // File is bigger than current free space.
                    return false;
                }
            }
        }
    }
    return false;
}

void remov(int *diskBlocks, map<string, vector<vector<int>>>& filePointers, string &fileName, int &currentSize, const int &blockMaxSize, bool &wasRemoveAfterOptimize){
    vector<vector<int>> fileStartEndSize = filePointers[fileName];
    if (fileStartEndSize.size() != 0) {
        if (fileStartEndSize.size() > 1) { // File is in next several block.
            vector<int> start = fileStartEndSize[0];
            vector<int> end = fileStartEndSize[1];
            diskBlocks[start[0]] += start[1];
            diskBlocks[end[0]] += end[1];
            currentSize -= start[1] + end[1];
            int lowerBound = start[0] + 1;
            int upperBound = end[0] - 1;
            while (lowerBound <= upperBound) {
                diskBlocks[lowerBound] = blockMaxSize;
                currentSize -= blockMaxSize;
                if (lowerBound != upperBound) {
                    diskBlocks[upperBound] = blockMaxSize;
                    currentSize -= blockMaxSize;
                    upperBound--;
                }
                lowerBound++;
            }
        }
        else { // File is in one block.
            diskBlocks[fileStartEndSize[0][0]] += fileStartEndSize[0][1];
            currentSize -= fileStartEndSize[0][1];
        }
        filePointers.erase(fileName); 
        wasRemoveAfterOptimize = true;
    }
}

// Move all files left to the beginnig and next to each other removing the slack space between files.
// So after last file, memory will have complete free space merged.
void optimize(int *diskBlocks, map<string, vector<vector<int>>>& filePointers, int &currentSize, const int &blockMaxSize) {
    int firstFreeBlock = 0;
    int fileSize;
    for (int i = 0;i < 8; i++) 	{
        if (diskBlocks[i] < blockMaxSize) {// Founded block with files.
            string fileOnMultipleBlocks = "";
            string f;
            for ( const auto &filePointersKey : filePointers ) {
                f = filePointersKey.first;
                if (filePointers[f][0][0] == i) {
                    if (filePointers[f].size() == 1) { // If file is in only one block.
                        fileSize = filePointers[f][0][1];
                        diskBlocks[i] += fileSize;
                        currentSize -= fileSize;
                        filePointers.erase(f);
                        insert(fileSize, f, diskBlocks, filePointers, currentSize, blockMaxSize);
                    } else {
                        fileOnMultipleBlocks = f;
                    }
                }
            }
            // File which is in the current block and he occupied next several blocks will go after all others which are in the current block.
            if (fileOnMultipleBlocks != "" and filePointers[fileOnMultipleBlocks][0][0] == i) { 
                fileSize = filePointers[fileOnMultipleBlocks][0][1] + filePointers[fileOnMultipleBlocks][1][1];
                int additionalBlocks = filePointers[fileOnMultipleBlocks][1][0] - filePointers[fileOnMultipleBlocks][0][0] - 1;
                fileSize += additionalBlocks * blockMaxSize;
                int startInd = filePointers[fileOnMultipleBlocks][0][0];
                int endInd = filePointers[fileOnMultipleBlocks][1][0];
                diskBlocks[startInd] += filePointers[fileOnMultipleBlocks][0][1];
                diskBlocks[endInd] += filePointers[fileOnMultipleBlocks][1][1];
                filePointers.erase(fileOnMultipleBlocks);
                startInd += 1;
                endInd -= 1;
                while (startInd <= endInd){
                    diskBlocks[startInd] = blockMaxSize;
                    if (startInd != endInd) {
                        diskBlocks[endInd] = blockMaxSize;
                        endInd -=1;
                    }
                    startInd += 1;
                }
                currentSize -= fileSize;
                insert(fileSize, fileOnMultipleBlocks, diskBlocks, filePointers, currentSize, blockMaxSize);
            }
        }
    }
}

int main() {
    ios_base::sync_with_stdio(false); // faster I/O operations
    cin.tie(NULL);
    vector<string> outputs;
    int N, maxDiskSize;
    string diskSize, command, fileName, T;
    while (true) {
        cin >> N;
        if (N == 0) break;
        cin >> diskSize;
        maxDiskSize = converter(diskSize); // disk size Kb, Mb, Gb converted to Kb
        map<string, vector<vector<int>>> filePointers;
        int currentSize = 0;
        int blockMaxSize = maxDiskSize/8;
        bool error = false;
        bool wasRemoveAfterOptimize = false;
        
        // Memory is separated to 8 blocks of equal size.
        int diskBlocks[] = {blockMaxSize,blockMaxSize,blockMaxSize,blockMaxSize,
                            blockMaxSize,blockMaxSize,blockMaxSize,blockMaxSize};
        for (int index = 0;index < N; index++) 	{
            cin >> command; // insert, optimize, remove
            if (error)  {
                getline(cin, command, '\n');
                continue;
            }
            int fileSize;
            if (command == "insert")    {
                cin >> fileName; // NAME
                cin >> T;        // T => 1Kb,1Gb,...
                fileSize = converter(T);
                if (fileSize > maxDiskSize - currentSize) {
                    printf("%s \n", "ERROR: Disk is full!");
                    error = true;
                    continue;
                }
                int fileInserted = insert(fileSize, fileName, diskBlocks, filePointers, currentSize, blockMaxSize);
                    // Execute optimize to free space.
                    if (!fileInserted && wasRemoveAfterOptimize) {
                        optimize(diskBlocks, filePointers, currentSize, blockMaxSize);
                        wasRemoveAfterOptimize = false;
                        insert(fileSize, fileName, diskBlocks, filePointers, currentSize, blockMaxSize);
                    }
            } else if (command == "remove") {
                cin >> fileName; // NAME
                if (filePointers.count(fileName) > 0) {
                    remov(diskBlocks, filePointers, fileName, currentSize, blockMaxSize, wasRemoveAfterOptimize);
                }
            } else if (command == "optimize") {
                if (wasRemoveAfterOptimize) {
                    optimize(diskBlocks, filePointers, currentSize, blockMaxSize);
                    wasRemoveAfterOptimize = false;
                }
            }
        }
        if (!error) { // If there wasn't error for full disk, estimate and print state of free space in the blocks.
            string diskGraphic = "";
            int percentage;
            for (int block : diskBlocks)  {
                percentage = ((double)block/blockMaxSize)*100;
                if (percentage>=0 && percentage<=25)
                    diskGraphic += "[#]";
                else if (percentage>25 && percentage<=75)
                    diskGraphic += "[-]";
                else if (percentage>75 && percentage<=100)
                    diskGraphic += "[ ]";
            }
            printf("%s \n", diskGraphic.c_str());
        }
    }
	return 0;
}