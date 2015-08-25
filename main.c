/*
main.c
User interface to tar function

Copyright (c) 2015 Jason Lee

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE
*/

#include <stdio.h>

#include "tar.h"

int main(int argc, char * argv[]){
    if (((argc == 2) && (strncmp(argv[1], "help", MAX(strlen(argv[1]), 4)))) && (argc != 3)){
        fprintf(stderr, "Usage: %s option(s) tarfile [sources]\n", argv[0]);
        fprintf(stderr, "Usage: %s help\n", argv[0]);
        return -1;
    }

    if (argc == 2){
        fprintf(stdout, "Usage: %s options(s) tarfile [sources]\n"\
                        "Usage: %s help\n"\
                        "\n"\
                        "Important:\n"\
                        "    This program is not meant to be a full tar implementation.\n"\
                        "    Only a subset of the functions the GNU tar utility has are supported.\n"
                        "\n"\
                        "    Special files that already exist will not be replaced when extracting (no error)\n"\
                        "    Regular expression expansion/matching is not done.\n"\
                        "\n"\
                        "    options (only one allowed at a time):\n"\
                        "        a - append files to archive\n"\
                        "        c - create a new archive\n"\
                        "        d - diff the tar file with the workding directory\n"\
                        "        l - list the files in the directory\n"\
                        "        r - remove files from the directory\n"\
                        "        u - update entries that have newer modification times\n"\
                        "        x - extract from archive\n"\
                        "\n"\
                        "    other options:\n"\
                        "        v - make operation verbose\n"\
                        "\n"\
                        "Ex: %s vl archive.tar\n"\
                      , argv[0], argv[0], argv[0]);
      return 0;
    }

    argc -= 3;

    int rc = 0;
    char a = 0,             // append
         c = 0,             // create
         d = 0,             // diff
         l = 0,             // list
         r = 0,             // remove
         u = 0,             // update
         x = 0;             // extract
    char verbosity = 0;     // 0: no print; 1: print file names; 2: print file properties

    // parse options
    for(int i = 0; argv[1][i]; i++){
        switch (argv[1][i]){
            case 'a': a = 1; break;
            case 'c': c = 1; break;
            case 'd': d = 1; break;
            case 'l': l = 1; break;
            case 'r': r = 1; break;
            case 'u': u = 1; break;
            case 'x': x = 1; break;
            case 'v': verbosity = 1; break;
            case '-': break;
            default:
                fprintf(stderr, "Error: Bad option: %c\n", argv[1][i]);
                fprintf(stderr, "Do '%s help' for help\n", argv[0]);
                return 0;
                break;
        }
    }

    // make sure only one of these options was selected
    const char used = a + c + d + l + r + u + x;
    if (used > 1){
        fprintf(stderr, "Error: Cannot have so all of these flags at once\n");
        return -1;
    }
    else if (used < 1){
        fprintf(stderr, "Error: Need one of 'acdlrux' options set\n");
        return -1;
    }

    const char * filename = argv[2];
    const char ** files = (const char **) &argv[3];

    // //////////////////////////////////////////

    struct tar_t * archive = NULL;
    int fd = -1;
    if (c){             // create new file
        if ((fd = open(filename, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IWUSR)) == -1){
            fprintf(stderr, "Error: Unable to open file %s\n", filename);
            return -1;
        }

        if (tar_write(fd, &archive, argc, files, verbosity) < 0){
            rc = -1;
        }
    }
    else{
        // open existing file
        if ((fd = open(filename, O_RDWR)) < 0){
            fprintf(stderr, "Error: Unable to open file %s\n", filename);
            return -1;
        }

        // read in data
        if (tar_read(fd, &archive, 0) < 0){
            tar_free(archive);
            close(fd);          // don't bother checking for fd < 0
            return -1;
        }

        // perform operation
        if ((a && (tar_write(fd, &archive, argc, files, verbosity) < 0))          ||  // append
            (d && (tar_diff(stdout, archive, verbosity) < 0))                     ||  // diff with current working directory
            (l && (tar_ls(stdout, archive, argc, files, verbosity + 1) < 0))      ||  // list entries
            (r && (tar_remove(fd, &archive, argc, files, verbosity) < 0))         ||  // remove entries
            (u && (tar_update(fd, &archive, argc, files, verbosity) < 0))         ||  // update entries
            (x && (tar_extract(fd, archive, argc, files, verbosity) < 0))             // extract entries
            ){
            fprintf(stderr, "Exiting with error due to previous error\n");
            rc = -1;
        }
    }

    tar_free(archive);
    close(fd);          // don't bother checking for fd < 0
    return rc;
}