#include <iostream>
#include <vector>
#include <map>
#include <cassert>
#include <cstring>
#include <cmath>
#include <fcntl.h>

using namespace std;

#define DISK_SIZE 256

// ============================================================================
void decToBinary(int n, char &c){ // array to store binary number
    int binaryNum[8];
    int i = 0; // counter for binary array
    while (n > 0){ // storing remainder in binary array
        binaryNum[i] = n % 2;
        n = n / 2;
        i++;
    }
    for (int j = i - 1; j >= 0; j--){ // printing binary array in reverse order
        if (binaryNum[j] == 1)
            c = c | 1u << j;
    }
}
// ============================================================================
class FsFile {
    int file_size;
    int block_size;
    int index_block;
    int block_in_use;
    int usedBlocks;
    int usedBlockOffset;

public:  FsFile(int _block_size) {
        file_size = 0;
        block_in_use = 0;
        block_size = _block_size;
        index_block = -1;
        usedBlocks = 0;
        usedBlockOffset = 0;
    }
    int getFileSize(){ return file_size; }
    void setFileSize(int nSize){ file_size = nSize; }
    void incFileSize(){ file_size++; }

    int getBlockSize(){ return block_size; }
    void setBlockSize(int nBlockSize){ block_size = nBlockSize; }

    int getIndexBlock(){ return index_block; }
    void setIndexBlock(int nIndexBlock){ index_block = nIndexBlock; }

    int getBlockInUse(){ return block_in_use; }
    void setBlockInUse(int nBlockInUse){ block_in_use = nBlockInUse; }

    int getUsedBlocks(){ return usedBlocks; }
    void setUsedBlock(int nUsedBlocks){ usedBlocks = nUsedBlocks; }
    void incUsedBlocks(){ usedBlocks++; }

    int getUsedBlockOffset(){ return usedBlockOffset; }
    void setUsedBlockOffset(int nUsedBlockOffset){ usedBlockOffset = nUsedBlockOffset; }
    void incUsedBlockOffset(){ usedBlockOffset++; }
};
// ============================================================================
class FileDescriptor {
    string file_name;
    FsFile* fs_file;
    bool inUse;
    int fd;

public: FileDescriptor(string FileName, FsFile* fsi) {
        file_name = FileName;
        fs_file = fsi;
        inUse = true;
        fd = -1;
    }
    string getFileName(){ return file_name; }
    void setFileName(string nFileName){ file_name = nFileName; }

    FsFile* getFsFile(){ return fs_file; }
    void setFsFile(FsFile* nFsFile){ fs_file = nFsFile; }

    bool getInUse(){return inUse; }
    void setInUse(bool nInUse){ inUse = nInUse; }

    int getFD(){ return fd;}
    void setFD(int nFD){fd = nFD;}

};
#define DISK_SIM_FILE "DISK_SIM_FILE.txt"
// ============================================================================

class fsDisk {
    FILE *sim_disk_fd;
    bool is_Formatted;
    int blockSize;

    int BitVectorSize;
    int *BitVector;
    // BitVector - "bit" (int) vector, indicate which block in the disk is free
    //              or not.  (i.e. if BitVector[0] == 1 , means that the
    //             first block is occupied.

    map<string,FileDescriptor*> MainDir;
    // Structure that links the file name to its FsFile

    vector<FileDescriptor*> OpenFileDescriptors ;
    //  when you open a file,
    // the operating system creates an entry to represent that file
    // This entry number is the file descriptor.

