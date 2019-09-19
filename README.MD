C Documentation Generator
==========================================================

[![Build Status](https://travis-ci.org/maldan/cdocgen.svg?branch=master)](https://travis-ci.org/maldan/cdocgen)

# By now it's very unstable

# Demo
http://maldan.ru/osoyanlib/doc.html
This is generated document with documentation from comments of header files.

# Install

```
git clone --recurse-submodules https://github.com/maldan/cdocgen.git
cd cdocgen
chmod 0777 install.sh && sudo ./install.sh
```

# Usage
```
./cdocgen --include ../osoyanlib/include/ --out final.doc.html
```

Where 
* --include is folder with headers files.
* --out is destination for generated document, by default cdocgen generate document in doc.html.

By now program works only with header files. Here is example of comments https://github.com/maldan/osoyanlib, watch include folder. The program works only with this type of comments. You can comment file or functions. You must use exactly format of comment with spaces and etc. If at least one space is wrong the program will crash.
