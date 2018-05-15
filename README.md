# Names: Sarah Depew and Rose Lin

## Design:
  52 inodes in the inode region. The data region begins at block offset 12. Each block size is 512 bytes.<br/>
  Open file table is an array of size 20. So we are limiting the max number of open files and directory files to 20.<br/>
  The root directory file always occupies the first entry in the open file table.<br/>
  As for the file on the disk, we only allow writing to the end of idblocks.<br/>
  As for the directory files on the disk, we only allow 80 directories totally. That is to say that the directory files can only be written to the end of dblocks.<br/>
  When we open a directory, we only keep the root directory and the parent directory open and put them into open file table.<br/>
  f_mount() an empty disk generated by ./format<br/>

## How to log in:
  Two users to choose from: super/basic <br/>
  Passwords for them respectively: suser/buser<br/>

## How to use format to format a disk:
  Run ./format <diskname> to get an empty disk.
  - For example, "./format DISK" is going to give an empty disk named DISK.<br/>
  Run ./formatdir <diskname>
  - For example, "./formatdir DISK" is going to give a disk with directory "/user/test.txt" named DISK<br/>

### ls:
  ls <br/>
  -- Displays the file and directory names under current directory as desired.<br/>
  ls -F<br/>
  -- Displays the file and directory names under current directory as desired. <br/>
  ls -l<br/>
  -- Displays the file information.<br/>
  -- File information content: filename, filesize, uid, gid, ctime, mtime, atime, fileytpe(0 for directory files, and 1 for regular files), permission and inode_index<br/>

### cd:
  cd .<br/>
  -- Stays at the current working directory.<br/>
  cd ..<br/>
  -- Jumps to parent directory and changes the current working directory at the same time.<br/>
  cd <absolute path><br/>
  -- Jumps to the right places and changes the current working directory at the same time.<br/>
  cd <relative path><br/>
  -- Jumps to the right places and changes the current working directory at the same time.<br/>
  cd<br/>
  -- Jumps to the root directory and changes the current working directory at the same time.<br/>

### pwd:
  pwd<br/>
  -- Prints out the current working directory(absolute path). <br/>

### mkdir:
  mkdir <relative path><br/>
  -- mkdir <filename><br/>
  -- Creates the directory at the right palce.<br/>
  -- mkdir ./<filename><br/>
  -- Creates the directory at the right palce.<br/>
  -- mkdir ../<filename><br/>
  -- Creates the directory at the right place.<br/>
  mkdir <absolute path><br/>
  -- Creates the directory at the right palce.<br/>

### rm:
  rm <relative path><br/>
  -- Removes the file as desired. <br/>
  rm <absolute path><br/>
  -- Removes the file as desired. <br/>
  *rm works on both empty directories and on empty files<br/>*

### rmdir:
  rmdir <relative path><br/>
  -- Recursively removes the directory as desired. <br/>
  rmdir <absolute path><br/>
  -- Recursively removes the directory as desired. <br/>

### chmod:
  Not implemented.<br/>

### cat:

### mount:

### unmount:

## How to test:
#### shell function:
- test ls
- test cd
- test pwd
- test mkdir
- test rm
- test rmdir
- test cat
#### file system functions:
- test f_read
- test f_write
- test f_mount
- test f_unmount

## Known issues and limitations:
- gid and uid are not implemented
- ctime, mtime and atime are not implemented
- permission is not implemented.
- Mkdir will not prevent us from creating two directories with the same name.
- Memory leak
- fseek() unitialized
- We can only mount our disk at the root directory, and mounting multiple disks at different directory is not implemented.
- Writing to i2 and i3 blocks of a file are not handled.
