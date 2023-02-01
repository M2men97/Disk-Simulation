Final Project : Index Allocation Simulation 
Authored by Mo'men Abu Gharbieh

==Description==

in this program we have a small simulation for the disk by index allocation , a small explanation about index allocation : when we create a file the system will allocate an index block for it , this index block will contain the pointers to the used block for this file , for example if we used the block number 2,4,5,7 to save the data of the file , when we look at the index block we will see these numbers inside it .

in this program we have 3 classes :
class 1 : FsFile -> this class will save the data about the file size , block size , the number of the index block , how many used block for this file , and the block that is currently in use and his offset (we need them in the case of writing ) .
class 2 : FileDescriptor -> this class contain the name of the file , and the FsFile that belong to this file and a boolean variable (to check if the file is in use ) .
class 3 : FsDisk -> this class will represent our disk and it contain a pointer to , a file that we will save all the data in it , a boolean variable to check if the disk is formatted , the block size , BitVectorSize , BitVector array (that will save for us if the block is used or not ) , a MainDir map (that will connect between the file name and the FsFile of the file ) , OpenFileDescriptor vector (that will save for us the file descriptor of the open files ) .
in class 3 , we have 8 main function and 5 auxiliary function we will explain about them in the Functions section .

==Functions==
we have 13 functions :

auxiliary functions :

1. getFreeBlock : this function will search inside the BitVector array until it find the first 0 in it this would be the first free block in out disk .
2. setBitVector : this function will change the values of the BitVector from 0 to 1 if we are using the block , from 1 to 0 if we are deleting the data inside the block .
3. getBlockFromIndexBlock : this function will go to the index block in a specific position , and return the number of the block that is saved there .
4. setIndexBlockOffset : this function is the opposite of the previous function , it will write inside the index block the number of the used block .
5. deleteBlock : this function will take the index block of the file and how many blocks the file use and delete the content of them and set them to 0 in the BitVector array.

main functions :

1. ListAll : this function will print the content of disk .

2. FsFormat : this function will format our disk and set the block size and the number of blocks in our file .
(all the next functions require that the disk must be formatted first)

3. createFile : this function will create a new file , first we will check if the file is exist then we will check if there is a free block to make it the index block of the disk , then we will add the file to the MainDir , then we will find a suitable place to the file in the OpenFileDescriptor (if there is a free place in the middle of the OpenFileDescriptor vector caused by closing a file or deleting it we will use it for the new file and it will represent the file descriptor of the file ) or if there is no such a free place like we explained , we will add it to the end of the OpenFileDescriptor and also this place will represent the file descriptor of the file .

4. openFile : this function will take a file name and check if it's opened if not to open it , when we open a file we will make the same process that we used in the createFile function to get a new file descriptor for this file in the OpenFileDescriptor.

5. closeFile : this function will take file descriptor of a file and will check if the file is closed if not to close it , the mechanism of this function is : we will release the file descriptor of this file (we will make it usable , if we opened or created a file it will use it as his file descriptor ).

6. writeToFile : this function will take file descriptor and a buffer that contain the input that we want to write inside the file and the length of the buffer , the function will check a few things before start writing , first if the fils is available , then if the file is open and at the end if there is enough space inside the file to write on it .

7. deleteFile : this function will take a file name , and check if the file is closed if yes we will delete everything that belong to the file , if no we cant delete the file .

8. readFromFile : this function will take the same as writeToFile , a file file descriptor and a buffer that we want to read to it , and the length that we want to read , we will check if the file is exist , if its open , if the size of the file is big enough to read the asked length , and will print it to the screen .


==Program Files==
README.txt
sim_disk.cpp


==How to compile?==

g++ sim_disk.cpp -o sim_disk

==How to run the program==

./sim_disk

==Input:==

we have an option menu :

inserting 0 : will close the file and delete everything .

inserting 1 : will call the listAll function .

inserting 2 : will call the FsFormat function which will require to insert the block size and then the disk will be formatted .

inserting 3 : will call CreateFile function which will require to insert a name for the file .

inserting 4 : will call OpenFile function which will require to insert the name of the file .

inserting 5 : will call CloseFile function which will require the file descriptor of the file .

inserting 6 : will call WriteToFile function which will require the file descriptor of the file and then the input that we want to write inside the file .

inserting 7 : will call ReadFromFile function which will require the file descriptor of the file and then the length that we want to read .

inserting 8 : will call DeleteFile function which will require the file name that we want to delete . 

==Output:==

inserting 0 : NO OUTPUT .

inserting 1 : will show us to the screen all the content of the file .

inserting 2 : NO OUTPUT . 

inserting 3 : will show us the name of the file and his file descriptor .

inserting 4 : will show us the name of the file and his file descriptor . 

inserting 5 : will show us the name of the file and that the file has been closed .

inserting 6 : will show us how many bits has been written to the file . 

inserting 7 : will show us the bits that we asked for from the file .

inserting 8 : will show us the name of the file and that the file has been deleted 


