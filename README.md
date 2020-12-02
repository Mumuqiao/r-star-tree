R* Tree for C++
===============

<<<<<<< HEAD
In 2008 I created this R* Tree implementation in C++. My goal for this
implementation was as follows:

* Simple API – Most existing implementations I’ve seen are far from simple
* Generic enough to use with most types, with low overhead
* N dimensions for bounding boxes
* Take advantage of modern C++ features to get decent performance

This is a header-only implementation of an R tree with an R* index, and makes
heavy use of templates, STL, and STL-style functors; but it should work in any
relatively modern C++ compiler. To do searching for nodes, this implementation
uses the Visitor pattern via functors — there are examples and documentation in
RStarVisitor.h.

Maintenance
===========

I do not use or update this code anymore, but I welcome pull requests.

License
=======

GNU Lesser Public License v2.1
=======
* 我对代码和注释的内容做出了一些改变，但为了与原来的代码和注释风格项匹配，我并没有将注释改为中文。
  但我对一些主要的功能做了一份中文的文档。

* I made some changes to the content of the code and comments, but I didn't change the comments to Chinese in order to match the original code and comment style items.
  But I made a document in Chinese for some of the main functions.
>>>>>>> 第一次修改

Author
======

<<<<<<< HEAD
Dustin Spicuzza (dustin@virtualroadside.com)
=======
original author: Dustin Spicuzza (dustin@virtualroadside.com)
>>>>>>> 第一次修改
