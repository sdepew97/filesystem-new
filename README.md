# Names: Sarah Depew and Rose Lin

## Design:
  52 inodes in the inode region. The data region begins at block offset 12. Each block size is 512 bytes.
  Open file table is an array of size 20. So we are limiting the max number of open files and directory files to 20.
  The root directory file always occupies the first entry in the open file table.
  As for the file on the disk, we only allow writing to the end of idblocks.
  As for the directory files on the disk, we only allow 80 directories totally under any parent. That is to say that the directory files can only be written to the end of dblocks. Also, the number of inodes and data blocks are dependent upon the size of the disk you format. Format can be run for a default of 1MB with ./format DISK and ./format "-s #2" DISK for 2MB. Replacing the "2" with another value will give you larger, unit tested disks.  
  When we open a directory, we only keep the root directory and the parent directory open and put them into open file table.
  f_mount() an empty disk generated by ./format called DISK. This disk is requested to be formatted without error if it does not exist initially. 
  
  Our basic design stuck fairly similarly to the original. We added return types as we went along and used whatever seemed most appropriate. f_stat's return value did change.

## How to log in:
  Two users to choose from: super/basic
  Passwords for them respectively: suser/buser

## How to use format to format a disk:
  Run ./format <diskname> to get an empty disk of 1MB size.
  - For example, ./format DISK is going to give an empty disk named DISK.
  - For example, ./format "-s #2" DISK is going to give you an empty disk called DISK of size 2MB.
  Run ./formatdir <diskname>
  - For example, "./formatdir DISK" is going to give a disk with directory "/user/test.txt" named DISK

### ls: (completely implemented) 
  ls
  ls -F
  ls -l
  -- displaying the file information.
  -- file information content: file/directory name, filesize, uid, gid, ctime, mtime, atime, fileytpe(0 for directory files, and 1 for regular files), permission and inode_index

### chmod : 
  Permissions are not implemented, so this method does not work.
  
### cd: (completely implemented)
  cd .
  -- Stays at the current working directory.
  cd ..
  -- Jumps to parent directory and changes the current working directory at the same time.
  cd <absolute path>
  -- Jumps to the right places and changes the current working directory at the same time.
  cd <relative path>
  -- Jumps to the right places and changes the current working directory at the same time.
  cd
  -- Jumps to the root directory and changes the current working directory at the same time.

### pwd: (completely implemented)
  pwd
  
  Errors Known: There are invalid read and write errors with this method.
  
### mkdir: (completely implemented)
  mkdir <relative path>
  -- mkdir <filename>
  -- mkdir ./<filename>
  -- mkdir ../<filename>
  mkdir <absolute path>
  -- Creates the directory at the right place.
  
  Errors Known: There is a confusing uninitialized buffer error that occurs when this function runs. We think it has something to do with fseek. 
  
### rm: (completely implemented)
  rm <relative path>
  rm <absolute path>
  rm works on both empty directories and on empty files Syscall param write(buf) points to uninitialised byte(s) error under valgrind with. We think it may have something to do with fseek.

### rmdir: (completely implemented)
  rmdir <relative path>
  rmdir <absolute path>

### chmod:
  not implemented, since permissions are not implemented 

### cat:
  This is partially working. Cat and redirection are implemented. You can see redirection work with methods other than cat or the built ins. For example mail with redirection uses the linux builtin and works. Basic cat with no arguments and redirection works and so does cat on a file if you have one on the disk. You can make up to one file with like 10 characters and everything works. When you make a file, you have to exit the shell before you read it, otherwise you get double free errors and segfaults.

### more: 
  This is implemented fully, but hard to test without the ability to get files on the disk. If you can get two files on the disk, then you can more them.  More doesn't do paging, but does print out files all in one stream, block by block.
### mount:
  This functionality is not implemented.
  
### unmount:
  This functionality is not implemented. 
  
## How we teststed:
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
- Memory leaks are a problem on some of the methods. We were fixing these and they ended up breaking other parts of the shell. As a result, we left a few leaks there as to not break our code completely. However, we do know of their existence. 
- fseek() unitialized buffer error mentioned, above
- We can only mount our disk at the root directory, and mounting multiple disks at different directory is not implemented.
- Writing to i2 and i3 blocks of a file are not handled.
- When you cat >> with redirection and then attempt an ls -l, it segfaults
- There are issues with creating files that are more than one block. It is also giving errors when you try to create more than one file. Sometimes you can create multiple files with a single block and then use more to show these files contents. This can be used to show more and cat do work on files, but there is some miscommunication between f_write and f_read which we were unable to figure out how to fix.