    // ------------------------------------------------------------------------
public: fsDisk() {
        sim_disk_fd = fopen(DISK_SIM_FILE , "w+");
        assert(sim_disk_fd);

        for (int i=0; i < DISK_SIZE ; i++) {
            int ret_val = fseek ( sim_disk_fd , i , SEEK_SET );
            ret_val = fwrite( "\0" , 1 , 1, sim_disk_fd);
            assert(ret_val == 1);
        }
        fflush(sim_disk_fd);
        is_Formatted = false;
    }
    int getFreeBlock(){ // TO GET THE FIRST FREE BLOCK
        for(int i=0;i<BitVectorSize;i++){
            if(BitVector[i] == 0){
                return i;
            }
        }
        return -1;
    }
    void setBitvector(int index , int val){ // THE USED BLOCK WILL BE 1 AND HE UNUSED WILL BE 0
        BitVector[index] = val;
    }
    int getBlockFromIndexBlock(int indexBlock , int offset){
        char block;
        fseek(sim_disk_fd,(indexBlock*blockSize)+offset,SEEK_SET);
        fread(&block,1,1,sim_disk_fd);
        int blockInUse = (int)block;
        return blockInUse-48;
    }
    void setIndexBlockOffset(int indexBlock , int block , int offset){
        char c = '0';
        decToBinary(block+48,c);
        fseek(sim_disk_fd,(indexBlock * blockSize) + offset , SEEK_SET);
        fwrite(&c,1,1,sim_disk_fd);
        fflush(sim_disk_fd);
    }
    void deleteBlock(int indexBlock , int usedBlock){
        setBitvector(indexBlock,0);
        for(int j=0;j<usedBlock;j++){
            int blockToDelete = getBlockFromIndexBlock(indexBlock,j);
            fseek(sim_disk_fd,(indexBlock*blockSize) + j , SEEK_SET);
            fwrite("\0",1,1,sim_disk_fd);
            setBitvector(blockToDelete,0);
            for(int i=0;i<blockSize;i++){
                fseek(sim_disk_fd,(blockToDelete*blockSize) + i , SEEK_SET);
                fwrite("\0",1,1,sim_disk_fd);
            }
        }
    }
    ~fsDisk(){
        delete[] BitVector; // DELETE BITVECTOR ARRAY
        for(auto file : MainDir){ // DELETE EVERYTHING LEFT IN MAINDIR
            delete file.second->getFsFile();
            delete file.second;
        }
        MainDir.clear();
        OpenFileDescriptors.clear();
        fclose(sim_disk_fd);
    }
    // ------------------------------------------------------------------------
    void listAll() {
        int i = 0;
        for (auto it = begin(MainDir) ; it != end(MainDir) ; ++it) {
            if(it->second == NULL){
                continue;
            }
            cout << "index: " << it->second->getFD() << ": FileName: " << it->first <<  " , isInUse: " << it->second->getInUse() << endl;
        }
        char buffer;
        cout << "Disk content: '";
        for (i = 0; i < DISK_SIZE; i++)
        {
            cout << "(";
            int ret_val = fseek(sim_disk_fd, i, SEEK_SET);
            ret_val = fread(&buffer, 1, 1, sim_disk_fd);
            cout << buffer;
            cout << ")";
        }
        cout << "'" << endl;
    }
    // ------------------------------------------------------------------------
    void fsFormat( int blockSize = 4 ) {
        if(is_Formatted){
            for(auto it = begin(MainDir); it != end(MainDir) ; ++it){
                delete it->second->getFsFile();
                delete it->second;
            }
            MainDir.clear();
            OpenFileDescriptors.clear();
        }
        is_Formatted = true;
        this->blockSize = blockSize;
        BitVectorSize = DISK_SIZE / blockSize;
        BitVector = new int[BitVectorSize];
        memset(BitVector,0,BitVectorSize);
    }
    // ------------------------------------------------------------------------
    int CreateFile(string fileName) {
        if(!is_Formatted){ // THE DISK NEEDS TO BE FORMATTED FIRST
            return -1;
        }
        auto file = MainDir.find(fileName);
        if(file != MainDir.end()){ // THERE IS A FILE WITH THE SAME NAME IN THE DISK
            return -1;
        }
        int indexBlock = getFreeBlock(); // GETS THE FIRST FREE BLOCK
        if(indexBlock == -1) { // DISK IS FULL
            return -1;
        }
        setBitvector(indexBlock,1);
        FsFile* nFsFile = new FsFile(blockSize); // NEW FsFile OBJECT
        nFsFile->setIndexBlock(indexBlock);
        FileDescriptor* nFD = new FileDescriptor(fileName,nFsFile); // NEW FILE DESCRIPTOR OBJECT
        MainDir.insert({fileName, nFD});
        int deletedIndex = -1;
        for(int i=0 ; i < OpenFileDescriptors.size() ; i++){ // IN CASE THERE WAS A FREE SPACE CAUSED BY DELETING A FILE
            if(OpenFileDescriptors[i] == NULL){
                deletedIndex = i;
                break;
            }
        }
        if(deletedIndex != -1){ // IN CASE WE FOUND A FREE SPACE CAUSED BY DELETING A FILE  OR UNUSED FILE
            nFD->setFD(deletedIndex);
            OpenFileDescriptors.at(deletedIndex) = nFD;
            return deletedIndex;
        }
        OpenFileDescriptors.push_back(nFD);
        nFD->setFD(OpenFileDescriptors.size() - 1);
        return (OpenFileDescriptors.size() - 1);
    }
    // ------------------------------------------------------------------------
    int OpenFile(string fileName) {
        if(!is_Formatted){ // CHECK IF THE DISK IS FORMATTED
            return -1;
        }
        auto is_Exist = MainDir.find(fileName);
        if( is_Exist == MainDir.end()){ // FILE NOT AVAILABLE
            return -1;
        }
        if(is_Exist->second->getInUse() == true){ // FILE ALREADY OPENED
            return -1;
        }
        int fd = 0;
        is_Exist->second->setInUse(true);
        for(auto file = OpenFileDescriptors.begin() ; file != OpenFileDescriptors.end() ; ++file){
            if((*file) == NULL){
                OpenFileDescriptors.at(fd) = is_Exist->second;
                is_Exist->second->setFD(fd);
                return fd;
            }
            fd++;
        }
        OpenFileDescriptors.push_back(is_Exist->second);
        is_Exist->second->setFD(OpenFileDescriptors.size() - 1);
        return OpenFileDescriptors.size() - 1;
    }
    // ------------------------------------------------------------------------
    string CloseFile(int fd) {
        if(!is_Formatted){ //  CHECK IF THE DISK IS FORMATTED
            return "-1";
        }
        if(OpenFileDescriptors.size() <= fd || OpenFileDescriptors[fd] == NULL){ // IF THE FILE IS NOT AVAILABLE
            return "-1";
        }
        auto file = OpenFileDescriptors[fd];
        if(file->getFD() == -1){ // FILE ALREADY CLOSED
            return "-1";
        }
        file->setInUse(false);
        file->setFD(-1);
        OpenFileDescriptors[fd] = NULL;
        return "1";
    }
    // ------------------------------------------------------------------------
    int WriteToFile(int fd, char *buf, int len ) {
        if(!is_Formatted){ // CHECK IF THE DISK IS FORMATTED
            return -1;
        }
        if(OpenFileDescriptors.size() <= fd || fd < 0){ // FILE NOT AVAILABLE
            return -1;
        }
        if(OpenFileDescriptors[fd] == NULL){ // FILE IS NOT OPENED
            return -1;
        }
        FsFile* file = OpenFileDescriptors[fd]->getFsFile(); // TO GET THE FILE THAT WE WANT TO WRITE TO
        string fileName = OpenFileDescriptors[fd]->getFileName();
        int fileSize = file->getFileSize();
        if(fileSize+len > (blockSize*blockSize)){
            len = (blockSize * blockSize) - fileSize;
        }
        int offset = file->getUsedBlockOffset(); // IN CASE WE ALREADY WROTE ON THE FILE
        int written = 0;
        for(int i=0 ; i<len ; i++){
            int freeSpaceInBlock = blockSize - (fileSize % blockSize);
            if(freeSpaceInBlock == blockSize){
                int nBlock = getFreeBlock(); // NEW BLOCK
                if(nBlock == -1){ // DISK IS FULL
                    break;
                }
                setBitvector(nBlock,1);
                file->setBlockInUse(nBlock); // SET THE NEW BLOCK AS IN USE BLOCK
                setIndexBlockOffset(file->getIndexBlock(),nBlock,file->getUsedBlocks());
                file->incUsedBlocks();
                offset = 0;
            }
            fseek(sim_disk_fd,(file->getBlockInUse()*blockSize)+offset,SEEK_SET);
            fwrite(&buf[i],1,1,sim_disk_fd);
            written++;
            fileSize++;
            file->setFileSize(fileSize);
            offset++;
            file->setUsedBlockOffset(offset);
        }
        return written;
    }
    // ------------------------------------------------------------------------
    int DelFile( string FileName ) {
        if(!is_Formatted){ // THE DISK NEEDS TO BE FORMATTED FIRST
            return -1;
        }
        auto file = MainDir.find(FileName);
        if(file == MainDir.end()) { // FILE NOT EXIST
            return -1;
        }
        if(file->second->getInUse()){ // THE FILE IS OPEN , CAN NOT DELETE THE FILE
            return -1;
        }
        int indexBlock = file->second->getFsFile()->getIndexBlock(); // TO GET THE INDEX BLOCK OF THE FILE
        int usedBlocks = file->second->getFsFile()->getUsedBlocks(); // TO GET HOW MANY BLOCKS ARE USED FOR THIS FILE
        deleteBlock(indexBlock,usedBlocks); // DELETE FUNCTION
        delete file->second->getFsFile();
        delete file->second;
        MainDir.erase(file);
        return 1;
    }
    // ------------------------------------------------------------------------
    int ReadFromFile(int fd, char *buf, int len ) {
        if(!is_Formatted){ // IF THE DISK IS NOT FORMATTED
            return -1;
        }
        if(OpenFileDescriptors.size() <= fd){ // FILE NOT AVAILABLE
            return -1;
        }
        if(OpenFileDescriptors[fd] == NULL){ // FILE IS NOT AVAILABLE
            return -1;
        }
        FsFile* file = OpenFileDescriptors[fd]->getFsFile();
        if(len > file->getFileSize()){ // THE ASKED LENGTH IS GREATER THAN THE FILE SIZE THE DISK WILL READ THE WHOLE FILE
            len = file->getFileSize();
        }
        int indexBlock = file->getIndexBlock();
        int indexBlockOffset = 0;
        int blockInUse = getBlockFromIndexBlock(indexBlock, indexBlockOffset);
        int offset = 0;
        for(int i=0;i<len;i++){
            if(offset == blockSize){
                offset = 0;
                indexBlockOffset++;
                blockInUse = getBlockFromIndexBlock(indexBlock, indexBlockOffset);
            }
            fseek(sim_disk_fd,(blockInUse * blockSize) + offset , SEEK_SET);
            fread(buf + i ,1,1,sim_disk_fd);
            offset++;
        }
        buf[len] = '\0';
        return fd;
    }
};
// ------------------------------------------------------------------------
int main() {
    int blockSize;
    int direct_entries; // UNUSED
    string fileName;
    char str_to_write[DISK_SIZE];
    char str_to_read[DISK_SIZE];
    int size_to_read;
    int _fd;

    fsDisk *fs = new fsDisk();
    int cmd_;
    while(1) {
        cin >> cmd_;
        switch (cmd_)
        {
            case 0:   // exit
                delete fs;
                exit(0);
                break;

            case 1:  // list-file
                fs->listAll();
                break;

            case 2:    // format
                cin >> blockSize;
                fs->fsFormat(blockSize);
                break;

            case 3:    // creat-file
                cin >> fileName;
                _fd = fs->CreateFile(fileName);
                cout << "CreateFile: " << fileName << " with File Descriptor #: " << _fd << endl;
                break;

            case 4:  // open-file
                cin >> fileName;
                _fd = fs->OpenFile(fileName);
                cout << "OpenFile: " << fileName << " with File Descriptor #: " << _fd << endl;
                break;

            case 5:  // close-file
                cin >> _fd;
                fileName = fs->CloseFile(_fd);
                cout << "CloseFile: " << fileName << " with File Descriptor #: " << _fd << endl;
                break;

            case 6:   // write-file
                cin >> _fd;
                cin >> str_to_write;
                cout<<"WriteToFile : "<<fs->WriteToFile( _fd , str_to_write , strlen(str_to_write) )<<endl;
                break;

            case 7:    // read-file
                cin >> _fd;
                cin >> size_to_read ;
                fs->ReadFromFile( _fd , str_to_read , size_to_read );
                cout << "ReadFromFile: " << str_to_read << endl;
                break;

            case 8:   // delete file
                cin >> fileName;
                _fd = fs->DelFile(fileName);
                cout << "DeletedFile: " << fileName << " with File Descriptor #: " << _fd << endl;
                break;
            default:
                break;
        }
    }
}
