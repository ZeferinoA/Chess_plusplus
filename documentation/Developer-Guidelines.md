# Developer Guidelines

## Obtaining Source Code
The source code will all be sorted in the src file of the repository, and can be downloaded and organized from there on.

## Layout of Directory
The layout of the repository directory starts with the items out of folders, which are the living document, the readme that includes the idea and goal of the project, and the team resource markdown file. The source files are located in the src folder. This guide and the user manual are in the documentation folder. The weekly documentation and reports are located in the reports folder.

## Building the Software
A makefile will be provided so that with the source code downloaded. To run the makefile, just type `make` in the terminal in the directory that the source code was downloaded to. After this is ran, an executable is created with the name chess_game. To run this, simply put in the same terminal `./chess_game`

## Software Testing
A makefile for test will be provide in order to be able to run any in the test folder. When the make file creates the executable of `./chess_test`, you can run that from the command line for the source files.

## Adding New Test
The format to follow when adding to the testing folder will be [what its testing]_test.cpp 

## Building a Release
For building a release of the software, supply a zip file for every supported operating system under the releases page of the github repo with the executable in the releases page